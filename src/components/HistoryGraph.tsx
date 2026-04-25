import React from 'react';
import { cn } from '@/src/lib/utils';

interface HistoryGraphProps {
  data: number[];
  integrated: number;
  className?: string;
}

export const HistoryGraph: React.FC<HistoryGraphProps> = ({ data, integrated, className }) => {
  const maxLufs = 0;
  const minLufs = -48;

  const getPointY = (val: number, height: number) => {
    const clamped = Math.max(minLufs, Math.min(maxLufs, val));
    const normalized = (clamped - minLufs) / (maxLufs - minLufs);
    return height - (normalized * height);
  };

  const getIntegratedY = (height: number) => {
    return getPointY(integrated, height);
  };

  return (
    <div className={cn("w-full h-48 bg-[#0a0a0c] border border-[#222] rounded-lg overflow-hidden relative group", className)}>
      {/* Grid Lines */}
      <div className="absolute inset-0 flex flex-col justify-between py-2 opacity-10 pointer-events-none">
        {[-3, -6, -9, -14, -23, -36].map((l) => (
          <div key={l} className="w-full border-t border-zinc-500 relative">
            <span className="absolute -top-1 right-1 text-[7px] text-zinc-400">{l}</span>
          </div>
        ))}
      </div>

      <svg className="w-full h-full overflow-visible" preserveAspectRatio="none">
        {/* Short Term Area Path */}
        <path
          d={`M ${data.map((v, i) => `${(i / (data.length - 1)) * 100}% ${getPointY(v, 180)}`).join(' L ')} L 100% 180 L 0 180 Z`}
          fill="url(#loudnessGradient)"
          className="transition-all duration-100"
        />

        {/* Short Term Line */}
        <polyline
          points={data.map((v, i) => `${(i / (data.length - 1)) * 100},${getPointY(v, 180)}`).join(' ')}
          fill="none"
          stroke="#22d3ee"
          strokeWidth="1.5"
          className="transition-all duration-100"
        />

        {/* Integrated Target Line */}
        {integrated > -70 && (
          <line
            x1="0"
            y1={getIntegratedY(180)}
            x2="100%"
            y2={getIntegratedY(180)}
            stroke="#00ff66"
            strokeWidth="1"
            strokeDasharray="4,2"
            opacity="0.6"
          />
        )}

        <defs>
          <linearGradient id="loudnessGradient" x1="0" y1="0" x2="0" y2="1">
            <stop offset="0%" stopColor="#22d3ee" stopOpacity="0.4" />
            <stop offset="100%" stopColor="#22d3ee" stopOpacity="0.05" />
          </linearGradient>
        </defs>
      </svg>

      {/* Threshold Labels */}
      <div className="absolute top-2 left-2 text-[8px] font-mono text-zinc-600 uppercase tracking-widest bg-black/40 px-1 rounded">
        Short-Term History
      </div>
    </div>
  );
};
