#ifndef PLUGINPROCESSOR_H
#define PLUGINPROCESSOR_H

#include <JuceHeader.h>
#include <vector>

class SonicMeterAudioProcessor : public juce::AudioProcessor
{
public:
    SonicMeterAudioProcessor();
    ~SonicMeterAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "SonicMeter Pro"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Metering API for the Editor
    struct Meters
    {
        float peak = -100.0f;
        float peakMax = -100.0f;
        float momentaryLufs = -100.0f;
        float momentaryMax = -100.0f;
        float shortTermLufs = -100.0f;
        float shortTermMax = -100.0f;
        float integratedLufs = -100.0f;
        float loudnessRange = 0.0f;
        float vuValue = -20.0f;
    };

    Meters getMeters() const { return currentMeters; }
    void resetStats();

private:
    Meters currentMeters;
    
    // EBU R128 Filters (K-Weighting)
    juce::IIRFilter preFilterL, preFilterR;
    juce::IIRFilter weightFilterL, weightFilterR;
    
    // Buffers for ST and Integrated
    std::vector<float> momentaryHistory;
    std::vector<float> shortTermHistory;
    std::vector<double> accumulationBuffer;
    
    float gainFactor = 1.0f;
    float vuCalibration = -18.0f;

    void updateLoudness (const juce::AudioBuffer<float>& buffer);
    float linearToDb(float linear) { return linear > 0.00001f ? 20.0f * std::log10(linear) : -100.0f; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SonicMeterAudioProcessor)
};

#endif
