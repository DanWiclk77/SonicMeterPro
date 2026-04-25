/**
 * @license
 * SPDX-License-Identifier: Apache-2.0
 */

import React, { useState } from 'react';
import { useAudioProcessor } from './hooks/useAudioProcessor';
import { VUMeter } from './components/VUMeter';
import { DigitalMeter } from './components/DigitalMeter';
import { HistoryGraph } from './components/HistoryGraph';
import { 
  Play, 
  Square, 
  Settings2, 
  RotateCcw,
  Activity, 
  Zap,
  Mic,
  Volume2,
  AlertTriangle
} from 'lucide-react';
import { cn } from './lib/utils';
import { motion, AnimatePresence } from 'motion/react';

export default function App() {
  const { 
    isActive, 
    start, 
    stop, 
    metrics, 
    gain, 
    setGain, 
    vuCalibration, 
    setVuCalibration,
    resetMetrics
  } = useAudioProcessor();

  const [showConfig, setShowConfig] = useState(false);

  // Auto-start engine on mount
  React.useEffect(() => {
    start();
    return () => stop();
  }, [start, stop]);

  return (
    <div className="min-h-screen bg-[#0a0a0c] text-[#e0e0e0] font-sans p-4 md:p-8 flex items-center justify-center selection:bg-cyan-500/30 overflow-hidden">
      
      {/* Hardware Interface Main Container */}
      <div className="w-full max-w-6xl bg-[#121214] border border-[#2a2a2e] rounded-xl shadow-[0_30px_60px_-12px_rgba(0,0,0,0.8)] flex flex-col relative overflow-hidden">
        
        {/* Top Hardware Header */}
        <header className="h-12 border-b border-[#222] bg-[#121214] flex items-center justify-between px-6">
          <div className="flex items-center gap-4">
            <div className={cn(
              "w-2.5 h-2.5 rounded-full transition-all duration-500",
              isActive ? "bg-[#00ff66] shadow-[0_0_10px_#00ff66]" : "bg-[#333]"
            )} />
            <span className="text-[10px] font-bold tracking-[0.25em] text-[#888] uppercase">
              SONIC-METER PRO <span className="text-[#444] mx-1">/</span> VST3 METERING SUITE
            </span>
          </div>
          
          <div className="flex items-center gap-2">
            <button 
              onClick={resetMetrics}
              className="px-3 py-1 bg-[#1c1c1f] border border-[#2a2a2e] text-[9px] uppercase tracking-widest text-zinc-500 hover:text-cyan-400 hover:border-cyan-900/50 transition-all flex items-center gap-2"
            >
              <RotateCcw size={10} />
              Reset Stats
            </button>
            <button 
              onClick={() => setShowConfig(!showConfig)}
              className={cn(
                "p-1.5 rounded transition-colors hover:bg-white/5",
                showConfig ? "text-cyan-400" : "text-[#444]"
              )}
            >
              <Settings2 size={14} />
            </button>
          </div>
        </header>

        {/* Hidden Config Panel */}
        <AnimatePresence>
          {showConfig && (
            <motion.div 
              initial={{ height: 0, opacity: 0 }}
              animate={{ height: "auto", opacity: 1 }}
              exit={{ height: 0, opacity: 0 }}
              className="bg-[#1a1a1d] border-b border-[#2a2a2e] overflow-hidden"
            >
              <div className="p-6 grid grid-cols-1 md:grid-cols-2 gap-8 max-w-3xl mx-auto">
                <div className="space-y-4">
                  <div className="flex justify-between items-center mb-1">
                    <label className="text-[10px] uppercase tracking-widest text-[#666] font-bold">VU Calibration</label>
                    <span className="font-mono text-xs text-cyan-400">{vuCalibration} dBFS = 0 VU</span>
                  </div>
                  <input 
                    type="range" min="-24" max="-12" step="1" 
                    value={vuCalibration} 
                    onChange={(e) => setVuCalibration(Number(e.target.value))}
                    className="w-full h-1 bg-zinc-800 rounded-lg appearance-none cursor-pointer accent-cyan-500"
                  />
                  <p className="text-[9px] text-zinc-600 leading-relaxed italic">
                    Adjust the reference level where the analog VU needle hits zero. Standard is -18 dBFS.
                  </p>
                </div>
                <div className="bg-[#0e0e10] p-4 border border-[#222] rounded space-y-3">
                  <h4 className="text-[9px] uppercase text-zinc-500 font-bold border-b border-zinc-800 pb-2">Device Info</h4>
                  <div className="grid grid-cols-2 gap-2 text-[10px] font-mono">
                    <span className="text-zinc-600">Sample Rate:</span><span className="text-zinc-400 text-right">48.0 kHz</span>
                    <span className="text-zinc-600">Buffer Size:</span><span className="text-zinc-400 text-right">2048 Smp</span>
                    <span className="text-zinc-600">Engine:</span><span className="text-emerald-500 text-right uppercase">EBU R128</span>
                  </div>
                </div>
              </div>
            </motion.div>
          )}
        </AnimatePresence>

        {/* Main Interface Content */}
        <main className="flex-1 p-6 md:p-8 flex flex-col md:grid md:grid-cols-12 gap-8 items-stretch">
          
          {/* COLUMN 1: ANALOG & GAIN (4 spans) */}
          <section className="col-span-12 lg:col-span-4 flex flex-col gap-6">
            <div className="bg-[#151518] border border-[#2a2a2e] rounded-xl p-6 shadow-inner flex flex-col items-center justify-between min-h-[340px]">
              <div className="w-full flex justify-between items-center mb-6">
                <h2 className="text-[10px] uppercase tracking-[0.2em] text-[#666] font-bold">Analog Heritage</h2>
                <div className="px-2 py-0.5 bg-[#0e0e10] border border-[#222] rounded text-[9px] font-mono text-cyan-400/70 uppercase">Calibrated</div>
              </div>

              <VUMeter value={metrics.vu} label="Analog Standard" className="transform scale-110 mb-4" />

              {/* Gain Staging Knob Area */}
              <div className="w-full mt-6 flex flex-col items-center gap-4">
                 <div className="flex flex-col items-center gap-1">
                    <div className="relative w-16 h-16 rounded-full bg-gradient-to-tr from-[#000] to-[#222] border-2 border-black flex items-center justify-center shadow-[0_5px_15px_rgba(0,0,0,0.5)] cursor-pointer group">
                      <motion.div 
                        className="absolute top-1 w-1 h-3 bg-cyan-400 rounded-full origin-bottom"
                        style={{ transform: `rotate(${gain * 6}deg)`, x: "-50%" }}
                      />
                      <div className="w-12 h-12 rounded-full border border-[#333] flex items-center justify-center">
                        <Volume2 size={16} className="text-[#333] group-hover:text-cyan-400/50 transition-colors" />
                      </div>
                    </div>
                    <span className={cn(
                      "text-xl font-mono mt-2 tracking-tighter transition-all duration-300",
                      gain === 0 ? "text-zinc-600" : "text-cyan-400 shadow-[0_0_10px_rgba(34,211,238,0.2)]"
                    )}>
                      {gain > 0 ? "+" : ""}{gain.toFixed(2)} <span className="text-[10px] text-zinc-600">dB</span>
                    </span>
                    <span className="text-[9px] uppercase tracking-widest text-zinc-700 font-bold">Input Gain</span>
                 </div>
                 
                 <input 
                  type="range" min="-24" max="24" step="0.01" 
                  value={gain} 
                  onChange={(e) => setGain(Number(e.target.value))}
                  className="w-4/5 h-1 bg-zinc-900 rounded-lg appearance-none cursor-pointer accent-cyan-600"
                />
              </div>
            </div>

            {/* Audio Engine Status */}
            <div className="flex flex-col gap-3">
              <div className={cn(
                "flex items-center justify-center gap-3 w-full py-4 border rounded font-bold uppercase tracking-[0.3em] text-[10px] transition-all",
                isActive ? "bg-emerald-900/10 border-emerald-500/30 text-emerald-500" : "bg-zinc-900 border-zinc-800 text-zinc-600"
              )}>
                <Activity size={14} className={isActive ? "animate-pulse" : ""} />
                {isActive ? "Engine Active" : "Engine Suspended"}
              </div>
            </div>
          </section>

          {/* COLUMN 2: LOUDNESS CENTER (5 spans) */}
          <section className="col-span-12 lg:col-span-5 bg-[#151518] border border-[#2a2a2e] rounded-xl p-8 flex flex-col shadow-inner">
            <div className="flex items-center justify-between mb-8">
              <div className="flex items-center gap-3">
                <Activity size={16} className="text-cyan-500" />
                <h2 className="text-[11px] uppercase tracking-[0.25em] text-zinc-400 font-bold">Loudness Analysis</h2>
              </div>
              <span className="text-[9px] font-mono text-zinc-600 tracking-widest border border-zinc-800 px-2 py-0.5 rounded">EBU R128</span>
            </div>
            
            {/* Real-time History Graph */}
            <div className="mb-8">
               <HistoryGraph data={metrics.history} integrated={metrics.integratedLufs} />
            </div>
            
            <div className="flex-1 grid grid-cols-2 gap-8">
              {/* Primary Integrated Large Display */}
              <div className="col-span-2 bg-[#0e0e10] border border-[#222] p-8 rounded-lg flex flex-col items-center justify-center relative overflow-hidden group">
                <div className="absolute inset-0 bg-gradient-to-br from-cyan-500/5 to-transparent opacity-0 group-hover:opacity-100 transition-opacity" />
                <span className="text-[10px] text-zinc-500 uppercase tracking-[0.4em] font-black mb-2 z-10">Integrated Loudness</span>
                <div className="flex items-baseline gap-2 z-10">
                   <span className="text-6xl font-mono font-bold text-cyan-400 drop-shadow-[0_0_12px_rgba(34,211,238,0.4)]">
                    {metrics.integratedLufs <= -70 ? "-∞" : metrics.integratedLufs.toFixed(1)}
                   </span>
                   <span className="text-sm font-mono text-zinc-600">LUFS</span>
                </div>
                
                <div className="mt-6 flex gap-12 w-full justify-center">
                  <div className="flex flex-col items-center">
                    <span className="text-xs font-mono text-zinc-300">{(metrics.plrIntegrated).toFixed(1)}</span>
                    <span className="text-[8px] text-zinc-600 uppercase font-bold tracking-tighter">PLR Integrated</span>
                  </div>
                  <div className="flex flex-col items-center">
                    <span className="text-xs font-mono text-zinc-300">{metrics.loudnessRange.toFixed(1)}</span>
                    <span className="text-[8px] text-zinc-600 uppercase font-bold tracking-tighter">Loudness Range</span>
                  </div>
                </div>
              </div>

              {/* LUFS Bars & Short Term Metrics */}
              <div className="space-y-4">
                 <DigitalMeter 
                    label="Short Term" 
                    value={metrics.shortTermLufs} 
                    unit="LUFS" 
                    warningThreshold={-12} 
                    criticalThreshold={-8}
                  />
                  <DigitalMeter 
                    label="Momentary" 
                    value={metrics.momentaryLufs} 
                    unit="LUFS" 
                    warningThreshold={-10} 
                    criticalThreshold={-6}
                  />
              </div>

              <div className="space-y-4">
                 <div className="bg-[#0e0e10] hardware-border rounded p-3 flex flex-col gap-1">
                    <span className="text-[8px] uppercase text-[#444] font-bold">PLR Short Term</span>
                    <div className="flex items-baseline justify-between">
                       <span className="text-xl font-mono text-zinc-400">{metrics.plrShortTerm.toFixed(1)}</span>
                       <span className="text-[9px] text-[#222] font-black italic">DYNAMICS</span>
                    </div>
                 </div>
                 <div className="bg-[#0e0e10] p-4 flex-1 flex flex-col justify-center gap-2">
                    <div className="flex justify-between items-center text-[9px] uppercase text-zinc-600 font-bold">
                       <span>Momentary Max</span>
                       <span className="text-cyan-400 font-mono">{metrics.momentaryMax.toFixed(1)}</span>
                    </div>
                    <div className="flex justify-between items-center text-[9px] uppercase text-zinc-600 font-bold">
                       <span>Short Term Max</span>
                       <span className="text-cyan-400 font-mono">{metrics.shortTermMax.toFixed(1)}</span>
                    </div>
                 </div>
              </div>
            </div>
          </section>

          {/* COLUMN 3: PEAK & PERFORMANCE (3 spans) */}
          <section className="col-span-12 lg:col-span-3 flex flex-col gap-6">
            <div className="flex-1 bg-[#151518] border border-[#2a2a2e] rounded-xl p-6 flex flex-col shadow-inner">
               <div className="flex items-center gap-3 mb-8">
                  <Zap size={14} className="text-red-500" />
                  <h2 className="text-[11px] uppercase tracking-[0.2em] text-[#666] font-bold">Transient Check</h2>
               </div>

               <div className="space-y-8 flex-1">
                  <div className="space-y-3">
                    <DigitalMeter 
                      label="True Peak (Current)" 
                      value={metrics.peak} 
                      unit="dBTP" 
                      warningThreshold={-2} 
                      criticalThreshold={-0.1} 
                    />
                    <div className="flex flex-col bg-[#0a0a0c] p-3 border border-[#222] rounded">
                       <span className="text-[9px] uppercase text-zinc-600 font-bold mb-1">True Peak Max</span>
                       <span className={cn(
                         "text-2xl font-mono tracking-tighter leading-none transition-colors",
                         metrics.peakMax >= -0.1 ? "text-red-500 shadow-[0_0_15px_rgba(239,68,68,0.3)]" : "text-zinc-200"
                       )}>
                         {metrics.peakMax <= -99 ? "-∞" : metrics.peakMax.toFixed(2)}
                       </span>
                    </div>
                  </div>

                  <div className="h-px bg-[#222]" />

                  <div className="space-y-4">
                    <DigitalMeter 
                      label="RMS AVG" 
                      value={metrics.rms} 
                      unit="dB" 
                      warningThreshold={-14} 
                      criticalThreshold={-1} 
                    />
                    
                    <div className="p-4 bg-[#0e0e10] rounded border border-[#222] relative overflow-hidden">
                       <div className="absolute right-0 top-0 p-2">
                          <AlertTriangle size={10} className="text-zinc-800" />
                       </div>
                       <span className="text-[9px] uppercase text-[#444] font-black tracking-widest">Mastering Safety</span>
                       <p className="text-[10px] text-zinc-500 mt-2 font-medium">
                        {metrics.peakMax >= -0.1 ? "WARNING: CLIPPING DETECTED" : "SIGNALS WITHIN HEADROOM"}
                       </p>
                    </div>
                  </div>
               </div>
            </div>
          </section>
        </main>

        {/* Status Bar / Footer */}
        <footer className="h-10 bg-[#0a0a0c] border-t border-[#222] flex items-center px-6 justify-between">
          <div className="flex items-center gap-6">
            <div className="flex items-center gap-2">
              <Mic size={10} className="text-[#333]" />
              <span className="text-[9px] font-mono text-[#555] uppercase tracking-widest">
                Source: {isActive ? "Direct Monitoring Active" : "Waiting for Input"}
              </span>
            </div>
          </div>
          <div className="flex items-center gap-4">
             <span className="text-[9px] font-bold text-[#333] uppercase spacing-wide tracking-[0.2em]">Hardware Engine v2.4a</span>
             <div className="w-1.5 h-1.5 rounded-full bg-emerald-500/20 active:bg-emerald-500 shadow-[0_0_5px_rgba(16,185,129,0.3)]" />
          </div>
        </footer>

        {/* Decorative Hardware Bolts */}
        <div className="absolute top-2 left-2 w-1.5 h-1.5 rounded-full bg-[#1c1c1f] shadow-inner border border-black/50" />
        <div className="absolute top-2 right-2 w-1.5 h-1.5 rounded-full bg-[#1c1c1f] shadow-inner border border-black/50" />
        <div className="absolute bottom-2 left-2 w-1.5 h-1.5 rounded-full bg-[#1c1c1f] shadow-inner border border-black/50" />
        <div className="absolute bottom-2 right-2 w-1.5 h-1.5 rounded-full bg-[#1c1c1f] shadow-inner border border-black/50" />
      </div>
    </div>
  );
}
