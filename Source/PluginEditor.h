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
class SimpleDelayTapeAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleDelayTapeAudioProcessorEditor (SimpleDelayTapeAudioProcessor&);
    ~SimpleDelayTapeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleDelayTapeAudioProcessor& audioProcessor;


    juce::Slider delaySliderI;
    juce::Slider delaySliderD;
    juce::Slider feedbackSlider;
    juce::Slider dryWetSlider;
    juce::Slider wowSlider;
    juce::Slider wowSpeed;

    juce::ToggleButton syncButton{ "Sync to BPM" }, pingPongButton{ "Ping Pong" };
    juce::ComboBox divisionBoxI;
    juce::ComboBox divisionBoxD;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> timeknobI;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> timeknobD;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wowKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speedKnob;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> pingPongAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> divisionAttachmentI;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> divisionAttachmentD;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayTapeAudioProcessorEditor)
};
