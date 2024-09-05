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
class HypoflatAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    HypoflatAudioProcessorEditor (HypoflatAudioProcessor&);
    ~HypoflatAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    HypoflatAudioProcessor& audioProcessor;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>

        sliderAtt_strength,
        sliderAtt_pink,
        sliderAtt_agc,
        sliderAtt_soften;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        comboAtt_fft_order;
    juce::Slider mSldStrength;
    juce::Slider mSldPink;
    juce::Slider mSldAgc;
    juce::Slider mSldSoften;
    juce::ComboBox mCmbFftOrder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HypoflatAudioProcessorEditor)
};
