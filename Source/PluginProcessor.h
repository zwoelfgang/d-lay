/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ChainSettings
{
  float lowCut{0};
  float highCut{0};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState &state);

//==============================================================================
/**
*/
class DLayAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    DLayAudioProcessor();
    ~DLayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getState();

private:
    juce::AudioProcessorValueTreeState state;

    juce::AudioPlayHead* playHead;
    juce::Optional<juce::AudioPlayHead::PositionInfo> position;
    juce::Optional<double> bpm;

    float freq;
    float highCut;
    float lowCut;
    float gain;

    using Filter = juce::dsp::IIR::Filter<float>;
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter>;
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, CutFilter>;
    MonoChain leftChain, rightChain;

    enum ChainPositions
    {
      LowCut,
      HighCut,
    };

    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DLayAudioProcessor)
};
