#ifndef PLUGINEDITOR_H
#define PLUGINEDITOR_H

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

/**
    SonicMeter Pro Editor - Platinum Edition
    Optimized for ARM64 and High-Resolution rendering.
*/
class SonicMeterAudioProcessorEditor : public juce::AudioProcessorEditor, 
                                       public juce::Timer
{
public:
    SonicMeterAudioProcessorEditor (SonicMeterAudioProcessor&);
    ~SonicMeterAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void mouseDown (const juce::MouseEvent& e) override;

private:
    SonicMeterAudioProcessor& processor;
    
    juce::Slider gainSlider;
    juce::Label gainLabel;
    juce::TextButton resetButton;
    
    // Internal Drawing Primitives
    void drawVUMeter (juce::Graphics& g, const juce::Rectangle<float> area, float value);
    void drawDigitalMeter (juce::Graphics& g, const juce::Rectangle<float> area, const juce::String label, float value, juce::Colour color);
    void drawHistoryGraph (juce::Graphics& g, const juce::Rectangle<float> area, const float* history, int historyIdx);
    void drawCorrelationMeter (juce::Graphics& g, const juce::Rectangle<float> area, float value);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SonicMeterAudioProcessorEditor)
};

#endif
