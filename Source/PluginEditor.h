#ifndef PLUGINEDITOR_H
#define PLUGINEDITOR_H

#include "PluginProcessor.h"

class SonicMeterAudioProcessorEditor : public juce::AudioProcessorEditor, 
                                       public juce::Timer
{
public:
    SonicMeterAudioProcessorEditor (SonicMeterAudioProcessor&);
    ~SonicMeterAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    SonicMeterAudioProcessor& processor;
    
    juce::Slider gainSlider;
    juce::Label gainLabel;
    
    void drawVUMeter (juce::Graphics& g, juce::Rectangle<float> area, float value);
    void drawDigitalMeter (juce::Graphics& g, juce::Rectangle<float> area, juce::String label, float value, juce::Colour color);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SonicMeterAudioProcessorEditor)
};

#endif
