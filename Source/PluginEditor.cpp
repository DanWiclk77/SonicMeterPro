/* 
    SonicMeter Pro - Platinum UI Edition
    Precision Engineered for ARM64 (macOS) and high-performance x64.
*/
#include <cmath>
#include <algorithm>
#include "PluginProcessor.h"
#include "PluginEditor.h"

SonicMeterAudioProcessorEditor::SonicMeterAudioProcessorEditor (SonicMeterAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // High-resolution UI dimensions
    setSize (1000, 600);
    
    // Gain Slider - Technical Precision (dB Scale)
    gainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    gainSlider.setRange(-25.0, 25.0, 0.1);
    gainSlider.setTextValueSuffix(" dB");
    gainSlider.setValue((double)processor.getGainDb(), juce::dontSendNotification);
    gainSlider.onValueChange = [this] { processor.setGainDb((float)gainSlider.getValue()); };
    addAndMakeVisible(gainSlider);
    
    // VU Calibration Slider
    calibSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    calibSlider.setRange(-24.0, -8.0, 1.0);
    calibSlider.setValue((double)processor.getCalibration(), juce::dontSendNotification);
    calibSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    calibSlider.onValueChange = [this] { 
        processor.setCalibration((float)calibSlider.getValue()); 
        calibLabel.setText("VU REF: " + juce::String(calibSlider.getValue(), 0) + " dBFS", juce::dontSendNotification);
    };
    addAndMakeVisible(calibSlider);

    calibLabel.setText("VU REF: " + juce::String(calibSlider.getValue(), 0) + " dBFS", juce::dontSendNotification);
    calibLabel.setFont(juce::Font(10.0f, juce::Font::bold));
    calibLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(calibLabel);

    // Preset Selector
    presetSelector.addItemList({"NO PRESET", "SPOTIFY (-14)", "YOUTUBE (-14)", "APPLE MUSIC (-16)", "BEATPORT (-9)", "CLUB / LOUD (-6)"}, 1);
    presetSelector.setSelectedItemIndex((int)processor.getPreset(), juce::dontSendNotification);
    presetSelector.onChange = [this] {
        processor.setPreset((SonicMeterAudioProcessor::StreamingPreset)presetSelector.getSelectedItemIndex());
    };
    addAndMakeVisible(presetSelector);
    
    // Labeling
    gainLabel.setText("INPUT TRIM", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(gainLabel);

    // Reset Button
    resetButton.setButtonText("RESET STATS");
    resetButton.onClick = [this] { processor.resetStats(); };
    addAndMakeVisible(resetButton);

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
    g.fillAll (juce::Colour::fromFloatRGBA (0.05f, 0.05f, 0.06f, 1.0f));

    const juce::Rectangle<float> fullBounds = getLocalBounds().toFloat();
    juce::Rectangle<float> bounds = fullBounds.reduced(20.0f);
    
    // Grid Accents (Background subtle pattern)
    g.setColour(juce::Colours::white.withAlpha(0.02f));
    for (float x = 0.0f; x < fullBounds.getWidth(); x += 40.0f)
        g.drawVerticalLine((int)x, 0.0f, fullBounds.getHeight());
    for (float y = 0.0f; y < fullBounds.getHeight(); y += 40.0f)
        g.drawHorizontalLine((int)y, 0.0f, fullBounds.getWidth());

    // Header Title
    auto headerArea = bounds.removeFromTop(40.0f);
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 14.0f, juce::Font::bold));
    g.drawText("SONIC-METER PRO / PLATINUM EDITION", headerArea, juce::Justification::centred);

    // --- LEFT COLUMN: ANALOG HERITAGE ---
    auto leftColumn = bounds.removeFromLeft(300.0f);
    
    // VU Meter Panel
    auto vuPanel = leftColumn.removeFromTop(320.0f);
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillRoundedRectangle(vuPanel, 6.0f);
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.drawRoundedRectangle(vuPanel, 6.0f, 1.0f);
    
    auto vuContent = vuPanel.reduced(10.0f);
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.setFont(10.0f);
    g.drawText("ANALOG HERITAGE", vuContent.removeFromTop(20), juce::Justification::left);
    drawVUMeter(g, vuContent.removeFromTop(220).reduced(15), meters.vuValue);
    
    // Gain Section (is positioned by resized())

    // --- CENTER COLUMN: LOUDNESS ANALYSIS ---
    bounds.removeFromLeft(20.0f);
    auto centerColumn = bounds.removeFromLeft(340.0f);
    
    // Analysis Panel
    auto analysisPanel = centerColumn;
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillRoundedRectangle(analysisPanel, 8.0f);
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.drawRoundedRectangle(analysisPanel, 8.0f, 1.0f);
    
    auto analysisContent = analysisPanel.reduced(15.0f);
    auto analysisHeader = analysisContent.removeFromTop(30.0f);
    g.setColour(juce::Colours::cyan.withAlpha(0.8f));
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("LOUDNESS ANALYSIS", analysisHeader, juce::Justification::centred);
    
    drawHistoryGraph(g, analysisContent.removeFromTop(140.0f), meters.history, meters.historyIdx);
    
    analysisContent.removeFromTop(15.0f);
    drawDigitalMeter(g, analysisContent.removeFromTop(80.0f).reduced(2.0f), "INTEGRATED LOUDNESS", meters.integratedLufs, juce::Colours::cyan);
    
    auto miniGrid = analysisContent.reduced(2.0f);
    float miniWidth = miniGrid.getWidth() / 2.0f;
    auto row1 = miniGrid.removeFromTop(55.0f);
    drawDigitalMeter(g, row1.removeFromLeft(miniWidth).reduced(2.0f), "SHORT TERM", meters.shortTermLufs, juce::Colours::cyan.withAlpha(0.8f));
    drawDigitalMeter(g, row1.reduced(2.0f), "PLR INTEGRATED", meters.plr, juce::Colours::white.withAlpha(0.6f));
    
    auto row2 = miniGrid.reduced(2.0f);
    drawDigitalMeter(g, row2.removeFromLeft(miniWidth).reduced(2.0f), "MOMENTARY", meters.momentaryDisplay, juce::Colours::cyan.withAlpha(0.7f));
    drawDigitalMeter(g, row2.reduced(2.0f), "MOMENTARY MAX", meters.momentaryMax, juce::Colours::white.withAlpha(0.5f));

    // --- RIGHT COLUMN: TRANSIENT & FIELD ---
    bounds.removeFromLeft(20.0f);
    auto rightColumn = bounds;
    
    // Field Analysis
    auto fieldPanel = rightColumn.removeFromTop(120.0f);
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillRoundedRectangle(fieldPanel, 6.0f);
    
    auto fieldContent = fieldPanel.reduced(10.0f);
    g.setColour(juce::Colours::cyan.withAlpha(0.6f));
    g.setFont(10.0f);
    g.drawText("FIELD ANALYSIS", fieldContent.removeFromTop(15.0f), juce::Justification::left);
    drawCorrelationMeter(g, fieldContent.removeFromTop(45.0f).reduced(5.0f), meters.correlation);
    
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.setFont(10.0f);
    g.drawText("STEREO WIDTH: " + juce::String((int)(meters.stereoWidth * 100)) + "%", fieldContent, juce::Justification::centred);

    // Transient Check
    rightColumn.removeFromTop(15.0f);
    auto transientPanel = rightColumn;
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRoundedRectangle(transientPanel, 6.0f);
    
    auto transientContent = transientPanel.reduced(10.0f);
    g.setColour(juce::Colours::red.withAlpha(0.7f));
    g.setFont(10.0f);
    g.drawText("TRANSIENT CHECK", transientContent.removeFromTop(15.0f), juce::Justification::left);
    
    drawDigitalMeter(g, transientContent.removeFromTop(90.0f).reduced(2.0f), "TRUE PEAK (CURRENT)", meters.truePeakDisplay, juce::Colours::red.withSaturation(0.7f));
    drawDigitalMeter(g, transientContent.removeFromTop(90.0f).reduced(2.0f), "TRUE PEAK MAX", meters.peakMax, juce::Colours::orange.withSaturation(0.7f));
    drawDigitalMeter(g, transientContent.reduced(2.0f), "RMS AVG", meters.rmsDisplay, juce::Colours::white.withAlpha(0.8f));
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
    
    // Grid Lines and Labels
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.setFont(9.0f);
    
    float dbMarkers[] = { 0.0f, -6.0f, -12.0f, -18.0f, -24.0f, -36.0f, -48.0f };
    for (float db : dbMarkers)
    {
        float y = juce::jmap(db, -48.0f, 0.0f, area.getBottom(), area.getY());
        g.drawHorizontalLine((int)y, area.getX(), area.getRight());
        
        if (db == 0.0f || db == -18.0f || db == -48.0f) {
            g.setColour(juce::Colours::white.withAlpha(0.2f));
            g.drawText(juce::String((int)db), area.getX() + 2, (int)y - 10, 30, 20, juce::Justification::left);
            g.setColour(juce::Colours::white.withAlpha(0.05f));
        }
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
    
    // Fill under path
    juce::Path fillP = p;
    fillP.lineTo(area.getRight(), area.getBottom());
    fillP.lineTo(area.getX(), area.getBottom());
    fillP.closeSubPath();
    
    juce::ColourGradient grad(juce::Colours::cyan.withAlpha(0.15f), 0, area.getY(), 
                             juce::Colours::transparentBlack, 0, area.getBottom(), false);
    g.setGradientFill(grad);
    g.fillPath(fillP);

    // Glow effect stroke
    g.setColour(juce::Colours::cyan.withAlpha(0.4f));
    g.strokePath(p, juce::PathStrokeType(2.5f));
    g.setColour(juce::Colours::cyan);
    g.strokePath(p, juce::PathStrokeType(1.0f));
}

