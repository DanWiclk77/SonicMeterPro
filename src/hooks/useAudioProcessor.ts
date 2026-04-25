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
}

export function useAudioProcessor() {
  const [isActive, setIsActive] = useState(false);
  const [gain, setGain] = useState(0); 
  const [vuCalibration, setVuCalibration] = useState(-18);
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
    vu: -20
  });

  const audioCtxRef = useRef<AudioContext | null>(null);
  const analyserRef = useRef<AnalyserNode | null>(null);
  const gainNodeRef = useRef<GainNode | null>(null);
  const streamRef = useRef<MediaStream | null>(null);
  const rafRef = useRef<number | null>(null);

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
    }));
  }, []);

  const start = useCallback(async () => {
    try {
      const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
      streamRef.current = stream;

      const audioCtx = new (window.AudioContext || (window as any).webkitAudioContext)();
      audioCtxRef.current = audioCtx;

      const source = audioCtx.createMediaStreamSource(stream);
      const analyser = audioCtx.createAnalyser();
      analyser.fftSize = 2048;
      analyserRef.current = analyser;

      const gainNode = audioCtx.createGain();
      gainNodeRef.current = gainNode;

      source.connect(gainNode);
      gainNode.connect(analyser);

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
    if (gainNodeRef.current) gainNodeRef.current.gain.value = Math.pow(10, gain / 20);
  }, [gain]);

  useEffect(() => {
    if (!isActive || !analyserRef.current) return;

    const bufferLength = analyserRef.current.fftSize;
    const dataArray = new Float32Array(bufferLength);
    const sampleRate = audioCtxRef.current?.sampleRate || 44100;
    
    const update = () => {
      if (!analyserRef.current) return;
      analyserRef.current.getFloatTimeDomainData(dataArray);

      let sumSquares = 0;
      let currentPeak = 0;

      for (let i = 0; i < bufferLength; i++) {
        const val = dataArray[i];
        const absVal = Math.abs(val);
        if (absVal > currentPeak) currentPeak = absVal;
        sumSquares += val * val;
      }

      const rmsLinear = Math.sqrt(sumSquares / bufferLength);
      const peakDb = 20 * Math.log10(currentPeak || 1e-10);
      const mntDb = 20 * Math.log10(rmsLinear || 1e-10) + 0.69; // Simple k-weight offset

      // Track Maxima
      if (peakDb > peakMaxRef.current) peakMaxRef.current = peakDb;
      if (mntDb > momentaryMaxRef.current) momentaryMaxRef.current = mntDb;

      // Sliding Window for Short Term (ST) - approx 3 seconds
      // Buffer length 2048 at 48k is ~42ms per update. 3s = ~72 updates
      stBuffer.current.push(rmsLinear);
      if (stBuffer.current.length > 72) stBuffer.current.shift();

      const stRmsSum = stBuffer.current.reduce((a, b) => a + b * b, 0);
      const stRmsLinear = Math.sqrt(stRmsSum / stBuffer.current.length);
      const stLufs = 20 * Math.log10(stRmsLinear || 1e-10) + 0.69;
      
      if (stLufs > shortTermMaxRef.current) shortTermMaxRef.current = stLufs;

      // Integrated History
      integratedHistory.current.push(rmsLinear);
      // Simple Integrated: Average of all non-gated samples
      const gatedHistory = integratedHistory.current.filter(v => v > 0.00001); // Very basic gate
      const intRmsSum = gatedHistory.reduce((a, b) => a + b * b, 0);
      const intRmsLinear = Math.sqrt(intRmsSum / (gatedHistory.length || 1));
      const intLufs = 20 * Math.log10(intRmsLinear || 1e-10) + 0.69;

      // LRA (Simplified: Range of momentary values)
      const sortedInt = [...gatedHistory].sort();
      const p10 = sortedInt[Math.floor(sortedInt.length * 0.1)] || 0;
      const p95 = sortedInt[Math.floor(sortedInt.length * 0.95)] || 1;
      const lra = Math.abs(20 * Math.log10(p95 / (p10 || 1e-10)));

      setMetrics(prev => {
        const targetVu = peakDb - vuCalibration;
        const vu = prev.vu + (targetVu - prev.vu) * 0.1;
        
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
          vu: Math.max(-20, Math.min(3, vu))
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
    vuCalibration, 
    setVuCalibration,
    resetMetrics 
  };
}
