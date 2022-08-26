/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DLayAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DLayAudioProcessorEditor (DLayAudioProcessor&);
    ~DLayAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    juce::Slider freqKnob;
    juce::Slider highCutKnob;
    juce::Slider lowCutKnob;
    juce::Slider gainKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highCutAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowCutAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DLayAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DLayAudioProcessorEditor)
};
