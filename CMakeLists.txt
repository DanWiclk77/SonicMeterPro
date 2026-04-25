cmake_minimum_required(VERSION 3.22)

project(SonicMeterPro VERSION 1.8.0)

# Global Xcode settings BEFORE any project configuration
set(CMAKE_XCODE_GENERATE_SCHEME ON)
set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC ON)

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

# 3. Configure Xcode AFTER all targets are created
if(APPLE)
    # Set architectures for main target
    set_target_properties(SonicMeterPro PROPERTIES 
        OSX_ARCHITECTURES "arm64;x86_64"
    )
    
    # Configure CMake utility targets - MUST be done AFTER juce_add_plugin
    # These targets are created by CMake/JUCE internally
    set_property(TARGET ZERO_CHECK APPEND PROPERTY
        XCODE_GENERATE_SCHEME ON
    )
    
    set_property(TARGET ALL_BUILD APPEND PROPERTY
        XCODE_GENERATE_SCHEME ON
    )
    
    # Configure plugin targets with proper input/output dependencies
    foreach(plugin_target IN ITEMS SonicMeterPro_VST3 SonicMeterPro_AU SonicMeterPro_Standalone)
        if(TARGET ${plugin_target})
            # Set properties after target creation
            set_target_properties(${plugin_target} PROPERTIES
                XCODE_GENERATE_SCHEME ON
            )
            
            # Use explicit input/output for build phases
            set_property(TARGET ${plugin_target} APPEND PROPERTY
                XCODE_GENERATE_SCHEME ON
            )
        endif()
    endforeach()
    
    # Set CMake utility directories to track changes
    set(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym")
    set(CMAKE_XCODE_ATTRIBUTE_COPY_PHASE_STRIP NO)
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
