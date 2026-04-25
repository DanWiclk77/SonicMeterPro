import { clsx, type ClassValue } from 'clsx';
import { twMerge } from 'tailwind-merge';

export function cn(...inputs: ClassValue[]) {
  return twMerge(clsx(inputs));
}

export function dbToPercent(db: number, range: number = 60): number {
  if (db <= -range) return 0;
  return (db + range) / range;
}

export function percentToDb(percent: number, range: number = 60): number {
  return percent * range - range;
}

export function linearToDb(linear: number): number {
  if (linear <= 0) return -100;
  return 20 * Math.log10(linear);
}

export function dbToLinear(db: number): number {
  return Math.pow(10, db / 20);
}
