import React from 'react';
import { cn } from '@/src/lib/utils';

interface DigitalMeterProps {
  label: string;
  value: number;
  unit?: string;
  warningThreshold?: number;
  criticalThreshold?: number;
}

export const DigitalMeter: React.FC<DigitalMeterProps> = ({ 
  label, 
  value, 
  unit = "dB", 
  warningThreshold = -6, 
  criticalThreshold = -0.1 
}) => {
  const isCritical = value >= criticalThreshold;
  const isWarning = value >= warningThreshold && !isCritical;
  
  const formattedValue = value <= -70 ? "-∞" : value.toFixed(1);

  return (
    <div className="flex flex-col gap-1 w-full bg-[#0e0e10] p-4 rounded border border-[#222] shadow-[inset_0_2px_4px_rgba(0,0,0,0.4)] group overflow-hidden relative">
      {/* Background Subtle Gradient */}
      <div className="absolute inset-0 bg-gradient-to-br from-white/5 to-transparent opacity-5" />
      
      <div className="flex justify-between items-center text-[8px] font-mono uppercase tracking-[0.2em] text-[#666] font-black z-10">
        <span>{label}</span>
        <span className={cn(
          "transition-all duration-300",
          isCritical ? "text-[#ff3333] shadow-[0_0_10px_rgba(255,51,51,0.5)]" : "text-transparent"
        )}>
          {isCritical ? "OVR" : ""}
        </span>
      </div>
      
      <div className="flex items-baseline justify-between mt-1 z-10">
        <span className={cn(
          "text-3xl font-mono tracking-tighter leading-none transition-all duration-300",
          isCritical 
            ? "text-[#ff3333] drop-shadow-[0_0_8px_rgba(255,51,51,0.6)]" 
            : isWarning 
              ? "text-yellow-400 drop-shadow-[0_0_8px_rgba(250,204,21,0.4)]" 
              : "text-cyan-400 drop-shadow-[0_0_8px_rgba(34,211,238,0.4)]"
        )}>
          {formattedValue}
        </span>
        <span className="text-[10px] font-mono text-[#444] mb-0.5 uppercase">{unit}</span>
      </div>

      {/* Mini Segmented Bar */}
      <div className="h-1 bg-[#050506] rounded-full overflow-hidden mt-2 flex gap-0.5">
        {[...Array(20)].map((_, i) => {
          const threshold = -60 + (i * 3);
          const isActive = value >= threshold;
          return (
            <div 
              key={i}
              className={cn(
                "h-full flex-1 transition-all duration-200",
                isActive 
                  ? (threshold > criticalThreshold ? "bg-[#ff3333]" : threshold > warningThreshold ? "bg-yellow-400" : "bg-cyan-500") 
                  : "bg-zinc-900"
              )}
            />
          );
        })}
      </div>
    </div>
  );
};
