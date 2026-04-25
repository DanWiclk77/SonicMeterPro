/* SonicMeter Pro UI - Modern Visualization */
#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

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
    
    drawHistoryGraph(g, center.removeFromTop(120).reduced(10), meters.history, meters.historyIdx);
    
    drawDigitalMeter(g, center.removeFromTop(80).reduced(10), "INTEGRATED", meters.integratedLufs, juce::Colours::cyan);
    drawDigitalMeter(g, center.removeFromTop(80).reduced(10), "SHORT TERM", meters.shortTermLufs, juce::Colours::cyan);
    
    // Right Section: Peaks & Analysis
    auto right = bounds.reduced(10);
    drawCorrelationMeter(g, right.removeFromTop(40).reduced(5), meters.correlation);
    right.removeFromTop(10);
    drawDigitalMeter(g, right.removeFromTop(80).reduced(5), "TRUE PEAK", meters.peak, juce::Colours::red);
    drawDigitalMeter(g, right.removeFromTop(80).reduced(5), "PEAK MAX", meters.peakMax, juce::Colours::orange);
}

void SonicMeterAudioProcessorEditor::drawCorrelationMeter(juce::Graphics& g, juce::Rectangle<float> area, float value)
{
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(area, 4.0f);
    
    auto barArea = area.reduced(5);
    g.setColour(juce::Colours::darkgrey);
    g.drawHorizontalLine(barArea.getCentreY(), barArea.getX(), barArea.getRight());
    
    g.setColour(value < 0 ? juce::Colours::red : juce::Colours::cyan);
    float x = juce::jmap(value, -1.0f, 1.0f, barArea.getX(), barArea.getRight());
    g.fillEllipse(x - 3, barArea.getCentreY() - 3, 6, 6);
}

void SonicMeterAudioProcessorEditor::drawHistoryGraph(juce::Graphics& g, juce::Rectangle<float> area, const float* history, int historyIdx)
{
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(area, 4.0f);
    
    g.setColour(juce::Colours::cyan.withAlpha(0.2f));
    juce::Path p;
    float step = area.getWidth() / 200.0f;
    
    for (int i = 0; i < 200; ++i)
    {
        int idx = (historyIdx + i) % 200;
        float val = juce::jlimit(-48.0f, 0.0f, history[idx]);
        float y = juce::jmap(val, -48.0f, 0.0f, area.getBottom(), area.getY());
        
        if (i == 0) p.startNewSubPath(area.getX(), y);
        else p.lineTo(area.getX() + (i * step), y);
    }
    
    g.setColour(juce::Colours::cyan);
    g.strokePath(p, juce::PathStrokeType(1.5f));
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
    // Background plate
    g.setColour(juce::Colour(0xffe6e2d3)); // Classic cream color
    g.fillRoundedRectangle(area, 10.0f);
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.drawRoundedRectangle(area, 10.0f, 2.0f);
    
    // Scale
    g.setColour(juce::Colours::black);
    float centerX = area.getCentreX();
    float bottomY = area.getBottom() - 20.0f;
    float radius = area.getHeight() * 0.85f;
    
    for (int i = -20; i <= 3; i += 2)
    {
        float angle = juce::jmap((float)i, -20.0f, 3.0f, -0.8f, 0.8f);
        auto p1 = juce::Point<float>(centerX, bottomY).getPointOnLine(radius, angle - juce::MathConstants<float>::halfPi);
        auto p2 = juce::Point<float>(centerX, bottomY).getPointOnLine(radius - 10, angle - juce::MathConstants<float>::halfPi);
        g.drawLine(p1.x, p1.y, p2.x, p2.y, 2.0f);
        
        if (i % 4 == 0 || i == 0 || i == 3) {
            g.setFont(12.0f);
            auto pText = juce::Point<float>(centerX, bottomY).getPointOnLine(radius - 25, angle - juce::MathConstants<float>::halfPi);
            g.drawText(juce::String(i), pText.x - 15, pText.y - 10, 30, 20, juce::Justification::centred);
        }
    }

    // Needle
    float angle = juce::jmap(juce::jlimit(-20.0f, 6.0f, value), -20.0f, 3.0f, -0.8f, 0.8f);
    juce::Path needle;
    needle.addRectangle(-1.5f, -radius, 3.0f, radius);
    
    g.setColour(juce::Colours::red.darker());
    g.fillPath(needle, juce::AffineTransform::rotation(angle).translated(centerX, bottomY));
    
    // Pivot cap
    g.setColour(juce::Colours::black);
    g.fillEllipse(centerX - 10, bottomY - 10, 20, 20);
}

void SonicMeterAudioProcessorEditor::resized() {}
void SonicMeterAudioProcessorEditor::timerCallback() { repaint(); }
