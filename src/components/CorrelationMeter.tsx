import React from 'react';
import { cn } from '@/src/lib/utils';

interface CorrelationMeterProps {
  value: number; // -1 to 1
  className?: string;
}

export const CorrelationMeter: React.FC<CorrelationMeterProps> = ({ value, className }) => {
  const percentage = ((value + 1) / 2) * 100;

  return (
    <div className={cn("flex flex-col gap-1 w-full bg-[#0e0e10] p-3 rounded border border-[#222] shadow-inner", className)}>
      <div className="flex justify-between items-center text-[8px] font-mono text-[#666] tracking-widest uppercase mb-1">
        <span>Correlation</span>
        <span className={cn(
          "font-bold transition-colors",
          value < 0 ? "text-red-500" : "text-cyan-400"
        )}>{value.toFixed(2)}</span>
      </div>
      
      <div className="h-2 bg-[#050506] rounded-full overflow-hidden relative flex items-center px-0.5">
        <div className="absolute left-1/2 w-px h-full bg-zinc-800 z-10" />
        <div 
          className={cn(
            "h-full rounded-full transition-all duration-300",
            value < 0 ? "bg-red-500 shadow-[0_0_5px_rgba(239,68,68,0.5)]" : "bg-cyan-500 shadow-[0_0_5px_rgba(34,211,238,0.5)]"
          )}
          style={{ 
            width: `${Math.abs(value * 50)}%`, 
            marginLeft: value < 0 ? `${50 - Math.abs(value * 50)}%` : '50%' 
          }}
        />
      </div>
      
      <div className="flex justify-between text-[7px] text-zinc-700 font-bold px-1 mt-1">
        <span>-1</span>
        <span>0</span>
        <span>+1</span>
      </div>
    </div>
  );
};
