/* SonicMeter Pro UI - Modern Visualization */
#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

SonicMeterAudioProcessorEditor::SonicMeterAudioProcessorEditor (SonicMeterAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (850, 500);
    
    // Gain Slider Configuration
    gainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gainSlider.setRange(0.0, 2.0, 0.01);
    gainSlider.setValue(processor.getGain());
    gainSlider.onValueChange = [this] { processor.setGain((float)gainSlider.getValue()); };
    addAndMakeVisible(gainSlider);
    
    gainLabel.setText("INPUT GAIN", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setFont(12.0f);
    addAndMakeVisible(gainLabel);

    startTimerHz(30); 
}

SonicMeterAudioProcessorEditor::~SonicMeterAudioProcessorEditor() {}

void SonicMeterAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto meters = processor.getMeters();
    
    // Background - Dark technical finish
    g.fillAll (juce::Colour::fromFloatRGBA (0.08f, 0.08f, 0.1f, 1.0f));

    const juce::Rectangle<float> fullBounds = getLocalBounds().toFloat();
    auto bounds = fullBounds.reduced(10);
    
    // Left Section: VU Meter + Gain
    auto leftSection = bounds.removeFromLeft(220.0f);
    
    // VU Meter at top
    drawVUMeter(g, leftSection.removeFromTop(200.0f).reduced(5), meters.vuValue);
    
    // Divider
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawVerticalLine(bounds.getX() - 5, bounds.getY() + 10, bounds.getBottom() - 10);

    // Middle Section: Loudness Stats
    auto centerSection = bounds.removeFromLeft(380.0f).reduced(10);
    g.setColour(juce::Colour::fromFloatRGBA(0.04f, 0.04f, 0.05f, 1.0f));
    g.fillRoundedRectangle(centerSection, 6.0f);
    
    auto headerArea = centerSection.removeFromTop(40);
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 18.0f, juce::Font::bold));
    g.drawText("LOUDNESS ANALYSIS", headerArea, juce::Justification::centred);
    
    drawHistoryGraph(g, centerSection.removeFromTop(140).reduced(10), meters.history, meters.historyIdx);
    
    auto metersGrid = centerSection.reduced(10);
    float rowHeight = metersGrid.getHeight() / 2.0f;
    drawDigitalMeter(g, metersGrid.removeFromTop(rowHeight).reduced(5), "INTEGRATED (LUFS)", meters.integratedLufs, juce::Colours::cyan);
    drawDigitalMeter(g, metersGrid.reduced(5), "SHORT TERM (LUFS)", meters.shortTermLufs, juce::Colours::cyan);
    
    // Right Section: Peaks & Analysis
    auto rightSection = bounds.reduced(10);
    
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(12.0f);
    g.drawText("PHASE / STEREO", rightSection.removeFromTop(20), juce::Justification::centred);
    drawCorrelationMeter(g, rightSection.removeFromTop(40).reduced(5), meters.correlation);
    
    rightSection.removeFromTop(20);
    drawDigitalMeter(g, rightSection.removeFromTop(100).reduced(5), "TRUE PEAK", meters.peak, juce::Colours::red);
    drawDigitalMeter(g, rightSection.reduced(5), "PEAK MAX", meters.peakMax, juce::Colours::orange);
}

void SonicMeterAudioProcessorEditor::drawCorrelationMeter(juce::Graphics& g, juce::Rectangle<float> area, float value)
{
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(area, 4.0f);
    
    auto barArea = area.reduced(5);
    g.setColour(juce::Colours::darkgrey);
    g.drawHorizontalLine(barArea.getCentreY(), barArea.getX(), barArea.getRight());
    
    g.setColour(value < 0 ? juce::Colours::red : juce::Colours::cyan);
    float xPos = juce::jmap(value, -1.0f, 1.0f, barArea.getX(), barArea.getRight());
    g.fillEllipse(xPos - 3.0f, barArea.getCentreY() - 3.0f, 6.0f, 6.0f);
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
    g.drawText(label, area.removeFromTop(15.0f), juce::Justification::left);
    
    g.setColour(color);
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 30.0f, juce::Font::bold));
    const juce::String valStr = (value <= -70.0f) ? juce::String("-inf") : juce::String(value, 1);
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
        float scaleAngle = juce::jmap((float)i, -20.0f, 3.0f, -0.8f, 0.8f);
        const juce::Point<float> p1 = juce::Point<float>(centerX, bottomY).getPointOnLine(radius, scaleAngle - juce::MathConstants<float>::halfPi);
        const juce::Point<float> p2 = juce::Point<float>(centerX, bottomY).getPointOnLine(radius - 10.0f, scaleAngle - juce::MathConstants<float>::halfPi);
        g.drawLine(p1.x, p1.y, p2.x, p2.y, 2.0f);
        
        if (i % 4 == 0 || i == 0 || i == 3) {
            g.setFont(12.0f);
            const juce::Point<float> pText = juce::Point<float>(centerX, bottomY).getPointOnLine(radius - 20.0f, scaleAngle - juce::MathConstants<float>::halfPi);
            g.drawText(juce::String(i), juce::Rectangle<float>(pText.getX() - 15.0f, pText.getY() - 10.0f, 30.0f, 20.0f), juce::Justification::centred, false);
        }
    }

    // Needle
    float clampedValue = juce::jlimit(-20.0f, 6.0f, value);
    float angle = juce::jmap(clampedValue, -20.0f, 3.0f, -0.8f, 0.8f);
    
    juce::Path needle;
    needle.addRectangle(-1.0f, -radius * 0.95f, 2.0f, radius * 0.95f);
    
    g.setColour(juce::Colours::red.darker());
    g.fillPath(needle, juce::AffineTransform::rotation(angle).translated(centerX, bottomY));
    
    // Pivot cap
    g.setColour(juce::Colours::black);
    g.fillEllipse(centerX - 8.0f, bottomY - 8.0f, 16.0f, 16.0f);
}

void SonicMeterAudioProcessorEditor::resized() 
{
    auto bounds = getLocalBounds().toFloat().reduced(10.0f);
    auto leftSection = bounds.removeFromLeft(220.0f);
    leftSection.removeFromTop(200.0f); // Skip VU area
    
    auto gainArea = leftSection.reduced(30.0f, 20.0f);
    gainLabel.setBounds(gainArea.removeFromTop(20.0f).toNearestInt());
    gainSlider.setBounds(gainArea.toNearestInt());
}
void SonicMeterAudioProcessorEditor::timerCallback() { repaint(); }

void SonicMeterAudioProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    processor.resetStats();
}
