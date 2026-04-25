# SonicMeter Pro VST3

Professional Loudness and Stereo Field Metering Plugin.
Built with JUCE and C++.

## Versión 1.5.1 "Vanguard Fix"
- **Correcciones Gráficas:** Corregido error de rotación de aguja y compatibilidad con `juce::Graphics`.
- **Estabilidad de Tipos:** Mejorada la inicialización de estructuras de datos para mayor compatibilidad con MSVC.
- **Inclusiones Modulares:** Refinadas las cabeceras para evitar dependencias circulares o inexistentes.

## Características
- **EBU R128 Compliant:** Integrated, Short-Term, and Momentary LUFS.
- **True Peak Metering:** Accurate peak detection.
- **Stereo Analysis:** Correlation meter and width estimation.
- **Analog VU Simulation:** Calibrated needle response.
- **Real-time History:** 2D graph for loudness over time.

## Build Status
[![Build VST3](https://github.com/{{GITHUB_USER}}/{{REPO_NAME}}/actions/workflows/build.yml/badge.svg)](https://github.com/{{GITHUB_USER}}/{{REPO_NAME}}/actions/workflows/build.yml)

## How to use
1. Load the VST3 in your favorite DAW (Ableton, FL Studio, etc.).
2. Use the **Gain Staging** knob to adjust input levels.
3. Monitor your **Integrated LUFS** to hit target levels (e.g., -14 LUFS for Spotify).
