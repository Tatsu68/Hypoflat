/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HypoflatAudioProcessorEditor::HypoflatAudioProcessorEditor (HypoflatAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    mSldStrength.setBounds(50, 50, 200, 200);
    mSldStrength.setTextBoxStyle(juce::Slider::NoTextBox, false, 0,0);
    mSldStrength.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    addAndMakeVisible(mSldStrength);


    sliderAtt_strength = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "strength", mSldStrength);
    sliderAtt_fft_order = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "fft_order", mSldFftOrder);
    sliderAtt_oversample = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "oversample", mSldOversample);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (300, 300);
}

HypoflatAudioProcessorEditor::~HypoflatAudioProcessorEditor()
{
    sliderAtt_strength.reset();
    sliderAtt_fft_order.reset();
    sliderAtt_oversample.reset();
}

//==============================================================================
void HypoflatAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText(juce::String("Hypo Hype\nb - ") + juce::String(__DATE__) + "\n" + juce::String(__TIME__), getLocalBounds(), juce::Justification::centred, 1);
    //g.drawFittedText (__TIME__, getLocalBounds(), juce::Justification::centred, 1);
}

void HypoflatAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
