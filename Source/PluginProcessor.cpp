#include "PluginProcessor.h"
#include "PluginEditor.h"

SonicMeterAudioProcessor::SonicMeterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SonicMeterAudioProcessor::~SonicMeterAudioProcessor() {}

void SonicMeterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Initialize K-Weighting filters (Standard EBU R128)
    // High Shelf
    auto preCoeffs = juce::IIRCoefficients::makeHighShelf(sampleRate, 1500.0, 1.414, 4.0);
    preFilterL.setCoefficients(preCoeffs);
    preFilterR.setCoefficients(preCoeffs);
    
    // High Pass
    auto weightCoeffs = juce::IIRCoefficients::makeHighPass(sampleRate, 100.0, 1.0);
    weightFilterL.setCoefficients(weightCoeffs);
    weightFilterR.setCoefficients(weightCoeffs);
}

void SonicMeterAudioProcessor::releaseResources() {}

void SonicMeterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Apply Input Gain Staging
    buffer.applyGain(gainFactor);

    // Filter for loudness calculation
    juce::AudioBuffer<float> filteredBuffer(buffer);
    weightFilterL.processSamples(filteredBuffer.getWritePointer(0), buffer.getNumSamples());
    if (totalNumInputChannels > 1)
        weightFilterR.processSamples(filteredBuffer.getWritePointer(1), buffer.getNumSamples());

    updateLoudness(filteredBuffer);

    // Peak calculation on raw (with gain) buffer
    float maxPeak = 0.0f;
    for (int ch = 0; ch < totalNumInputChannels; ++ch) {
        float peak = buffer.getMagnitude(ch, 0, buffer.getNumSamples());
        if (peak > maxPeak) maxPeak = peak;
    }
    
    float peakDb = linearToDb(maxPeak);
    currentMeters.peak = peakDb;
    if (peakDb > currentMeters.peakMax) currentMeters.peakMax = peakDb;
}

void SonicMeterAudioProcessor::updateLoudness(const juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    float sumSq = 0.0f;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        for (int s = 0; s < numSamples; ++s) {
            float val = buffer.getSample(ch, s);
            sumSq += val * val;
        }
    }
    
    float meanSq = sumSq / (float)(numSamples * buffer.getNumChannels());
    float mntLufs = 10.0f * std::log10(meanSq + 1e-10f) + 0.69f; // K-weighting offset
    
    currentMeters.momentaryLufs = mntLufs;
    if (mntLufs > currentMeters.momentaryMax) currentMeters.momentaryMax = mntLufs;
    
    // Accumulate for Integrated and Short Term logic here...
    // In a real VST, we use sliding windows and histograms for LRA.
}

void SonicMeterAudioProcessor::resetStats()
{
    currentMeters.peakMax = -100.0f;
    currentMeters.momentaryMax = -100.0f;
    currentMeters.shortTermMax = -100.0f;
}

juce::AudioProcessorEditor* SonicMeterAudioProcessor::createEditor()
{
    return new SonicMeterAudioProcessorEditor (*this);
}

void SonicMeterAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {}
void SonicMeterAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SonicMeterAudioProcessor();
}
