#ifndef PLUGINPROCESSOR_H
#define PLUGINPROCESSOR_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_core/juce_core.h>
#include <vector>
#include <cmath>
#include <algorithm>

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
        float rms = -100.0f;
        float rmsMax = -100.0f;
        float momentaryLufs = -100.0f;
        float momentaryMax = -100.0f;
        float shortTermLufs = -100.0f;
        float shortTermMax = -100.0f;
        float integratedLufs = -100.0f;
        float loudnessRange = 0.0f;
        float plr = 0.0f;
        float vuValue = -20.0f;
        
        float spectrum[128];
        
        // Smoothed values for visual legibility (Slow Ballistics)
        float peakDisplay = -100.0f;
        float rmsDisplay = -100.0f;
        float momentaryDisplay = -100.0f;
        float truePeakDisplay = -100.0f;
        
        float history[200];
        int historyIdx = 0;
        float correlation = 0.0f;
        float stereoWidth = 0.0f;

        Meters()
        {
            for (int i = 0; i < 200; ++i) history[i] = -70.0f;
            for (int i = 0; i < 128; ++i) spectrum[i] = 0.0f;
        }
    };

    enum StreamingPreset {
        None,
        Spotify,
        YouTube,
        AppleMusic,
        Beatport,
        Club
    };

    Meters getMeters() const { return currentMeters; }
    void resetStats();

    float getGainDb() const { return gainDb; }
    void setGainDb(float newGainDb) { 
        gainDb = juce::jlimit(-25.0f, 25.0f, newGainDb); 
        gainFactor = std::pow(10.0f, gainDb / 20.0f);
    }
    
    float getCalibration() const { return vuCalibration; }
    void setCalibration(float newCal) { vuCalibration = newCal; }
    
    StreamingPreset getPreset() const { return currentPreset; }
    void setPreset(StreamingPreset p) { currentPreset = p; }

private:
    Meters currentMeters;
    
    // EBU R128 Filters (K-Weighting)
    juce::IIRFilter preFilterL, preFilterR;
    juce::IIRFilter weightFilterL, weightFilterR;
    
    // Buffers for ST and Integrated
    std::vector<float> momentaryHistory;
    std::vector<float> shortTermHistory;
    std::vector<double> accumulationBuffer;
    
    juce::dsp::WindowingFunction<float> window { 2048, juce::dsp::WindowingFunction<float>::hann };
    juce::dsp::FFT forwardFFT { 11 }; // 2048 points
    float fifo[2048];
    float fftData[4096];
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;

    float smoothingAlpha = 0.008f; 
    
    // LRA Statistics
    std::vector<float> lraHistory;
    const int maxLraPoints = 2000; // ~20 seconds of data for stats
    long long integratedCount = 0;

    void updateLoudness (const juce::AudioBuffer<float>& buffer);
    void updateLRA (const juce::AudioBuffer<float>& buffer);
    float calculateCorrelation (const juce::AudioBuffer<float>& buffer);
    float linearToDb(float linear) { return linear > 0.00001f ? 20.0f * std::log10f(linear) : -100.0f; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SonicMeterAudioProcessor)
};

#endif
