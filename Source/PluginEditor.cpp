#include "PluginProcessor.h"
#include "PluginEditor.h"

SonicMeterAudioProcessorEditor::SonicMeterAudioProcessorEditor (SonicMeterAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (800, 500);
    startTimerHz(30); // 30 FPS UI refresh
}

SonicMeterAudioProcessorEditor::~SonicMeterAudioProcessorEditor() {}

void SonicMeterAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto meters = processor.getMeters();
    
    // Background
    g.fillAll (juce::Colour::fromFloatRGBA (0.05f, 0.05f, 0.05f, 1.0f));

    auto bounds = getLocalBounds().toFloat();
    
    // Draw VU Meter
    drawVUMeter(g, bounds.removeFromLeft(bounds.getWidth() * 0.4f).reduced(20), meters.vuValue);
    
    // Middle Section: Loudness
    auto center = bounds.removeFromLeft(bounds.getWidth() * 0.6f).reduced(10);
    g.setColour(juce::Colours::darkgrey.darker());
    g.fillRoundedRectangle(center, 10.0f);
    
    g.setColour(juce::Colours::cyan);
    g.setFont(20.0f);
    g.drawText("LOUDNESS", center.removeFromTop(40), juce::Justification::centred);
    
    drawDigitalMeter(g, center.removeFromTop(80).reduced(10), "INTEGRATED", meters.integratedLufs, juce::Colours::cyan);
    drawDigitalMeter(g, center.removeFromTop(80).reduced(10), "SHORT TERM", meters.shortTermLufs, juce::Colours::cyan);
    
    // Right Section: Peaks
    auto right = bounds.reduced(10);
    drawDigitalMeter(g, right.removeFromTop(80).reduced(5), "TRUE PEAK", meters.peak, juce::Colours::red);
    drawDigitalMeter(g, right.removeFromTop(80).reduced(5), "PEAK MAX", meters.peakMax, juce::Colours::orange);
}

void SonicMeterAudioProcessorEditor::drawDigitalMeter(juce::Graphics& g, juce::Rectangle<float> area, juce::String label, float value, juce::Colour color)
{
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(area, 4.0f);
    
    g.setColour(juce::Colours::grey);
    g.setFont(10.0f);
    g.drawText(label, area.removeFromTop(15), juce::Justification::left);
    
    g.setColour(color);
    g.setFont(juce::Font("monospace", 30.0f, juce::Font::bold));
    juce::String valStr = value <= -70.0f ? "-inf" : juce::String(value, 1);
    g.drawText(valStr, area, juce::Justification::centredLeft);
}

void SonicMeterAudioProcessorEditor::drawVUMeter(juce::Graphics& g, juce::Rectangle<float> area, float value)
{
    // Simplified Analog VU Drawing
    g.setColour(juce::Colour(0xffe6e2d3));
    g.fillRoundedRectangle(area, 10.0f);
    
    g.setColour(juce::Colours::black);
    float angle = juce::jmap(value, -20.0f, 3.0f, -0.8f, 0.8f);
    auto pivot = area.getBottomLeft().getPointOnLine(area.getWidth() * 0.5f, -10.0f);
    
    juce::Path needle;
    needle.addRectangle(-1.0f, -area.getHeight() * 0.8f, 2.0f, area.getHeight() * 0.8f);
    
    g.saveState();
    g.translate(pivot.x, pivot.y);
    g.rotate(angle);
    g.fillPath(needle);
    g.restoreState();
}

void SonicMeterAudioProcessorEditor::resized() {}
void SonicMeterAudioProcessorEditor::timerCallback() { repaint(); }
