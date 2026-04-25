# SonicMeter Pro VST3

Professional Loudness and Stereo Field Metering Plugin.
Built with JUCE and C++.

## Versión 1.6.0 "Interface Pro"
- **Rediseño del Medidor VU:** Implementación de escala analógica real, aguja roja con suavizado de movimiento y pivotado corregido.
- **Estabilidad de Loudness:** Nueva lógica de acumulación para `Integrated LUFS` que evita reinicios en el DAW.
- **Fidelidad Visual:** Ajuste de colores y tipografías para coincidir con la versión de previsualización.
- **Optimización DSP:** Acceso directo a punteros de audio para reducir la carga de CPU en el proceso de medición.

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