void SonicMeterAudioProcessorEditor::drawDigitalMeter(juce::Graphics& g, const juce::Rectangle<float> area, const juce::String label, float value, juce::Colour color)
{
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRoundedRectangle(area, 4.0f);
    
    auto textBounds = area.reduced(8.0f, 4.0f);
    
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText(label.toUpperCase(), textBounds.removeFromTop(12.0f), juce::Justification::left);
    
    g.setColour(color);
    float fontSize = area.getHeight() > 60 ? 32.0f : 24.0f;
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), fontSize, juce::Font::bold));
    
    const juce::String valStr = (value <= -70.0f) ? juce::String("-INF") : juce::String(value, 1);
    g.drawText(valStr, textBounds, juce::Justification::centredLeft);

    // Draw Target Marker if Integrated
    if (label == "INTEGRATED LOUDNESS") {
        float target = 0.0f;
        auto preset = processor.getPreset();
        if (preset == SonicMeterAudioProcessor::Spotify || preset == SonicMeterAudioProcessor::YouTube) target = -14.0f;
        else if (preset == SonicMeterAudioProcessor::AppleMusic) target = -16.0f;
        else if (preset == SonicMeterAudioProcessor::Beatport) target = -9.0f;
        else if (preset == SonicMeterAudioProcessor::Club) target = -6.0f;

        if (target != 0.0f) {
            g.setColour(juce::Colours::white.withAlpha(0.2f));
            g.drawText("TARGET: " + juce::String(target, 0) + " LUFS", textBounds, juce::Justification::centredRight);
            
            // Visual diff indicator
            float diff = value - target;
            juce::Colour diffCol = std::abs(diff) < 1.0f ? juce::Colours::green : (diff > 0 ? juce::Colours::red : juce::Colours::orange);
            g.setColour(diffCol.withAlpha(0.6f));
            g.drawHorizontalLine((int)(textBounds.getBottom() - 2), textBounds.getX(), textBounds.getX() + 40);
        }
    }
}

