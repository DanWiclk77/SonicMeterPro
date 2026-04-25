# SonicMeter Pro VST3

Professional Loudness and Stereo Field Metering Plugin.
Built with JUCE and C++.

## Versión 1.6.1 "Interface Pro Fix"
- **Corrección de Compilación:** Corregido error en `drawText` que impedía la generación del binario en Windows/Mac.
- **Precisión DSP:** Implementación completa de la cadena de filtrado K-Weighting (Pre-filter + K-filter) según EBU R128.
- **Funcionalidad de Reset:** Ahora puedes hacer clic en cualquier parte de la interfaz para reiniciar las estadísticas de medición.
- **Estabilidad de Integrated LUFS:** Corregida la inicialización de buffers de acumulación para evitar valores erróneos al inicio.

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
