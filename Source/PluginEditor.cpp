/* 
    SonicMeter Pro - Platinum UI Edition
    Precision Engineered for ARM64 (macOS) and high-performance x64.
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"

SonicMeterAudioProcessorEditor::SonicMeterAudioProcessorEditor (SonicMeterAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // High-resolution UI dimensions
    setSize (850, 500);
    
    // Gain Slider - Technical Precision
    gainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gainSlider.setRange(0.0, 2.0, 0.01);
    gainSlider.setValue((double)processor.getGain(), juce::dontSendNotification);
    gainSlider.onValueChange = [this] { processor.setGain((float)gainSlider.getValue()); };
    addAndMakeVisible(gainSlider);
    
    // Labeling
    gainLabel.setText("INPUT TRIM", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(gainLabel);

    // High-precision UI sync
    startTimerHz(30); 
}

SonicMeterAudioProcessorEditor::~SonicMeterAudioProcessorEditor() 
{
    stopTimer();
}

void SonicMeterAudioProcessorEditor::paint (juce::Graphics& g)
{
    const SonicMeterAudioProcessor::Meters meters = processor.getMeters();
    
    // Background - Deep Carbon Finish
    g.fillAll (juce::Colour::fromFloatRGBA (0.07f, 0.07f, 0.08f, 1.0f));

    const juce::Rectangle<float> fullBounds = getLocalBounds().toFloat();
    juce::Rectangle<float> bounds = fullBounds.reduced(15.0f);
    
    // Grid Accents (Background subtle pattern)
    g.setColour(juce::Colours::white.withAlpha(0.03f));
    for (float x = 0.0f; x < fullBounds.getWidth(); x += 50.0f)
        g.drawVerticalLine((int)x, 0.0f, fullBounds.getHeight());
    
    // Layout - Left Control Section
    auto leftSection = bounds.removeFromLeft(220.0f);
    drawVUMeter(g, leftSection.removeFromTop(240.0f).reduced(5.0f), meters.vuValue);
    
    // Divider
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawVerticalLine((int)bounds.getX(), bounds.getY(), bounds.getBottom());

    // Layout - Center Analysis Section
    bounds.removeFromLeft(15.0f);
    auto centerSection = bounds.removeFromLeft(380.0f);
    
    // Container Shadow
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(centerSection.translated(2.0f, 2.0f), 8.0f);
    
    // Analysis Container
    g.setColour(juce::Colour::fromFloatRGBA(0.03f, 0.03f, 0.04f, 1.0f));
    g.fillRoundedRectangle(centerSection, 8.0f);
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawRoundedRectangle(centerSection, 8.0f, 1.0f);
    
    auto contentArea = centerSection.reduced(15.0f);
    auto headerArea = contentArea.removeFromTop(30.0f);
    
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 16.0f, juce::Font::bold));
    g.drawText("SONIC SPECTRUM ANALYSIS", headerArea, juce::Justification::centred);
    
    contentArea.removeFromTop(10.0f);
    drawHistoryGraph(g, contentArea.removeFromTop(160.0f), meters.history, meters.historyIdx);
    
    contentArea.removeFromTop(20.0f);
    float digitalHeight = contentArea.getHeight() / 2.0f;
    drawDigitalMeter(g, contentArea.removeFromTop(digitalHeight).reduced(5.0f), "INTEGRATED LUFS", meters.integratedLufs, juce::Colours::cyan);
    drawDigitalMeter(g, contentArea.reduced(5.0f), "MOMENTARY LUFS", meters.momentaryLufs, juce::Colours::cyan.brighter(0.2f));
    
    // Layout - Right Metrics Section
    bounds.removeFromLeft(15.0f);
    auto rightSection = bounds;
    
    // Technical Markers
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.setFont(10.0f);
    g.drawText("PANORAMIC / PHASE", rightSection.removeFromTop(20.0f), juce::Justification::centred);
    drawCorrelationMeter(g, rightSection.removeFromTop(45.0f).reduced(2.0f), meters.correlation);
    
    rightSection.removeFromTop(25.0f);
    drawDigitalMeter(g, rightSection.removeFromTop(110.0f).reduced(5.0f), "TRUE PEAK LEVEL", meters.peak, juce::Colours::red.withSaturation(0.8f));
    drawDigitalMeter(g, rightSection.reduced(5.0f), "PEAK HOLD MAX", meters.peakMax, juce::Colours::orange.withSaturation(0.8f));
}

void SonicMeterAudioProcessorEditor::drawCorrelationMeter(juce::Graphics& g, const juce::Rectangle<float> area, float value)
{
    g.setColour(juce::Colours::black.brighter(0.05f));
    g.fillRoundedRectangle(area, 4.0f);
    
    const juce::Rectangle<float> barArea = area.reduced(8.0f, 4.0f);
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawHorizontalLine((int)barArea.getCentreY(), barArea.getX(), barArea.getRight());
    
    // Center Marker
    g.drawVerticalLine((int)barArea.getCentreX(), barArea.getY(), barArea.getBottom());
    
    const float indicatorX = juce::jmap(value, -1.0f, 1.0f, barArea.getX(), barArea.getRight());
    const juce::Colour glow = value < 0.0f ? juce::Colours::red : juce::Colours::cyan;
    
    g.setColour(glow.withAlpha(0.4f));
    g.fillEllipse(indicatorX - 6.0f, barArea.getCentreY() - 6.0f, 12.0f, 12.0f);
    g.setColour(glow);
    g.fillEllipse(indicatorX - 3.0f, barArea.getCentreY() - 3.0f, 6.0f, 6.0f);
}

void SonicMeterAudioProcessorEditor::drawHistoryGraph(juce::Graphics& g, const juce::Rectangle<float> area, const float* history, int historyIdx)
{
    g.setColour(juce::Colours::black.darker());
    g.fillRoundedRectangle(area, 4.0f);
    
    // Grid Lines
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    for (int db = -48; db <= 0; db += 12)
    {
        float y = juce::jmap((float)db, -48.0f, 0.0f, area.getBottom(), area.getY());
        g.drawHorizontalLine((int)y, area.getX(), area.getRight());
    }

    juce::Path p;
    const float step = area.getWidth() / 200.0f;
    bool started = false;
    
    for (int i = 0; i < 200; ++i)
    {
        const int idx = (historyIdx + i) % 200;
        const float val = juce::jlimit(-48.0f, 0.0f, history[idx]);
        const float x = area.getX() + ((float)i * step);
        const float y = juce::jmap(val, -48.0f, 0.0f, area.getBottom(), area.getY());
        
        if (!started) {
            p.startNewSubPath(x, y);
            started = true;
        } else {
            p.lineTo(x, y);
        }
    }
    
    // Glow effect
    g.setColour(juce::Colours::cyan.withAlpha(0.3f));
    g.strokePath(p, juce::PathStrokeType(3.0f));
    g.setColour(juce::Colours::cyan);
    g.strokePath(p, juce::PathStrokeType(1.2f));
}

void SonicMeterAudioProcessorEditor::drawDigitalMeter(juce::Graphics& g, const juce::Rectangle<float> area, const juce::String label, float value, juce::Colour color)
{
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(area, 4.0f);
    
    auto textBounds = area.reduced(10.0f, 5.0f);
    
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText(label.toUpperCase(), textBounds.removeFromTop(12.0f), juce::Justification::left);
    
    g.setColour(color);
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 28.0f, juce::Font::bold));
    
    const juce::String valStr = (value <= -70.0f) ? juce::String("-INF") : juce::String(value, 1);
    g.drawText(valStr, textBounds, juce::Justification::centredLeft);
}

void SonicMeterAudioProcessorEditor::drawVUMeter(juce::Graphics& g, const juce::Rectangle<float> area, float value)
{
    // Metal Bezel
    g.setColour(juce::Colours::grey.darker());
    g.drawRoundedRectangle(area, 6.0f, 2.0f);
    
    auto plate = area.reduced(2.0f);
    g.setColour(juce::Colour(0xfff0eee4)); // Ivory Grade
    g.fillRoundedRectangle(plate, 6.0f);
    
    const float centerX = plate.getCentreX();
    const float bottomY = plate.getBottom() + 10.0f;
    const float radius = plate.getHeight() * 1.1f;
    
    // Scale Markings
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    for (int i = -20; i <= 3; i += 1)
    {
        const float angle = juce::jmap((float)i, -20.0f, 3.0f, -0.7f, 0.7f);
        const float tickLen = (i % 5 == 0) ? 12.0f : 6.0f;
        
        const juce::Point<float> pivot(centerX, bottomY);
        const juce::Point<float> p1 = pivot.getPointOnLine(radius, angle - juce::MathConstants<float>::halfPi);
        const juce::Point<float> p2 = pivot.getPointOnLine(radius - tickLen, angle - juce::MathConstants<float>::halfPi);
        
        g.drawLine(p1.getX(), p1.getY(), p2.getX(), p2.getY(), (i > 0) ? 2.5f : 1.5f);
        
        if (i > 0) g.setColour(juce::Colours::red.darker());
        else g.setColour(juce::Colours::black.withAlpha(0.8f));

        if (i % 5 == 0 || i == 3) {
            g.setFont(juce::Font(11.0f, juce::Font::bold));
            const juce::Point<float> pT = pivot.getPointOnLine(radius - 22.0f, angle - juce::MathConstants<float>::halfPi);
            g.drawText(juce::String(i), juce::Rectangle<float>(pT.getX() - 15.0f, pT.getY() - 10.0f, 30.0f, 20.0f), juce::Justification::centred);
        }
    }

    // Needle - Balanced Dynamics
    const float clampedValue = juce::jlimit(-20.0f, 5.0f, value);
    const float needleAngle = juce::jmap(clampedValue, -20.0f, 3.0f, -0.7f, 0.7f);
    
    juce::Path needle;
    needle.addRoundedRectangle(-1.2f, -radius * 0.92f, 2.4f, radius * 0.92f, 1.0f);
    
    g.setColour(juce::Colours::red.darker(0.2f));
    g.fillPath(needle, juce::AffineTransform::rotation(needleAngle).translated(centerX, bottomY));
    
    // Screw Head
    g.setColour(juce::Colours::black.brighter(0.1f));
    g.fillEllipse(centerX - 6.0f, bottomY - plate.getHeight() * 0.1f, 12.0f, 12.0f);
}

void SonicMeterAudioProcessorEditor::resized() 
{
    const auto bounds = getLocalBounds().toFloat().reduced(15.0f);
    auto leftArea = bounds.removeFromLeft(220.0f);
    leftArea.removeFromTop(250.0f); // Top VU Reserved
    
    auto controlArea = leftArea.reduced(25.0f, 10.0f);
    gainLabel.setBounds(controlArea.removeFromTop(20).toNearestInt());
    gainSlider.setBounds(controlArea.toNearestInt());
}

void SonicMeterAudioProcessorEditor::timerCallback() 
{ 
    repaint(); 
}

void SonicMeterAudioProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    processor.resetStats();
}