void SonicMeterAudioProcessorEditor::drawVUMeter(juce::Graphics& g, const juce::Rectangle<float> area, float value)
{
    // Metal Bezel (Outer)
    g.setColour(juce::Colours::grey.darker());
    g.drawRoundedRectangle(area, 6.0f, 2.0f);
    
    auto plate = area.reduced(4.0f);
    
    // Shadow inside bezel
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRoundedRectangle(plate.translated(1, 1), 6.0f);

    g.setColour(juce::Colour(0xfff5f3eb)); // Ivory Grade (Slightly warmer)
    g.fillRoundedRectangle(plate, 6.0f);
    
    const float centerX = plate.getCentreX();
    const float bottomY = plate.getBottom() + 35.0f; // Lower pivot for better scaling
    const float radius = plate.getHeight() * 0.90f;
    
    // Scale Markings
    for (int i = -20; i <= 3; i += 1)
    {
        const float angle = juce::jmap((float)i, -20.0f, 3.0f, -0.65f, 0.65f);
        const float angleRad = angle - juce::MathConstants<float>::halfPi;
        const float tickLen = (i % 5 == 0) ? 10.0f : 5.0f;
        
        const juce::Point<float> pivot(centerX, bottomY);
        const juce::Point<float> p1 = pivot + juce::Point<float> (radius * std::cos (angleRad), radius * std::sin (angleRad));
        const juce::Point<float> p2 = pivot + juce::Point<float> ((radius - tickLen) * std::cos (angleRad), (radius - tickLen) * std::sin (angleRad));
        
        if (i > 0) g.setColour(juce::Colours::red.darker());
        else g.setColour(juce::Colours::black.withAlpha(0.8f));

        g.drawLine(p1.getX(), p1.getY(), p2.getX(), p2.getY(), (i >= 0) ? 2.5f : 1.2f);
        
        if (i % 5 == 0 || i == 3) {
            g.setFont(juce::Font(10.0f, juce::Font::bold));
            const float textRadius = radius - 20.0f;
            const juce::Point<float> pT = pivot + juce::Point<float> (textRadius * std::cos (angleRad), textRadius * std::sin (angleRad));
            g.drawText(juce::String(i), juce::Rectangle<float>(pT.getX() - 15.0f, pT.getY() - 10.0f, 30.0f, 20.0f), juce::Justification::centred);
        }
    }

    // VU Label
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("VU", plate.withTrimmedTop(plate.getHeight() * 0.6f), juce::Justification::centred);

    // Needle - Balanced Dynamics
    const float clampedValue = juce::jlimit(-20.0f, 4.5f, value);
    const float needleAngle = juce::jmap(clampedValue, -20.0f, 3.0f, -0.65f, 0.65f);
    
    juce::Path needle;
    needle.addRoundedRectangle(-1.2f, -radius * 0.94f, 2.4f, radius * 0.94f, 1.0f);
    
    g.setColour(juce::Colours::red.darker(0.3f));
    g.fillPath(needle, juce::AffineTransform::rotation(needleAngle).translated(centerX, bottomY));
    
    // Screw Head
    g.setColour(juce::Colours::black.brighter(0.1f));
    g.fillEllipse(centerX - 8.0f, plate.getBottom() - 12.0f, 16.0f, 16.0f);
}

