cmake_minimum_required(VERSION 3.15)

project(SonicMeterPro VERSION 1.8.0)

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

# 3. Fix Xcode build phases to avoid unnecessary re-runs
# This resolves the "Run script build phase will be run during every build" warnings
if(APPLE)
    # Configure CMake to generate proper Xcode schemes with dependency analysis
    set_property(GLOBAL PROPERTY XCODE_EMIT_EFFECTIVE_PLATFORM_NAME ON)
    
    # Set dependency file build phase for all targets
    foreach(target SonicMeterPro_VST3 SonicMeterPro_AU SonicMeterPro_Standalone)
        if(TARGET ${target})
            set_target_properties(${target} PROPERTIES
                XCODE_DEPENDENCY_FILE_BUILD_PHASE ON
            )
        endif()
    endforeach()
endif()

# 4. Add Source files
target_sources(SonicMeterPro
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h)

# 5. Links and includes
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
