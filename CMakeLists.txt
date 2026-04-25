cmake_minimum_required(VERSION 3.15)

project(SonicMeterPro VERSION 1.7.1)

# 1. Download JUCE and include it
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        origin/master # or a specific version like 7.0.5
)
FetchContent_MakeAvailable(juce)

# 2. Add the plugin
juce_add_plugin(SonicMeterPro
    COMPANY_NAME "SonicTools"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Mnfc
    PLUGIN_CODE Snmt
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "SonicMeter Pro")

# 3. Add Source files
target_sources(SonicMeterPro
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h)

# 4. Links and includes
target_compile_definitions(SonicMeterPro
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(SonicMeterPro
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_audio_formats
        juce::juce_audio_devices
        juce::juce_audio_basics
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_core)
