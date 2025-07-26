/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SimpleDelayTapeAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleDelayTapeAudioProcessor();
    ~SimpleDelayTapeAudioProcessor() override;

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

    float getDelayTimeMS(bool syncEnabled, int divisionIndex, juce::StringRef channel);
    void processPingPongDelay(juce::AudioBuffer<float>& buffer, const float delayTimeMs);
    void checkMono(juce::AudioBuffer<float>& buffer);
    void processStereoDelay(juce::AudioBuffer<float>& buffer, int numSamples, int numChannels, float delayTimeMsI, float delayTimeMsD);
    float applyWowFlutter(float baseDealySamples, double sampleRate, int channel);

    juce::AudioProcessorValueTreeState tree;
private:
    float localSampleRate{ 44100 };
    float delayTimeMsI;
    float delayTimeMsD;
    float feedback;
    float dryWet;
    bool syncEnabled;
    int divisionIndexI;
    int divisionIndexD;
    bool pingPongMode;
    int maxDelaySamples;

    // Wow/Flutter
    float wowPhase1 = 0.0f ;
    float wowPhase2 = 0.0f;
    float flutterPhase = 0.0f;

    // Parámetros configurables
    float wowRate{ 1.5f };   // Hz
    float wowDepth{ 0.0f };   // Muestras

    float flutterRate = 6.0f;  // Hz
    float flutterDepth = 3.0f;  // Muestras

    //juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLineI;
    //juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLineD;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLines[2];

    juce::dsp::DryWetMixer<float> dryWetMixer;
    juce::SmoothedValue<float> smoothedMix;
    juce::SmoothedValue<float> smoothedDelayTimeL, smoothedDelayTimeR, smoothedDelayTimePingPong, smoothWowDepth;
    juce::AudioPlayHead::PositionInfo info;
    juce::dsp::IIR::Filter<float> lowPassFilters[2];

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayTapeAudioProcessor)
};
