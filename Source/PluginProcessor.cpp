/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HypoflatAudioProcessor::HypoflatAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(
                           *this, nullptr, "Parameters", createParameters()
                       )
#endif
{
}

HypoflatAudioProcessor::~HypoflatAudioProcessor()
{
}

//==============================================================================
const juce::String HypoflatAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HypoflatAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HypoflatAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HypoflatAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HypoflatAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HypoflatAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HypoflatAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HypoflatAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String HypoflatAudioProcessor::getProgramName (int index)
{
    return {};
}

void HypoflatAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void HypoflatAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    checkAndResetFreqEngine(true);
}

void HypoflatAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HypoflatAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void HypoflatAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }

    checkAndResetFreqEngine(false);
    float strength = apvts.getParameterAsValue("strength").getValue();
    float pink = apvts.getParameterAsValue("pink").getValue();
    mEngine->setParams(FreqEngine::Params{
        strength,
        pink,
        });
    //if (strength > 0.5) mEngine->process(buffer.getArrayOfWritePointers(), buffer.getNumSamples());
    mEngine->process(buffer.getArrayOfWritePointers(), buffer.getNumSamples());
}

//==============================================================================
bool HypoflatAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HypoflatAudioProcessor::createEditor()
{
    return new HypoflatAudioProcessorEditor (*this);
}

//==============================================================================
void HypoflatAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HypoflatAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    auto mb = juce::MidiBuffer();
}

juce::AudioProcessorValueTreeState::ParameterLayout HypoflatAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "fft_order",
        "FFT Order",
        choiceItems_fftOrder,
        3));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "oversample",
        "Oversample",
        choiceItems_oversample,
        0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "strength",
        "Strength",
        0.0f,
        1.0f,
        0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "pink",
        "pink",
        0.0f,
        1.0f,
        0.0f));
    return { params.begin(), params.end() };
}

void HypoflatAudioProcessor::checkAndResetFreqEngine(bool alwaysReset)
{
    int shiftFft = apvts.getParameterAsValue("fft_order").getValue();
    int shiftOs = apvts.getParameterAsValue("oversample").getValue();
    int size = 256 << shiftFft;
    int os = 1 << shiftOs;
    bool check = (mFftSize != size) || (mOversample != os);
    check = check || alwaysReset;
    setLatencySamples(size);
    mFftSize = size;
    mOversample = os;
    if (check)
        mEngine = std::make_unique < FreqEngine>(size, getSampleRate() * os);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HypoflatAudioProcessor();
}
