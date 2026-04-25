# Persistent Instructions for JUCE Development

- **No JuceHeader.h:** Modern JUCE with CMake does not generate `JuceHeader.h`. Always use direct module includes (e.g., `#include <juce_audio_processors/juce_audio_processors.h>`) in both `.h` and `.cpp` files to ensure stability across all platforms (Windows, Mac, GitHub Actions).
- **Type Safety:** Explicitly define types when using `juce::Point` or `juce::Rectangle` (e.g., `juce::Point<float>`) to avoid MSVC ambiguity errors.
- **DrawText Compatibility:** Use `juce::Rectangle<float>` when calling `g.drawText` to maintain cross-platform compatibility with modern JUCE graphics.
- **Environment Versions:** Always favor **Java 25** for Android/cross-platform tasks and the **latest stable version of Node.js** (Node 24+) for build scripts and actions to avoid deprecation warnings.
