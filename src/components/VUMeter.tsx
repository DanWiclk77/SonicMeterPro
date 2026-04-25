import React from 'react';
import { motion } from 'motion/react';
import { cn } from '@/src/lib/utils';

interface VUMeterProps {
  value: number; // -20 to +3
  label: string;
  className?: string;
}

export const VUMeter: React.FC<VUMeterProps> = ({ value, label, className }) => {
  const getAngle = (val: number) => {
    const minVal = -20;
    const maxVal = 3;
    const minAngle = -45;
    const maxAngle = 45;
    
    const normalized = (val - minVal) / (maxVal - minVal);
    return minAngle + normalized * (maxAngle - minAngle);
  };

  const angle = getAngle(value);

  return (
    <div className={cn("relative w-64 h-40 bg-gradient-to-b from-[#e6e2d3] to-[#dcd7c5] border-2 border-[#111] rounded-t-[100px] overflow-hidden shadow-[inset_0_2px_10px_rgba(0,0,0,0.2),0_10px_30px_rgba(0,0,0,0.5)] flex flex-col items-center justify-end pb-4", className)}>
      {/* Internal Shadow for depth */}
      <div className="absolute inset-0 border-[6px] border-[#1a1a1a]/10 pointer-events-none rounded-t-[100px]" />
      
      {/* Scale SVG */}
      <svg className="absolute inset-0 w-full h-full" viewBox="0 0 200 120">
        <path
          d="M 25,105 A 75,75 0 0,1 175,105"
          fill="none"
          stroke="#333"
          strokeWidth="1.5"
          strokeDasharray="2,1.5"
          opacity="0.3"
        />
        {/* Major Ticks */}
        {[-20, -10, -7, -5, -3, -2, -1, 0, 1, 2, 3].map((v) => {
          const a = (getAngle(v) - 90) * (Math.PI / 180);
          const r1 = v % 5 === 0 || v > -2 ? 78 : 82;
          const x1 = 100 + Math.cos(a) * r1;
          const y1 = 105 + Math.sin(a) * r1;
          const x2 = 100 + Math.cos(a) * 92;
          const y2 = 105 + Math.sin(a) * 92;
          const isRed = v > 0;
          return (
            <g key={v}>
              <line x1={x1} y1={y1} x2={x2} y2={y2} stroke={isRed ? "#cc0000" : "#222"} strokeWidth={v % 5 === 0 || v > -2 ? "1.5" : "1"} />
              {(v % 5 === 0 || v >= 0) && (
                <text
                  x={100 + Math.cos(a) * 65}
                  y={105 + Math.sin(a) * 65}
                  fill={isRed ? "#cc0000" : "#444"}
                  fontSize="7"
                  fontWeight="bold"
                  textAnchor="middle"
                  dominantBaseline="middle"
                  fontFamily="monospace"
                >
                  {v}
                </text>
              )}
            </g>
          );
        })}
        <text x="100" y="80" fill="#222" fontSize="12" textAnchor="middle" fontWeight="black" className="uppercase tracking-[0.2em] opacity-80">
          VU
        </text>
      </svg>

      {/* Needle Container - Pivot at 100, 105 */}
      <div className="absolute bottom-[10px] left-1/2 -translate-x-1/2 w-full h-full pointer-events-none z-10">
        <motion.div
           className="absolute left-1/2 bottom-0 w-0.5 h-[85px] bg-[#1a1a1a] origin-bottom"
           animate={{ rotate: angle }}
           transition={{ type: "spring", stiffness: 45, damping: 25 }}
           style={{ x: "-50%" }}
        >
          {/* Tip of needle is red if over 0 */}
          <div className={cn(
             "absolute top-0 w-full h-8 transition-colors duration-300",
             value > 0 ? "bg-red-600 shadow-[0_0_5px_rgba(220,38,38,0.5)]" : "bg-[#1a1a1a]"
          )} />
        </motion.div>
      </div>

      {/* Pivot Nut */}
      <div className="absolute bottom-[-10px] left-1/2 -translate-x-1/2 w-10 h-10 rounded-full bg-gradient-to-br from-[#333] to-[#000] border-2 border-[#111] z-20 shadow-lg flex items-center justify-center">
        <div className="w-2 h-2 rounded-full bg-[#222] border border-[#444]" />
      </div>
      
      <div className="z-20 mb-1 text-[8px] text-[#555] font-mono tracking-tighter uppercase font-bold">
        {label}
      </div>
    </div>
  );
};
