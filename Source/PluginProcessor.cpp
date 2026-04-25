/* 
    SonicMeter Pro - Platinum DSP Core
    Precision Loudness Analysis optimized for ARM64 and x64.
*/
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
    // Pre-allocate history buffers for 3-second Short Term analysis (approx 30 updates/sec * 3 = 90 slots)
    shortTermHistory.reserve(100);
}

SonicMeterAudioProcessor::~SonicMeterAudioProcessor() {}

void SonicMeterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Professional K-Weighting Filters (EBU R128)
    const auto preCoeffs = juce::IIRCoefficients::makeHighShelf(sampleRate, 1500.0, 0.707, 4.0);
    preFilterL.setCoefficients(preCoeffs);
    preFilterR.setCoefficients(preCoeffs);
    
    const auto weightCoeffs = juce::IIRCoefficients::makeHighPass(sampleRate, 100.0, 1.0);
    weightFilterL.setCoefficients(weightCoeffs);
    weightFilterR.setCoefficients(weightCoeffs);

    resetStats();
}

void SonicMeterAudioProcessor::releaseResources() {}

void SonicMeterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    const int numInputs = getTotalNumInputChannels();
    const int numOutputs = getTotalNumOutputChannels();

    for (int i = numInputs; i < numOutputs; ++i)
        buffer.clear (i, 0, numSamples);

    // Apply User Input Gain Staging
    buffer.applyGain(gainFactor);

    // Filter Scratch Buffer for R128 weighting
    juce::AudioBuffer<float> filteredBuffer;
    filteredBuffer.makeCopyOf(buffer);
    
    preFilterL.processSamples(filteredBuffer.getWritePointer(0), numSamples);
    weightFilterL.processSamples(filteredBuffer.getWritePointer(0), numSamples);
    
    if (numInputs > 1) {
        preFilterR.processSamples(filteredBuffer.getWritePointer(1), numSamples);
        weightFilterR.processSamples(filteredBuffer.getWritePointer(1), numSamples);
    }

    updateLoudness(filteredBuffer);

    // Peak analysis on the gain-staged buffer
    float maxPeak = 0.0f;
    float dotProduct = 0.0f;
    float magL = 0.0f;
    float magR = 0.0f;

    const float* channelL = buffer.getReadPointer(0);
    const float* channelR = (numInputs > 1) ? buffer.getReadPointer(1) : channelL;

    for (int s = 0; s < numSamples; ++s) {
        const float l = channelL[s];
        const float r = channelR[s];
        
        const float absL = std::fabsf(l);
        const float absR = std::fabsf(r);
        maxPeak = std::max({maxPeak, absL, absR});

        dotProduct += l * r;
        magL += l * l;
        magR += r * r;
    }
    
    // Stereo Field Analysis
    const float correlationDenom = std::sqrtf(magL * magR) + 1.0e-11f;
    currentMeters.correlation = dotProduct / correlationDenom;
    currentMeters.stereoWidth = 1.0f - std::fabsf(currentMeters.correlation + 1.0f) * 0.5f;

    // RMS Calculation
    const float rmsVal = std::sqrtf((magL + magR) / (float)(2 * numSamples));
    const float rmsDb = linearToDb(rmsVal);
    currentMeters.rms = rmsDb;
    if (rmsDb > currentMeters.rmsMax) currentMeters.rmsMax = rmsDb;

    const float peakDb = linearToDb(maxPeak);
    currentMeters.peak = peakDb;
    if (peakDb > currentMeters.peakMax) currentMeters.peakMax = peakDb;

    // Update PLR
    currentMeters.plr = currentMeters.peakMax - currentMeters.integratedLufs;
}

void SonicMeterAudioProcessor::updateLoudness(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    float sumSq = 0.0f;
    
    for (int ch = 0; ch < numChannels; ++ch) {
        const float* data = buffer.getReadPointer(ch);
        for (int s = 0; s < numSamples; ++s) {
            sumSq += data[s] * data[s];
        }
    }
    
    const float meanSq = sumSq / (float)(std::max(1, numSamples * numChannels));
    const float mntLufs = 10.0f * std::log10f(meanSq + 1.0e-11f) + 0.695f; 
    
    currentMeters.momentaryLufs = mntLufs;
    if (mntLufs > currentMeters.momentaryMax) currentMeters.momentaryMax = mntLufs;

    // Ballistics - VU Simulation (Standard 300ms integration)
    const float targetVu = juce::jlimit(-20.0f, 4.0f, mntLufs + 18.0f);
    if (targetVu > currentMeters.vuValue) 
        currentMeters.vuValue = targetVu;
    else 
        currentMeters.vuValue += (targetVu - currentMeters.vuValue) * 0.075f;

    // Integrated Accumulation
    integratedSum += (double)meanSq;
    integratedCount++;
    
    if (integratedCount > 0) {
        const float avgMeanSq = (float)(integratedSum / (double)integratedCount);
        currentMeters.integratedLufs = 10.0f * std::log10f(avgMeanSq + 1.0e-11f) + 0.695f;
    }
    
    // Short Term (3s rolling window approximation)
    shortTermHistory.push_back(meanSq);
    if (shortTermHistory.size() > 90) { // Approx 3s of blocks at typical buffer sizes
        shortTermHistory.erase(shortTermHistory.begin());
    }
    
    float stSumSq = 0.0f;
    for (float v : shortTermHistory) stSumSq += v;
    const float stMeanSq = stSumSq / (float)(std::max((size_t)1, shortTermHistory.size()));
    currentMeters.shortTermLufs = 10.0f * std::log10f(stMeanSq + 1.0e-11f) + 0.695f;
    
    // History Tracking
    currentMeters.history[currentMeters.historyIdx] = mntLufs;
    currentMeters.historyIdx = (currentMeters.historyIdx + 1) % 200;
}

void SonicMeterAudioProcessor::resetStats()
{
    const float infDb = -100.0f;
    currentMeters.peakMax = infDb;
    currentMeters.peak = infDb;
    currentMeters.rms = infDb;
    currentMeters.rmsMax = infDb;
    currentMeters.momentaryMax = infDb;
    currentMeters.shortTermMax = infDb;
    currentMeters.integratedLufs = infDb;
    currentMeters.plr = 0.0f;
    currentMeters.vuValue = -20.0f;
    
    integratedSum = 0.0;
    integratedCount = 0;
    shortTermHistory.clear();
    
    for (int i = 0; i < 200; ++i) 
        currentMeters.history[i] = -70.0f;
}

juce::AudioProcessorEditor* SonicMeterAudioProcessor::createEditor()
{
    return new SonicMeterAudioProcessorEditor (*this);
}

void SonicMeterAudioProcessor::getStateInformation (juce::MemoryBlock& destData) 
{
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(gainFactor);
}

void SonicMeterAudioProcessor::setStateInformation (const void* data, int sizeInBytes) 
{
    juce::MemoryInputStream stream(data, (size_t)sizeInBytes, false);
    gainFactor = stream.readFloat();
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SonicMeterAudioProcessor();
}
