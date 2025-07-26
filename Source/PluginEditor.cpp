/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDelayTapeAudioProcessorEditor::SimpleDelayTapeAudioProcessorEditor (SimpleDelayTapeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    delaySliderI.setSliderStyle(juce::Slider::Rotary);
    delaySliderI.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    delaySliderI.setRange(0.1f, 2000.0f, 500.0f);
    addAndMakeVisible(delaySliderI);

    delaySliderD.setSliderStyle(juce::Slider::Rotary);
    delaySliderD.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    delaySliderD.setRange(0.1f, 2000.0f, 500.0f);
    addAndMakeVisible(delaySliderD);

    feedbackSlider.setSliderStyle(juce::Slider::Rotary);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    feedbackSlider.setRange(0.0f, 0.7f, 0.1f);
    addAndMakeVisible(feedbackSlider);

    dryWetSlider.setSliderStyle(juce::Slider::Rotary);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    dryWetSlider.setRange(0.0f, 1.0f, 0.5f);
    addAndMakeVisible(dryWetSlider);

    wowSlider.setSliderStyle(juce::Slider::Rotary);
    wowSlider.setRange(0.0f, 10.0f, 0.0f);
    wowSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    addAndMakeVisible(wowSlider);

    wowSpeed.setSliderStyle(juce::Slider::Rotary);
    wowSpeed.setRange(0.1f, 4.0f, 0.0f);
    wowSpeed.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    addAndMakeVisible(wowSpeed);

    
    
    divisionBoxI.addItem("1/4", 1);
    divisionBoxI.addItem("1/4.", 2);
    divisionBoxI.addItem("1/4T", 3);
    divisionBoxI.addItem("1/8", 4);
    divisionBoxI.addItem("1/8.", 5);
    divisionBoxI.addItem("1/8T", 6);
    divisionBoxI.addItem("1/16", 7);

 
    divisionBoxD.addItem("1/4", 1);
    divisionBoxD.addItem("1/4.", 2);
    divisionBoxD.addItem("1/4T", 3);
    divisionBoxD.addItem("1/8", 4);
    divisionBoxD.addItem("1/8.", 5);
    divisionBoxD.addItem("1/8T", 6);
    divisionBoxD.addItem("1/16", 7);

    addAndMakeVisible(divisionBoxI);
    addAndMakeVisible(divisionBoxD);
    addAndMakeVisible(syncButton);
    addAndMakeVisible(pingPongButton);
    

    timeknobI = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "timeI", delaySliderI);
    timeknobD = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "timeD", delaySliderD);
    feedbackKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "feedback", feedbackSlider);
    dryWetKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "dryWet", dryWetSlider);
    wowKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "wow", wowSlider);
    speedKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "wowSpeed", wowSpeed);
    syncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.tree, "syncOn", syncButton);
    divisionAttachmentI = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.tree, "noteDivisionI", divisionBoxI);
    divisionAttachmentD = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.tree, "noteDivisionD", divisionBoxD);
    pingPongAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.tree, "ping pong", pingPongButton);
}

SimpleDelayTapeAudioProcessorEditor::~SimpleDelayTapeAudioProcessorEditor()
{
}

//==============================================================================
void SimpleDelayTapeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setColour(juce::Colours::white);
    g.drawText("TimeI", 10, 5, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
    g.drawText("TimeD", (getWidth() / 4) + 10, 5, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
    g.drawText("Feedback", 2 * (getWidth() / 4) + 10, 5, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
    g.drawText("DryWet", 3 * (getWidth() / 4) + 10, 5, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
    g.drawText("Wow", 10, 160, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
    g.drawText("WowSpeed", 10 + (getWidth() / 4), 160, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
}

void SimpleDelayTapeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    delaySliderI.setBounds(10, 10, (getWidth() / 4) - 10, 100);
    delaySliderD.setBounds((getWidth() / 4) + 10, 10, (getWidth() / 4) - 10, 100);
    feedbackSlider.setBounds(2 * (getWidth() / 4) + 10, 10, (getWidth() / 4) - 10, 100);
    dryWetSlider.setBounds(3 * (getWidth() / 4) + 10, 10, (getWidth() / 4) - 10, 100);
    divisionBoxI.setBounds(10, 120, (getWidth() / 4) - 10, 20);
    divisionBoxD.setBounds((getWidth() / 4) + 10, 120, (getWidth() / 4) - 10, 20);
    syncButton.setBounds(2*(getWidth() / 4) + 10, 110, (getWidth() / 4) - 10, 100);
    pingPongButton.setBounds(3 * (getWidth() / 4) + 10, 110, (getWidth() / 4) - 10, 100);
    wowSlider.setBounds(10, 150, (getWidth() / 4), 100);
    wowSpeed.setBounds(10 + (getWidth() / 4), 150, (getWidth() / 4), 100);
}