void SonicMeterAudioProcessorEditor::resized() 
{
    auto bounds = getLocalBounds();
    
    // Header Section - Fixed overlaps
    auto header = bounds.removeFromTop(40);
    resetButton.setBounds(header.removeFromLeft(140).reduced(10, 8));
    presetSelector.setBounds(header.removeFromRight(180).reduced(10, 8));

    // Layout helper
    auto mainArea = bounds.reduced(20);
    
    auto leftArea = mainArea.removeFromLeft(300);
    auto vuSection = leftArea.removeFromTop(320);
    
    // Calibration Slider positioning
    auto calibArea = vuSection.removeFromTop(35).reduced(20, 0);
    calibSlider.setBounds(calibArea.removeFromLeft(calibArea.getWidth() / 2).reduced(0, 5));
    calibLabel.setBounds(calibArea.reduced(0, 5));
    
    // Gain / Trim Section
    auto controlArea = leftArea.reduced(50, 10);
    gainLabel.setBounds(controlArea.removeFromTop(20));
    gainSlider.setBounds(controlArea);
}

void SonicMeterAudioProcessorEditor::timerCallback() 
{ 
    repaint(); 
}

void SonicMeterAudioProcessorEditor::mouseDown (const juce::MouseEvent& e)
{
    processor.resetStats();
}
