/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TapeHysteresis.h"
#include "TapeMagProcessor.h"

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

    //============================================

    void fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData);
    void getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, int delaySamples, const float* bufferData, const float* delayBufferData);
    void feedbackDelay(int channel, const int bufferLength, const int delayBufferLength, const float* wetBuffer);
    float cubicLagrange(const float* buffer, int bufferLength, float readPos);
    void preAmp(juce::AudioBuffer<float>& buffer);
    float cassetteSat(float x);
    void saturation(juce::AudioBuffer<float>& buffer);
    float udoDistortion(float input);
    void updateParameters();
    void setProcessingBuffer(juce::AudioBuffer<float>& buffer, int totalNumInputChannels, int totalNumOutputChannels);

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
    float wear;
    float memory;
    float preampGainDb;
    
    TapeHysteresis tapeSat[2];

    // Wow/Flutter
    float wowPhase1 = 0.0f ;
    float wowPhase2 = 0.0f;
    float flutterPhase1 = 0.0f;
    float flutterPhase2 = 0.0f;

    // Parámetros configurables
    float wowDepthMs{ 5.0f };
    float wowRate{ 0.4f };   // Hz
    float wowDepth{ 5.0f };   // Muestras

    float flutterDepthMs{ 1.5f };
    float flutterRate = 10.0f;  // Hz
    float flutterDepth = 1.5f;  // Muestras

    float flutterNoise{ 0.0f }; //rudio filtrado

    //juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLineI;
    //juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLineD;

    juce::AudioBuffer<float> delayBuffer[2];
    int writePosition[2] { 0 };

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLines[2];

    juce::dsp::DryWetMixer<float> dryWetMixer;
    juce::SmoothedValue<float> smoothedMix;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedPreamp, smoothedFeedback, smoothedWear;
    juce::SmoothedValue<float> smoothedDelayTimeL, smoothedDelayTimeR, smoothedDelayTimePingPong, smoothWowDepth, SmoothFlutterDepth;
    juce::AudioPlayHead::PositionInfo info;
    juce::dsp::IIR::Filter<float> lowPassFilters[2];
    juce::dsp::IIR::Filter<float> highPassFilterNoise;
    float currentDelaySamples[2] = { 0.0f, 0.0f };    //para el cambio estilo tape del delaysamples


    //TAPE MAG

    int OSamount = 2;
    int BlockSize = 512;
    std::unique_ptr<TapeMagProcessor> TapeMag = std::make_unique<TapeMagProcessor>();
    std::vector<std::vector<float>> processingbuffer;

    //SATURATION
   
    juce::dsp::Gain<float> pregain;

    const float highPassCutoff = 70.0f;
    juce::dsp::IIR::Filter<float> highPassFilter;
    juce::dsp::IIR::Filter<float> lowPassFilter;
   
    juce::dsp::FIR::Coefficients<float> Coefficients;

    juce::dsp::ProcessorDuplicator<juce::dsp::FIR::Filter<float>, juce::dsp::FIR::Coefficients<float>> antiAliasingFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filterProcessor;

    //NOISE TAPE HISS

    float noiseTapeGain;
    float noiseTapeDecibels;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayTapeAudioProcessor)
};
