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
    setSize(1200, 200);

    preAmpLabel = new juce::Label("", "Pre Amp");
    preAmpSlider.setSliderStyle(juce::Slider::Rotary);
    preAmpSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    preAmpSlider.setRange(-30.0f, 3.0f, -3.0f);
    preAmpLabel->setJustificationType(juce::Justification::centred);
    preAmpLabel->attachToComponent(&preAmpSlider, false);

    addAndMakeVisible(preAmpSlider);

    delayILabel = new juce::Label("", "DelayI ms");
    delaySliderI.setSliderStyle(juce::Slider::Rotary);
    delaySliderI.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    delaySliderI.setRange(0.1f, 2000.0f, 500.0f);
    delayILabel->setJustificationType(juce::Justification::centred);
    delayILabel->attachToComponent(&delaySliderI, false);
   
   
    addAndMakeVisible(delaySliderI);

    delayDLabel = new juce::Label("", "DelayD ms");
    delaySliderD.setSliderStyle(juce::Slider::Rotary);
    delaySliderD.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    delaySliderD.setRange(0.1f, 2000.0f, 500.0f);
    delayDLabel->setJustificationType(juce::Justification::centred);
    delayDLabel->attachToComponent(&delaySliderD, false);
    addAndMakeVisible(delaySliderD);

    feedbackLabel = new juce::Label("", "Feedback");
    feedbackSlider.setSliderStyle(juce::Slider::Rotary);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    feedbackSlider.setRange(0.0f, 0.7f, 0.1f);
    feedbackLabel->setJustificationType(juce::Justification::centred);
    feedbackLabel->attachToComponent(&feedbackSlider, false);
    addAndMakeVisible(feedbackSlider);

    tapeWearLabel = new juce::Label("", "TapeWear");
    tapeWearSlider.setSliderStyle(juce::Slider::Rotary);
    tapeWearSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    tapeWearSlider.setRange(0.5f, 5.0f, 0.5f);
    tapeWearLabel->setJustificationType(juce::Justification::centred);
    tapeWearLabel->attachToComponent(&tapeWearSlider, false);
    addAndMakeVisible(tapeWearSlider);

    tapeMemoryLabel = new juce::Label("", "Memory");
    tapeMemorySlider.setSliderStyle(juce::Slider::Rotary);
    tapeMemorySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    tapeMemorySlider.setRange(0.0f, 0.95f, 0.1f);
    tapeMemoryLabel->setJustificationType(juce::Justification::centred);
    tapeMemoryLabel->attachToComponent(&tapeMemorySlider, false);
    addAndMakeVisible(tapeMemorySlider);

    dryWetLabel = new juce::Label("", "Dry/Wet");
    dryWetSlider.setSliderStyle(juce::Slider::Rotary);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    dryWetSlider.setRange(0.0f, 1.0f, 0.5f);
    dryWetLabel->setJustificationType(juce::Justification::centred);
    dryWetLabel->attachToComponent(&dryWetSlider, false);
    addAndMakeVisible(dryWetSlider);

    wowDepthLabel = new juce::Label("", "WowDepth");
    wowSlider.setSliderStyle(juce::Slider::Rotary);
    wowSlider.setRange(0.0f, 10.0f, 0.0f);
    wowSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    wowDepthLabel->setJustificationType(juce::Justification::centred);
    wowDepthLabel->attachToComponent(&wowSlider, false);
    addAndMakeVisible(wowSlider);

    wowSpeedLabel = new juce::Label("", "WowSpeed");
    wowSpeed.setSliderStyle(juce::Slider::Rotary);
    wowSpeed.setRange(0.1f, 4.0f, 0.0f);
    wowSpeed.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    wowSpeedLabel->setJustificationType(juce::Justification::centred);
    wowSpeedLabel->attachToComponent(&wowSpeed, false);
    addAndMakeVisible(wowSpeed);

    /*
    preGainLabel = new juce::Label("", "Gain");
    preGainSlider.setSliderStyle(juce::Slider::Rotary);
    preGainSlider.setRange(1.0f, 50.0f, 1.0f);
    preGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    preGainLabel->setJustificationType(juce::Justification::centred);
    preGainLabel->attachToComponent(&preGainSlider, false);
    addAndMakeVisible(preGainSlider);

    filerSaturLabel = new juce::Label("", "Filter");
    filterSaturSlider.setSliderStyle(juce::Slider::Rotary);
    filterSaturSlider.setRange(1500.0f, 10000.0f, 1500.0f);
    filterSaturSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 20);
    filerSaturLabel->setJustificationType(juce::Justification::centred);
    filerSaturLabel->attachToComponent(&filterSaturSlider, false);
    addAndMakeVisible(filterSaturSlider);
    */
    
    
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
    

    preAmpKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "preAmp", preAmpSlider);
    timeknobI = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "timeI", delaySliderI);
    timeknobD = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "timeD", delaySliderD);
    feedbackKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "feedback", feedbackSlider);
    tapeWearKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "tapeWear", tapeWearSlider);
    tapeWearKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "tapeMemory", tapeMemorySlider);
    dryWetKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "dryWet", dryWetSlider);
    wowKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "wow", wowSlider);
    speedKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "wowSpeed", wowSpeed);
    preGainKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "preGain", preGainSlider);
    filterSaturKnob = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.tree, "filterSatur", filterSaturSlider);
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

    g.drawRoundedRectangle(preAmpRectangle.toFloat(), 8.0f, 2.0f);
    g.drawFittedText("PreAmp", preAmpRectangle.reduced(4), juce::Justification::topLeft, 1);

    g.drawRoundedRectangle(delayRectangle.toFloat(), 8.0f, 2.0f);
    g.drawFittedText("Delay", delayRectangle.reduced(4), juce::Justification::topLeft, 1);

    g.drawRoundedRectangle(wowRectangle.toFloat(), 8.0f, 2.0f);
    g.drawFittedText("Wow", wowRectangle.reduced(4), juce::Justification::topLeft, 1);

    /*
    g.drawRoundedRectangle(saturRectangle.toFloat(), 8.0f, 2.0f);
    g.drawFittedText("Saturation", saturRectangle.reduced(4), juce::Justification::topLeft, 1);
    */

    g.drawRoundedRectangle(outputRectangle.toFloat(), 8.0f, 2.0f);
    g.drawFittedText("Output", outputRectangle.reduced(4), juce::Justification::topLeft, 1);
    
    //g.drawText("TimeI", 10, 5, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
    //g.drawText("TimeD", (getWidth() / 4) + 10, 5, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
    //g.drawText("Feedback", 2 * (getWidth() / 4) + 10, 5, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
    //g.drawText("DryWet", 3 * (getWidth() / 4) + 10, 5, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
   // g.drawText("Wow", 10, 160, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
   // g.drawText("WowSpeed", 10 + (getWidth() / 4), 160, (getWidth() / 4) - 10, 100, juce::Justification::centred, true);
}

void SimpleDelayTapeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
   // delaySliderI.setBounds(10, 10, (getWidth() / 4) - 10, 100);
   // delaySliderD.setBounds((getWidth() / 4) + 10, 10, (getWidth() / 4) - 10, 100);
   // feedbackSlider.setBounds(2 * (getWidth() / 4) + 10, 10, (getWidth() / 4) - 10, 100);
   // dryWetSlider.setBounds(3 * (getWidth() / 4) + 10, 10, (getWidth() / 4) - 10, 100);
   // divisionBoxI.setBounds(10, 120, (getWidth() / 4) - 10, 20);
   // divisionBoxD.setBounds((getWidth() / 4) + 10, 120, (getWidth() / 4) - 10, 20);
   // syncButton.setBounds(2*(getWidth() / 4) + 10, 110, (getWidth() / 4) - 10, 100);
   // pingPongButton.setBounds(3 * (getWidth() / 4) + 10, 110, (getWidth() / 4) - 10, 100);
   // wowSlider.setBounds(10, 150, (getWidth() / 4), 100);
   // wowSpeed.setBounds(10 + (getWidth() / 4), 150, (getWidth() / 4), 100);


  
        auto bounds = getLocalBounds().reduced(10);

        // ==============================
        // FLEXBOX PRINCIPAL HORIZONTAL
        // ==============================
        juce::FlexBox mainFlex;
        mainFlex.flexDirection = juce::FlexBox::Direction::row;
        mainFlex.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
        mainFlex.alignItems = juce::FlexBox::AlignItems::stretch;

        // Rectángulos donde se dibujan bordes
        preAmpRectangle = bounds.removeFromLeft(bounds.getWidth() / 8).reduced(5);
        delayRectangle = bounds.removeFromLeft(bounds.getWidth() / 2).reduced(5);
        wowRectangle = bounds.removeFromLeft(bounds.getWidth() / 2).reduced(5);
        outputRectangle = bounds.reduced(5);

        // === FLEXBOX PREAMP GROUP ===
        juce::FlexBox flexBoxPreAmp;
        flexBoxPreAmp.flexDirection = juce::FlexBox::Direction::row;
        flexBoxPreAmp.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
        flexBoxPreAmp.items.add(juce::FlexItem(preAmpSlider).withWidth(50).withHeight(80).withMinWidth(80).withMinHeight(80).withMaxHeight(100));
        flexBoxPreAmp.alignContent = juce::FlexBox::AlignContent::center;
        flexBoxPreAmp.alignItems = juce::FlexBox::AlignItems::center;
        flexBoxPreAmp.performLayout(preAmpRectangle);

        // === FLEXBOX DELAY GROUP ===
        juce::FlexBox flexBoxDelay;
        flexBoxDelay.flexDirection = juce::FlexBox::Direction::row;
        flexBoxDelay.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
        flexBoxDelay.items.add(juce::FlexItem(delaySliderI).withWidth(100).withHeight(80).withMinWidth(80).withMinHeight(80).withMaxHeight(100));
        flexBoxDelay.items.add(juce::FlexItem(delaySliderD).withWidth(100).withHeight(80).withMinWidth(80).withMinHeight(80).withMaxHeight(100));
        flexBoxDelay.items.add(juce::FlexItem(feedbackSlider).withWidth(100).withHeight(80).withMinWidth(80).withMinHeight(80).withMaxHeight(100));
        flexBoxDelay.items.add(juce::FlexItem(tapeWearSlider).withWidth(100).withHeight(80).withMinWidth(80).withMinHeight(80).withMaxHeight(100));
        flexBoxDelay.items.add(juce::FlexItem(tapeMemorySlider).withWidth(100).withHeight(80).withMinWidth(80).withMinHeight(80).withMaxHeight(100));
        flexBoxDelay.items.add(juce::FlexItem(dryWetSlider).withWidth(100).withHeight(80).withMinWidth(80).withMinHeight(80).withMaxHeight(100));
        flexBoxDelay.alignContent = juce::FlexBox::AlignContent::center;
        flexBoxDelay.alignItems = juce::FlexBox::AlignItems::center;
        flexBoxDelay.performLayout(delayRectangle);

        // === FLEXBOX WOW GROUP ===
        juce::FlexBox flexBoxWow;
        flexBoxWow.flexDirection = juce::FlexBox::Direction::row;
        flexBoxWow.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
        flexBoxWow.items.add(juce::FlexItem(wowSlider).withWidth(80).withHeight(80).withMinWidth(80).withMinHeight(60).withMaxHeight(100));
        flexBoxWow.items.add(juce::FlexItem(wowSpeed).withWidth(80).withHeight(80).withMinWidth(80).withMinHeight(60).withMaxHeight(100));
        flexBoxWow.alignContent = juce::FlexBox::AlignContent::center;
        flexBoxWow.alignItems = juce::FlexBox::AlignItems::center;
        flexBoxWow.performLayout(wowRectangle);

        /*
        // === FLEXBOX SATURATION GROUP ===
        juce::FlexBox flexBoxSatur;
        flexBoxSatur.flexDirection = juce::FlexBox::Direction::row;
        flexBoxSatur.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
        flexBoxSatur.items.add(juce::FlexItem(preGainSlider).withWidth(80).withHeight(80).withMinWidth(80).withMinHeight(60).withMaxHeight(100));
        flexBoxSatur.items.add(juce::FlexItem(filterSaturSlider).withWidth(80).withHeight(80).withMinWidth(80).withMinHeight(60).withMaxHeight(100));
        flexBoxSatur.alignContent = juce::FlexBox::AlignContent::center;
        flexBoxSatur.alignItems = juce::FlexBox::AlignItems::center;
        flexBoxSatur.performLayout(saturRectangle);
        */

        // === FLEXBOX OUTPUT GROUP ===
        juce::FlexBox flexBoxOutput;
        flexBoxOutput.flexDirection = juce::FlexBox::Direction::column;
        flexBoxOutput.justifyContent = juce::FlexBox::JustifyContent::center;
        flexBoxOutput.alignItems = juce::FlexBox::AlignItems::center;
        flexBoxOutput.items.add(juce::FlexItem(pingPongButton).withMinWidth(80).withMinHeight(30));
        flexBoxOutput.items.add(juce::FlexItem(syncButton).withMinWidth(80).withMinHeight(30));
        flexBoxOutput.items.add(juce::FlexItem(divisionBoxI).withWidth(100).withMinWidth(80).withMinHeight(30));
        flexBoxOutput.items.add(juce::FlexItem(divisionBoxD).withWidth(100).withMinWidth(80).withMinHeight(30));
        flexBoxOutput.performLayout(outputRectangle);
    

}
