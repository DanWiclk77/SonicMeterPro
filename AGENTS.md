# SonicMeter Pro - Agent Knowledge Base

## C++ / JUCE Cross-Platform Fixes

- **Ambigüedad de juce::Point / juce::Rectangle:**
  - En arquitecturas ARM64 y con compiladores modernos (Clang/MSVC), llamar a métodos que existen en `juce::Line` pero no en `juce::Point` (como `getPointOnLine`) directamente sobre un `Point` fallará.
  - **Solución:** Usar interpolación manual o crear una `juce::Line<float>`.
  - Ejemplo de interpolación manual para punto en círculo:
    ```cpp
    const float angleRad = angle - juce::MathConstants<float>::halfPi;
    const juce::Point<float> p = pivot + juce::Point<float> (radius * std::cos (angleRad), radius * std::sin (angleRad));
    ```

- **Const correctness con Rectangle:**
  - Métodos como `removeFromLeft`, `removeFromRight`, etc., modifican el objeto original. NO se pueden llamar sobre un `const juce::Rectangle`.
  - **Solución:** Quitar el `const` si se desea modificar in-place, o usar `withTrimmedLeft()` para obtener una copia modificada.

- **Explicit Math Functions:**
  - Usar siempre las versiones float explícitas (`log10f`, `sqrtf`, `fabsf`) para evitar ambigüedades de sobrecarga en Clang (macOS ARM64).

## Design Philosophy - Platinum UI

- **Modular Panels:** Dividir la interfaz en secciones funcionales claras (Análisis Analógico, Espectro, Métricas Digitales).
- **Dark Carbon Theme:** Fondos casi negros con acentos cian y rojos de alta saturación.
- **Reference Grids:** Mostrar siempre líneas de referencia (grillas) en gráficos y medidores para utilidad técnica.
