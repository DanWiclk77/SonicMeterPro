# SonicMeter Pro - Platinum Edition
Professional high-performance Loudness and Stereo Field Metering.

## Version 1.8.0 "Platinum Optimization"
- **ARM64 Silicon Mastery:** Full type-safety audit for Apple Silicon (M1/M2/M3) and modern x64 processors.
- **Precision UI:** Technical technical UI rewrite using high-resolution geometry and carbon-fiber aesthetic.
- **Advanced DSP:** True 3-second Short Term LUFS rolling average and gated integrated metering approximation.
- **Zero Ambiguity:** Removed global namespaces and enforced explicit math functions (`log10f`, `sqrtf`, `fabsf`) to resolve all Clang/MSVC compiler conflicts.
- **Persistent State:** Added binary state serialization for gain staging parameters.

## Versión 1.7.1 "Silicon Resilience"
- **Correcciones de Ambigüedad (Clang/ARM):** Implementación de funciones matemáticas explícitas (`log10f`, `sqrtf`) para asegurar compilación exitosa en Apple Silicon.
- **Robustez de CI/CD:** Corregidas las rutas de artefactos con espacios y asegurado el entorno a Node 24 sin advertencias.
- **Sincronización Web-Plugin:** Alineada la lógica de medición del preview de React con el motor C++ para una experiencia idéntica.

## Versión 1.7.0 "Vanguard Professional"
- **Rediseño de Interfaz:** Nuevo layout bento-grid con panel de control de ganancia integrado y visualización técnica mejorada.
- **Gain Staging Pro:** Añadido control de ganancia de entrada para calibración precisa antes de la medición.
- **Robustez Multi-Plataforma:** Implementación definitiva de tipos explícitos para eliminar ambigüedades en compiladores MSVC y Clang.
- **Optimización de Renderizado:** Uso extensivo de `AffineTransform` y primitivas de dibujo de punto flotante para mayor nitidez y compatibilidad.
- **Entorno de Compilación:** Configuración estable con Java 25 y Node 24 para CI/CD sin avisos de deprecación.

## Versión 1.6.2 "Precision Core"
- **Correcciones Críticas (MSVC):** Eliminación de ambigüedades en tipos `auto` y `juce::Point` para asegurar la compilación en Windows.
- **Entorno de Compilación:** Actualizado a **Java 25** y **Node.js 24** en GitHub Actions para asegurar compatibilidad y evitar avisos de deprecación.
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
