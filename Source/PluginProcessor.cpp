/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DLayAudioProcessor::DLayAudioProcessor()
    : state (*this, nullptr, "PARAMETERS",
              { 
                std::make_unique<juce::AudioParameterFloat> ("timing", "Timing", juce::NormalisableRange<float>(0.0625f, 1.f, 0.0625f), 0.125f),
                std::make_unique<juce::AudioParameterFloat> ("low cut", "Low Cut", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.2f, false), 20.f),
                std::make_unique<juce::AudioParameterFloat> ("high cut", "High Cut", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.2f, false), 20000.f),
                std::make_unique<juce::AudioParameterFloat> ("gain", "Gain", juce::NormalisableRange<float>(0.1f, 1.f, 0.0001f), 0.5f)
              })
{
}

DLayAudioProcessor::~DLayAudioProcessor()
{
}

//==============================================================================
const juce::String DLayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DLayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DLayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DLayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DLayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DLayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DLayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DLayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DLayAudioProcessor::getProgramName (int index)
{
    return {};
}

void DLayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DLayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    auto chainSettings = getChainSettings(state);
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCut, sampleRate, 4);
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCut, sampleRate, 4);

    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();

    leftLowCut.get<0>().coefficients = lowCutCoefficients[0];
    rightLowCut.get<0>().coefficients = lowCutCoefficients[0];

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

    leftHighCut.get<0>().coefficients = highCutCoefficients[0];
    rightHighCut.get<0>().coefficients = highCutCoefficients[0];
}

void DLayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DLayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DLayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    freq = *state.getRawParameterValue("timing");
    gain = *state.getRawParameterValue("gain");

    auto chainSettings = getChainSettings(state);

    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCut, getSampleRate(), 4);
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCut, getSampleRate(), 4);

    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();

    leftLowCut.get<0>().coefficients = lowCutCoefficients[0];
    rightLowCut.get<0>().coefficients = lowCutCoefficients[0];

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();

    leftHighCut.get<0>().coefficients = highCutCoefficients[0];
    rightHighCut.get<0>().coefficients = highCutCoefficients[0];

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    playHead = this->getPlayHead();
    position = playHead->getPosition();
    bpm = position->getBpm();

    auto delayBufferSamples = getSampleRate() * (float)(*bpm / 60.f);
    delayBuffer.setSize (getTotalNumOutputChannels(), (int)(delayBufferSamples / freq));

    auto bufferSize = buffer.getNumSamples();
    auto delayBufferSize = delayBuffer.getNumSamples();

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...

        if (delayBufferSize > bufferSize + writePosition)
        {
            delayBuffer.copyFromWithRamp (channel, writePosition, channelData, bufferSize, 0.1f, 0.1f);
        }
        else
        {
            auto numSamplesToEnd = delayBufferSize - writePosition;

            delayBuffer.copyFromWithRamp (channel, writePosition, channelData, numSamplesToEnd, 0.1f, 0.1f);

            auto numSamplesAtStart = bufferSize - numSamplesToEnd;

            delayBuffer.copyFromWithRamp (channel, 0, channelData + numSamplesToEnd, numSamplesAtStart, 0.1f, 0.1f);
        }

        auto readPosition = writePosition - getSampleRate();

        if (readPosition + bufferSize < delayBufferSize)
        {
            buffer.addFromWithRamp (channel, 0, delayBuffer.getReadPointer (channel, readPosition), bufferSize, gain, gain);
        }
        else
        {
            auto numSamplesToEnd = delayBufferSize - readPosition;
            buffer.addFromWithRamp (channel, 0, delayBuffer.getReadPointer(channel, readPosition), numSamplesToEnd, gain, gain);

            auto numSamplesAtStart = bufferSize - numSamplesToEnd;
            buffer.addFromWithRamp (channel, numSamplesToEnd, delayBuffer.getReadPointer (channel, 0), numSamplesAtStart, gain, gain);
        }
    }

    writePosition += buffer.getNumSamples();
    writePosition %= delayBufferSize;
}

juce::AudioProcessorValueTreeState& DLayAudioProcessor::getState()
{
    return state;
}

//==============================================================================
bool DLayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DLayAudioProcessor::createEditor()
{
    return new DLayAudioProcessorEditor (*this);
}

//==============================================================================
void DLayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream stream(destData, true);
    state.state.writeToStream(stream);
}

void DLayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ValueTree tree = juce::ValueTree::readFromData(data, sizeInBytes);

    if (tree.isValid())
    {
        state.replaceState(tree);
    }
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& state)
{
    ChainSettings settings;

    settings.lowCut = state.getRawParameterValue("low cut")->load();
    settings.highCut = state.getRawParameterValue("high cut")->load();

    return settings;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DLayAudioProcessor();
}
