import React from 'react';
import { cn } from '@/src/lib/utils';

interface MonoCompatibilityProps {
  width: number; // 0 to 1
  className?: string;
}

export const MonoCompatibility: React.FC<MonoCompatibilityProps> = ({ width, className }) => {
  // width 0 = mono, width 1 = wide
  
  return (
    <div className={cn("flex flex-col gap-1 w-full bg-[#0e0e10] p-3 rounded border border-[#222] shadow-inner", className)}>
      <div className="flex justify-between items-center text-[8px] font-mono text-[#666] tracking-widest uppercase mb-1">
        <span>Stereo Width</span>
        <span className="text-zinc-400">{(width * 100).toFixed(0)}%</span>
      </div>

      <div className="flex items-center gap-2">
        <span className="text-[7px] text-zinc-700 font-black">M</span>
        <div className="flex-1 h-1 bg-[#050506] rounded-full overflow-hidden">
          <div 
            className="h-full bg-gradient-to-r from-blue-500 to-cyan-400 transition-all duration-300"
            style={{ width: `${width * 100}%` }}
          />
        </div>
        <span className="text-[7px] text-zinc-700 font-black">S</span>
      </div>
    </div>
  );
};
