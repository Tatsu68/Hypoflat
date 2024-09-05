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

    mSldPink.setBounds(20, 260, 60, 20);
    mSldPink.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mSldPink.setSliderStyle(juce::Slider::LinearHorizontal);
    addAndMakeVisible(mSldPink);
    mSldAgc.setBounds(80, 260, 60, 20);
    mSldAgc.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mSldAgc.setSliderStyle(juce::Slider::LinearHorizontal);
    addAndMakeVisible(mSldAgc);
    mSldSoften.setBounds(140, 260, 60, 20);
    mSldSoften.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mSldSoften.setSliderStyle(juce::Slider::LinearHorizontal);
    addAndMakeVisible(mSldSoften);

    mCmbFftOrder.addItemList(HypoflatAudioProcessor::choiceItems_fftOrder, 1);
    mCmbFftOrder.setBounds(10, 10, 80, 30);
    addAndMakeVisible(mCmbFftOrder);

    sliderAtt_strength = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "strength", mSldStrength);
    sliderAtt_pink = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "pink", mSldPink);
    sliderAtt_agc = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "agc", mSldAgc);
    sliderAtt_soften = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "soften", mSldSoften);
    comboAtt_fft_order = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.apvts, "fft_order", mCmbFftOrder);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (300, 300);
}

HypoflatAudioProcessorEditor::~HypoflatAudioProcessorEditor()
{
    sliderAtt_strength.reset();
    sliderAtt_pink.reset();
    sliderAtt_agc.reset();
    sliderAtt_soften.reset();
    comboAtt_fft_order.reset();
}

//==============================================================================
void HypoflatAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(64,64,96));
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText(juce::String("Hypo Hype\n" )+juce::String(JucePlugin_VersionString) + " " + juce::String(__DATE__) + "\n" + juce::String(__TIME__), getLocalBounds(), juce::Justification::centred, 1);
    g.setFont(juce::FontOptions(12.0f));
    g.drawFittedText(juce::String("Pink"),juce::Rectangle(20,280,60,20), juce::Justification::centred, 1);
    g.drawFittedText(juce::String("AGC"),juce::Rectangle(80,280,60,20), juce::Justification::centred, 1);
    g.drawFittedText(juce::String("Soften"),juce::Rectangle(140,280,60,20), juce::Justification::centred, 1);
}

void HypoflatAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
