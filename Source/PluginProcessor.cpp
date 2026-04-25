/* SonicMeter Pro - Advanced Loudness Metering Core */
#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

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
    // High Shelf (Pre-filter)
    auto preCoeffs = juce::IIRCoefficients::makeHighShelf(sampleRate, 1500.0, 0.707, 4.0);
    preFilterL.setCoefficients(preCoeffs);
    preFilterR.setCoefficients(preCoeffs);
    
    // High Pass (K-filter)
    auto weightCoeffs = juce::IIRCoefficients::makeHighPass(sampleRate, 100.0, 1.0);
    weightFilterL.setCoefficients(weightCoeffs);
    weightFilterR.setCoefficients(weightCoeffs);

    resetStats();
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
    juce::AudioBuffer<float> filteredBuffer;
    filteredBuffer.makeCopyOf(buffer);
    
    preFilterL.processSamples(filteredBuffer.getWritePointer(0), buffer.getNumSamples());
    weightFilterL.processSamples(filteredBuffer.getWritePointer(0), buffer.getNumSamples());
    
    if (totalNumInputChannels > 1) {
        preFilterR.processSamples(filteredBuffer.getWritePointer(1), buffer.getNumSamples());
        weightFilterR.processSamples(filteredBuffer.getWritePointer(1), buffer.getNumSamples());
    }

    updateLoudness(filteredBuffer);

    // Peak calculation on raw (with gain) buffer
    float maxPeak = 0.0f;
    float dotProduct = 0.0f;
    float magL = 0.0f;
    float magR = 0.0f;

    auto* channelL = buffer.getReadPointer(0);
    auto* channelR = totalNumInputChannels > 1 ? buffer.getReadPointer(1) : channelL;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        const float l = channelL[sample];
        const float r = channelR[sample];
        
        const float absL = std::abs(l);
        const float absR = std::abs(r);
        const float currentMax = std::max(absL, absR);
        if (currentMax > maxPeak) maxPeak = currentMax;

        dotProduct += l * r;
        magL += l * l;
        magR += r * r;
    }
    
    const float correlationDenom = std::sqrt(magL * magR) + 1.0e-10f;
    currentMeters.correlation = dotProduct / correlationDenom;
    currentMeters.stereoWidth = 1.0f - std::abs(currentMeters.correlation + 1.0f) * 0.5f;

    float peakDb = linearToDb(maxPeak);
    currentMeters.peak = peakDb;
    if (peakDb > currentMeters.peakMax) currentMeters.peakMax = peakDb;
}

void SonicMeterAudioProcessor::updateLoudness(const juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    float sumSq = 0.0f;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        auto* channelData = buffer.getReadPointer(ch);
        for (int s = 0; s < numSamples; ++s) {
            float val = channelData[s];
            sumSq += val * val;
        }
    }
    
    float meanSq = sumSq / (float)(numSamples * std::max(1, buffer.getNumChannels()));
    float mntLufs = 10.0f * std::log10(meanSq + 1e-10f) + 0.69f; // K-weighting offset
    
    currentMeters.momentaryLufs = mntLufs;
    
    // VU calculation (slow decay)
    float targetVu = juce::jlimit(-20.0f, 3.0f, mntLufs + 18.0f);
    if (targetVu > currentMeters.vuValue) 
        currentMeters.vuValue = targetVu;
    else 
        currentMeters.vuValue += (targetVu - currentMeters.vuValue) * 0.05f;

    if (mntLufs > currentMeters.momentaryMax) currentMeters.momentaryMax = mntLufs;
    
    // Simple averaging for Short Term and Integrated for now
    integratedSum += meanSq;
    integratedCount++;
    
    if (integratedCount > 0) {
        float avgMeanSq = integratedSum / (float)integratedCount;
        currentMeters.integratedLufs = 10.0f * std::log10(avgMeanSq + 1e-10f) + 0.69f;
    }
    
    currentMeters.shortTermLufs = mntLufs; // Simulated short term
    
    // Update history for graph
    currentMeters.history[currentMeters.historyIdx] = mntLufs;
    currentMeters.historyIdx = (currentMeters.historyIdx + 1) % 200;
}

void SonicMeterAudioProcessor::resetStats()
{
    currentMeters.peakMax = -100.0f;
    currentMeters.momentaryMax = -100.0f;
    currentMeters.shortTermMax = -100.0f;
    currentMeters.integratedLufs = -100.0f;
    currentMeters.vuValue = -20.0f;
    integratedSum = 0.0;
    integratedCount = 0;
    for (int i = 0; i < 200; ++i) currentMeters.history[i] = -70.0f;
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
