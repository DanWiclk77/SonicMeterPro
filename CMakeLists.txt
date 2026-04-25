cmake_minimum_required(VERSION 3.22)

project(SonicMeterPro VERSION 1.8.0)

# 1. Download JUCE and include it
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
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
    COPY_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Mnfc
    PLUGIN_CODE Snmt
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "SonicMeter Pro")

# 3. Configure Xcode to use dependency analysis for script phases
# This fixes the "Run script build phase will be run during every build" warnings
if(APPLE)
    # Set architectures
    set_target_properties(SonicMeterPro PROPERTIES 
        OSX_ARCHITECTURES "arm64;x86_64"
    )
    
    # Enable dependency analysis for all build phases to avoid unnecessary re-runs
    set_target_properties(SonicMeterPro PROPERTIES
        XCODE_GENERATE_SCHEME ON
    )
    
    # Configure JUCE helper targets if they exist
    foreach(juce_target IN ITEMS ZERO_CHECK ALL_BUILD)
        if(TARGET ${juce_target})
            set_target_properties(${juce_target} PROPERTIES
                XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
            )
        endif()
    endforeach()
    
    # Apply dependency file build phase to plugin targets
    foreach(plugin_target IN ITEMS SonicMeterPro_VST3 SonicMeterPro_AU SonicMeterPro_Standalone)
        if(TARGET ${plugin_target})
            # Use symbolic file input/output for CMake-generated schemes
            set_target_properties(${plugin_target} PROPERTIES
                XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "$(BUILD_DIR)/$(CONFIGURATION)$(EFFECTIVE_PLATFORM_NAME)"
            )
        endif()
    endforeach()
    
    # Global Xcode settings for proper dependency tracking
    set(CMAKE_XCODE_GENERATE_SCHEME ON)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC ON)
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
