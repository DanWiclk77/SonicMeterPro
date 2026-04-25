# SonicMeter Pro VST3

Professional Loudness and Stereo Field Metering Plugin.
Built with JUCE and C++.

## Versión 1.6.2 "Precision Core"
- **Correcciones Críticas (MSVC):** Eliminación de ambigüedades en tipos `auto` y `juce::Point` para asegurar la compilación en Windows.
- **Renderizado de Fuente:** Asegurada la compatibilidad de `drawText` con rectángulos flotantes.
- **Ajuste de Escala VU:** Calibración refinada del arco de medición y etiquetas.
- **Estabilidad de Compilación:** Refinamiento de la lógica de punteros de audio para evitar errores de conversión en Mac.

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
