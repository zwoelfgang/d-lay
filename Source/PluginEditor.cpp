/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DLayAudioProcessorEditor::DLayAudioProcessorEditor (DLayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(&freqKnob);
    freqKnob.setTextValueSuffix("Timing");
    freqKnob.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    freqKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(&lowCutKnob);
    lowCutKnob.setTextValueSuffix("Low Cut");
    lowCutKnob.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    lowCutKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(&highCutKnob);
    highCutKnob.setTextValueSuffix("High Cut");
    highCutKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    highCutKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 100);

    addAndMakeVisible(&gainKnob);
    gainKnob.setTextValueSuffix("Timing");
    gainKnob.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    gainKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 100);

    freqAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "timing", freqKnob));
    lowCutAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "low cut", lowCutKnob));
    highCutAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "high cut", highCutKnob));
    gainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(p.getState(), "gain", gainKnob));

    setSize (600, 400);
}

DLayAudioProcessorEditor::~DLayAudioProcessorEditor()
{
}

//==============================================================================
void DLayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
    g.setColour(juce::Colours::white);

    freqKnob.setColour(0xffffffff, juce::Colours::white);
    lowCutKnob.setColour(0xffffffff, juce::Colours::white);
    highCutKnob.setColour(0xffffffff, juce::Colours::white);
    gainKnob.setColour(0xffffffff, juce::Colours::white);

    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 20.0f, juce::Font::bold));
    g.drawText("Timing", ((getWidth() / 2) - 200 / 2), ((getHeight() / 2) - 150 / 2), 200, 200, juce::Justification::centred);
    g.drawText("Low Cut", ((getWidth() / 2) - 350 / 2), ((getHeight() / 2) + 170 / 2), 100, 100, juce::Justification::centred);
    g.drawText("High Cut", ((getWidth() / 2) + 150 / 2), ((getHeight() / 2) + 170 / 2), 100, 100, juce::Justification::centred);
    g.drawText("Gain", ((getWidth() / 2) - 100 / 2), ((getHeight() / 2) + 200 / 2), 100, 100, juce::Justification::centred);
}

void DLayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    freqKnob.setBounds(((getWidth() / 2) - 150 / 2), ((getHeight() / 2) - 250 / 2), 150, 150);
    lowCutKnob.setBounds(((getWidth() / 2) - 330 / 2), ((getHeight() / 2) + 100 / 2), 80, 80);
    highCutKnob.setBounds(((getWidth() / 2) + 170 / 2), ((getHeight() / 2) + 100 / 2), 80, 80);
    gainKnob.setBounds(((getWidth() / 2) - 80 / 2), ((getHeight() / 2) + 150 / 2), 80, 80);
}
