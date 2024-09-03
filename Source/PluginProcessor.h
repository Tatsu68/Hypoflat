/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FreqEngine.h"
//==============================================================================
/**
*/
class HypoflatAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    HypoflatAudioProcessor();
    ~HypoflatAudioProcessor() override;

    juce::AudioProcessorValueTreeState apvts;
    inline static const juce::StringArray choiceItems_fftOrder = { "256","512","1024","2048","4096","8192","16384"};
    inline static const juce::StringArray choiceItems_oversample = { "1x", "2x", "4x" };
    
    
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

private:
    int mFftSize = 0;
    int mOversample = 0;

    std::unique_ptr<FreqEngine> mEngine;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    void checkAndResetFreqEngine(bool alwaysReset);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HypoflatAudioProcessor)
};
