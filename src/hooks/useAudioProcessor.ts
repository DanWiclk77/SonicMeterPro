import { useEffect, useRef, useState, useCallback } from 'react';

export interface MeterData {
  peak: number;
  peakMax: number;
  rms: number;
  momentaryLufs: number;
  momentaryMax: number;
  shortTermLufs: number;
  shortTermMax: number;
  integratedLufs: number;
  loudnessRange: number;
  plrShortTerm: number;
  plrIntegrated: number;
  vu: number;
  history: number[];
  correlation: number; // -1 to 1
  stereoWidth: number; // 0 to 1
}

export function useAudioProcessor() {
  const [isActive, setIsActive] = useState(false);
  const [gain, setGain] = useState(0); 
  const [gainB, setGainB] = useState(0); 
  const [isUsingB, setIsUsingB] = useState(false);
  const [vuCalibration, setVuCalibration] = useState(-18);
  const [targetLufs, setTargetLufs] = useState(-14); // Default YouTube/Spotify
  const [metrics, setMetrics] = useState<MeterData>({
    peak: -100,
    peakMax: -100,
    rms: -100,
    momentaryLufs: -100,
    momentaryMax: -100,
    shortTermLufs: -100,
    shortTermMax: -100,
    integratedLufs: -100,
    loudnessRange: 0,
    plrShortTerm: 0,
    plrIntegrated: 0,
    vu: -20,
    history: new Array(100).fill(-70),
    correlation: 0,
    stereoWidth: 0
  });

  const audioCtxRef = useRef<AudioContext | null>(null);
  const analyserRef = useRef<AnalyserNode | null>(null);
  const analyserRRef = useRef<AnalyserNode | null>(null); // For stereo analysis
  const gainNodeRef = useRef<GainNode | null>(null);
  const streamRef = useRef<MediaStream | null>(null);
  const rafRef = useRef<number | null>(null);

  const currentGain = isUsingB ? gainB : gain;
  
  // Buffer for ST (3s) and Momentary (400ms)
  const stBuffer = useRef<number[]>([]);
  const integratedHistory = useRef<number[]>([]);
  const peakMaxRef = useRef(-100);
  const momentaryMaxRef = useRef(-100);
  const shortTermMaxRef = useRef(-100);

  const resetMetrics = useCallback(() => {
    stBuffer.current = [];
    integratedHistory.current = [];
    peakMaxRef.current = -100;
    momentaryMaxRef.current = -100;
    shortTermMaxRef.current = -100;
    setMetrics(prev => ({
      ...prev,
      peakMax: -100,
      momentaryMax: -100,
      shortTermMax: -100,
      integratedLufs: -100,
      loudnessRange: 0,
      history: new Array(100).fill(-70)
    }));
  }, []);

  const start = useCallback(async () => {
    try {
      const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
      streamRef.current = stream;

      const audioCtx = new (window.AudioContext || (window as any).webkitAudioContext)();
      audioCtxRef.current = audioCtx;

      const source = audioCtx.createMediaStreamSource(stream);
      
      // Split stereo for analysis
      const splitter = audioCtx.createChannelSplitter(2);
      source.connect(splitter);

      const analyserL = audioCtx.createAnalyser();
      const analyserR = audioCtx.createAnalyser();
      analyserL.fftSize = 2048;
      analyserR.fftSize = 2048;
      
      splitter.connect(analyserL, 0);
      splitter.connect(analyserR, 1);
      
      analyserRef.current = analyserL;
      analyserRRef.current = analyserR;

      const gainNode = audioCtx.createGain();
      gainNodeRef.current = gainNode;
      source.connect(gainNode);

      setIsActive(true);
    } catch (err) {
      console.error('Microphone access denied:', err);
    }
  }, []);

  const stop = useCallback(() => {
    if (rafRef.current) cancelAnimationFrame(rafRef.current);
    if (streamRef.current) streamRef.current.getTracks().forEach(t => t.stop());
    if (audioCtxRef.current) audioCtxRef.current.close();
    setIsActive(false);
  }, []);

  useEffect(() => {
    if (gainNodeRef.current) gainNodeRef.current.gain.value = Math.pow(10, currentGain / 20);
  }, [currentGain]);

  useEffect(() => {
    if (!isActive || !analyserRef.current || !analyserRRef.current) return;

    const bufferLength = analyserRef.current.fftSize;
    const dataL = new Float32Array(bufferLength);
    const dataR = new Float32Array(bufferLength);
    
    const update = () => {
      if (!analyserRef.current || !analyserRRef.current) return;
      analyserRef.current.getFloatTimeDomainData(dataL);
      analyserRRef.current.getFloatTimeDomainData(dataR);

      let sumSquaresL = 0;
      let currentPeakL = 0;
      let dotProduct = 0;
      let magL = 0;
      let magR = 0;

      for (let i = 0; i < bufferLength; i++) {
        const valL = dataL[i];
        const valR = dataR[i];
        
        sumSquaresL += valL * valL;
        if (Math.abs(valL) > currentPeakL) currentPeakL = Math.abs(valL);
        
        // Correlation & Width
        dotProduct += valL * valR;
        magL += valL * valL;
        magR += valR * valR;
      }

      const correlation = dotProduct / (Math.sqrt(magL * magR) || 1);
      const rmsLinear = Math.sqrt(sumSquaresL / bufferLength);
      const peakDb = 20 * Math.log10(currentPeakL || 1e-10);
      const mntDb = 20 * Math.log10(rmsLinear || 1e-10) + 0.69;

      if (peakDb > peakMaxRef.current) peakMaxRef.current = peakDb;
      if (mntDb > momentaryMaxRef.current) momentaryMaxRef.current = mntDb;

      stBuffer.current.push(rmsLinear);
      if (stBuffer.current.length > 72) stBuffer.current.shift();

      const stRmsSum = stBuffer.current.reduce((a, b) => a + b * b, 0);
      const stRmsLinear = Math.sqrt(stRmsSum / stBuffer.current.length);
      const stLufs = 20 * Math.log10(stRmsLinear || 1e-10) + 0.69;
      
      if (stLufs > shortTermMaxRef.current) shortTermMaxRef.current = stLufs;

      integratedHistory.current.push(rmsLinear);
      const gatedHistory = integratedHistory.current.filter(v => v > 0.00001);
      const intRmsSum = gatedHistory.reduce((a, b) => a + b * b, 0);
      const intRmsLinear = Math.sqrt(intRmsSum / (gatedHistory.length || 1));
      const intLufs = 20 * Math.log10(intRmsLinear || 1e-10) + 0.69;

      const sortedInt = [...gatedHistory].sort();
      const p10 = sortedInt[Math.floor(sortedInt.length * 0.1)] || 0;
      const p95 = sortedInt[Math.floor(sortedInt.length * 0.95)] || 1;
      const lra = Math.abs(20 * Math.log10(p95 / (p10 || 1e-10)));

      setMetrics(prev => {
        const targetVu = peakDb - vuCalibration;
        const vu = prev.vu + (targetVu - prev.vu) * 0.1;
        const updatedHistory = [...prev.history, stLufs];
        if (updatedHistory.length > 100) updatedHistory.shift();
        
        return {
          peak: peakDb,
          peakMax: peakMaxRef.current,
          rms: 20 * Math.log10(rmsLinear || 1e-10),
          momentaryLufs: mntDb,
          momentaryMax: momentaryMaxRef.current,
          shortTermLufs: stLufs,
          shortTermMax: shortTermMaxRef.current,
          integratedLufs: intLufs,
          loudnessRange: lra,
          plrShortTerm: peakDb - stLufs,
          plrIntegrated: peakMaxRef.current - intLufs,
          vu: Math.max(-20, Math.min(3, vu)),
          history: updatedHistory,
          correlation,
          stereoWidth: Math.abs(1 - (correlation + 1) / 2) // Rough width estimation
        };
      });

      rafRef.current = requestAnimationFrame(update);
    };

    rafRef.current = requestAnimationFrame(update);
    return () => {
      if (rafRef.current) cancelAnimationFrame(rafRef.current);
    };
  }, [isActive, vuCalibration]);

  return { 
    isActive, 
    start, 
    stop, 
    metrics, 
    gain, 
    setGain, 
    gainB,
    setGainB,
    isUsingB,
    setIsUsingB,
    targetLufs,
    setTargetLufs,
    vuCalibration, 
    setVuCalibration,
    resetMetrics 
  };
}
