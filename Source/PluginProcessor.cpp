/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define LEVEL_ID "level"
#define LEVEL_NAME "Level"
#define BPM_ID "bpm"
#define BPM_NAME "Bpm"
#define TIME_ID_I "timeI"
#define TIME_NAME_I "TimeI"
#define TIME_ID_D "timeD"
#define TIME_NAME_D "TimeD"
#define FEEDBACK_ID "feedback"
#define FEEDBACK_NAME "Feedback"
#define DRYWET_ID "dryWet"
#define DRYWET_NAME "DryWet"
#define SYNC_ID "syncOn"
#define SYNC_NAME "Sync"
#define PING_PONG_ID "ping pong"
#define PING_PONG_NAME "Ping Pong"
#define DIVISION_ID_I "noteDivisionI"
#define DIVISION_NAME_I "DivisionI"
#define DIVISION_ID_D "noteDivisionD"
#define DIVISION_NAME_D "DivisionD"
#define WOW_ID "wow"
#define WOW_NAME "Wow"

//==============================================================================
SimpleDelayTapeAudioProcessor::SimpleDelayTapeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), tree(*this, nullptr, "params", { std::make_unique<juce::AudioParameterFloat>(TIME_ID_I, TIME_NAME_I, 0.1f, 2000.0f, 500.0f),
                                    std::make_unique<juce::AudioParameterFloat>(TIME_ID_D, TIME_NAME_D, 0.1f, 2000.0f, 500.0f),
                                 std::make_unique<juce::AudioParameterFloat>(FEEDBACK_ID, FEEDBACK_NAME, 0.0f, 0.7f, 0.1f),
                                 std::make_unique<juce::AudioParameterFloat>(LEVEL_ID, LEVEL_NAME, 0.0f, 1.0f, 0.8f),
                                 std::make_unique<juce::AudioParameterFloat>(DRYWET_ID, DRYWET_NAME, 0.0f, 1.0f, 0.5f),
                                std::make_unique<juce::AudioParameterFloat>(WOW_ID, WOW_NAME, 0.0f, 10.0f, 0.0f),
                                std::make_unique<juce::AudioParameterBool>(SYNC_ID, SYNC_NAME, false),
                                std::make_unique<juce::AudioParameterBool>(PING_PONG_ID, PING_PONG_NAME, false),
                                std::make_unique<juce::AudioParameterChoice>(DIVISION_ID_I, DIVISION_NAME_I,
                                    juce::StringArray{  "1/4", "1/4.", "1/4T","1/8", "1/8.", "1/8T", "1/16" }, 2),
                                std::make_unique<juce::AudioParameterChoice>(DIVISION_ID_D, DIVISION_NAME_D,
                                                            juce::StringArray{  "1/4", "1/4.", "1/4T","1/8", "1/8.", "1/8T", "1/16" }, 2) })
#endif
{
}

SimpleDelayTapeAudioProcessor::~SimpleDelayTapeAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleDelayTapeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleDelayTapeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayTapeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayTapeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleDelayTapeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleDelayTapeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleDelayTapeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleDelayTapeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleDelayTapeAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleDelayTapeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleDelayTapeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{

    localSampleRate = sampleRate;

    const int maxDelayTimeMs = 2000; // 2 segundos
    const int maxDelaySamples = (int)(maxDelayTimeMs * localSampleRate / 1000.0);


    for (int i = 0; i < 2; ++i)
    {
        delayLines[i].reset();
        delayLines[i].setMaximumDelayInSamples(maxDelaySamples);
        delayLines[i].prepare({ localSampleRate, (juce::uint32)samplesPerBlock, 1 });

        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(localSampleRate, 5000.0f);
        *lowPassFilters[i].coefficients = *coeffs;
    }

    dryWetMixer.setMixingRule(juce::dsp::DryWetMixingRule::sin3dB); // o sinCos para más natural
    dryWetMixer.setWetLatency(0); // importante si el delay genera latencia (este no lo hace)
    dryWetMixer.prepare({ localSampleRate, (juce::uint32)samplesPerBlock, (juce::uint32)getTotalNumOutputChannels() });

    smoothedDelayTimeL.reset(localSampleRate, 0.05);
    smoothedDelayTimeR.reset(localSampleRate, 0.05);
}

void SimpleDelayTapeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleDelayTapeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

float SimpleDelayTapeAudioProcessor::getDelayTimeMS(bool syncEnabled, int divisionIndex, juce::StringRef channel)
{
    double bpm = 120.0;
    float delayTimeMs;

     // Obtener BPM del host
    if (auto* playHead = getPlayHead())
    {

        if (playHead->getPosition())
            if (info.getBpm() > 0.0)
            {
                if (auto bpmOpt = info.getBpm(); bpmOpt.hasValue())
                    bpm = *bpmOpt;
            }
    }

    if (syncEnabled)
    {
        float multiplier = 1.0f;
        switch (divisionIndex)
        {
        case 1: multiplier = 1.0f;          break; // 1/4
        case 2: multiplier = 1.5f;          break; // 1/4.
        case 3: multiplier = 2.0f / 3.0f;   break; // 1/4T
        case 4: multiplier = 0.5f;          break; // 1/8
        case 5: multiplier = 0.75f;         break; // 1/8.
        case 6: multiplier = 1.0f / 3.0f;   break; // 1/8T
        case 7: multiplier = 0.25f;         break; // 1/16
        default: multiplier = 1.0f;         break;
        }

        delayTimeMs = static_cast<float>((60.0 / bpm) * 1000.0 * multiplier);
    }
    else {
        delayTimeMs = *tree.getRawParameterValue(channel);
    }

    return delayTimeMs;
}

void SimpleDelayTapeAudioProcessor::processPingPongDelay(juce::AudioBuffer<float>& buffer, const float delayTimeMs)
{
    const int numSamples = buffer.getNumSamples();

    const float mix = *tree.getRawParameterValue(DRYWET_ID);

    smoothedDelayTimePingPong.setTargetValue(delayTimeMs);
    dryWetMixer.setWetMixProportion(mix);
    dryWetMixer.pushDrySamples(buffer);

    auto* left = buffer.getWritePointer(0);
    auto* right = buffer.getWritePointer(1);

    for (int i = 0; i < numSamples; ++i)
    {
        float inL = left[i];
        float inR = right[i];

        float delaySamples = smoothedDelayTimePingPong.getNextValue() * getSampleRate() / 1000.0f;

        float delaySamplesWow = applyWowFlutter(delaySamples, localSampleRate);

        float delayedL = lowPassFilters[0].processSample(delayLines[0].popSample(0, delaySamplesWow));
        float delayedR = lowPassFilters[1].processSample(delayLines[1].popSample(0, delaySamplesWow));

       // float feedbackL = inL + delayedR * feedback;
       // float feedbackR = inR + delayedL * feedback;

        delayLines[0].pushSample(0, inL + delayedR * feedback);
        delayLines[1].pushSample(0, delayedL * feedback);

        left[i] = delayedL;
        right[i] = delayedR;
    }

    dryWetMixer.mixWetSamples(buffer);
}

void SimpleDelayTapeAudioProcessor::checkMono(juce::AudioBuffer<float>& buffer)
{
    // Si la entrada es mono (1 canal) y la salida es estéreo (2 canales)
    if (getNumInputChannels() == 1 && getNumOutputChannels() == 2 && buffer.getNumChannels() == 2)
    {
        auto* inputMono = buffer.getReadPointer(0);
        auto* rightOut = buffer.getWritePointer(1);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
            rightOut[i] = inputMono[i]; // Copia canal izquierdo a canal derecho
    }
}
void SimpleDelayTapeAudioProcessor::processStereoDelay(juce::AudioBuffer<float>& buffer, int numSamples, int numChannels, float delayTimeMsI, float delayTimeMsD)
{
    const float dryWet = *tree.getRawParameterValue(DRYWET_ID);
    dryWetMixer.setWetMixProportion(dryWet);
    dryWetMixer.pushDrySamples(buffer);

    smoothedDelayTimeL.setTargetValue(delayTimeMsI);
    smoothedDelayTimeR.setTargetValue(delayTimeMsD);

    for (int channel = 0; channel < numChannels; ++channel)
    {

        float* channelData = buffer.getWritePointer(channel);


        for (int i = 0; i < numSamples; ++i)
        {

            float in = channelData[i];
            float delayMs = (channel == 0 ? smoothedDelayTimeL.getNextValue()
                : smoothedDelayTimeR.getNextValue());
            float delaySamples = delayMs * getSampleRate() / 1000.0f;

            //Modulamos delaySamples
            float modulatedDelaysamples = applyWowFlutter(delaySamples, getSampleRate());
            // Extraer muestra del delay interpolado
            float delayed = delayLines[channel].popSample(0, modulatedDelaysamples);

            // Aplicar filtro low-pass al delayed
            delayed = lowPassFilters[channel].processSample(delayed);

            float signalToDelay = in + delayed * feedback;
            // Mezclar wet + dry (simple ejemplo 50/50)
          // float wet = 0.5f * delayed;
           //float dry = 0.5f * in;
            channelData[i] = delayed;

            // Insertar muestra en delay line
            delayLines[channel].pushSample(0, signalToDelay);
        }
    }

    dryWetMixer.mixWetSamples(buffer);
}
float SimpleDelayTapeAudioProcessor::applyWowFlutter(float baseDealySamples, double sampleRate)
{
    constexpr float twoPi = juce::MathConstants<float>::twoPi;
    wowDepth = *tree.getRawParameterValue(WOW_ID);

    //En muestras

    float wowStep = twoPi * wowRate / (float)sampleRate;
    float flutterStep = twoPi * flutterRate / (float)sampleRate;

    //LFOs

    float wowMod = std::sin(wowPhase) * wowDepth;
    float flutterMod = std::sin(flutterPhase) * flutterDepth;

    //Avance de fase

    wowPhase += wowStep;
    flutterPhase += flutterStep;

    //Limitar
    if (wowPhase > twoPi)  wowPhase -= twoPi;
    if (flutterPhase > twoPi) flutterPhase -= twoPi;

    return baseDealySamples + wowMod;
}
void SimpleDelayTapeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    

    
    
    feedback = *tree.getRawParameterValue(FEEDBACK_ID);
    syncEnabled = *tree.getRawParameterValue(SYNC_ID);
    divisionIndexI = *tree.getRawParameterValue(DIVISION_ID_I);
    divisionIndexD = *tree.getRawParameterValue(DIVISION_ID_D);
    dryWet = *tree.getRawParameterValue(DRYWET_ID);
    delayTimeMsI = getDelayTimeMS(syncEnabled, divisionIndexI, TIME_ID_I);
    delayTimeMsD = getDelayTimeMS(syncEnabled, divisionIndexD, TIME_ID_D);
    pingPongMode = *tree.getRawParameterValue(PING_PONG_ID);

    if (pingPongMode && buffer.getNumChannels() >= 2)
    {
        processPingPongDelay(buffer, delayTimeMsI);
        return; // salta el delay normal
    }
    else {
        processStereoDelay(buffer, numSamples, numChannels, delayTimeMsI, delayTimeMsD);
    }
     
  
}

//==============================================================================
bool SimpleDelayTapeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleDelayTapeAudioProcessor::createEditor()
{
    return new SimpleDelayTapeAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleDelayTapeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleDelayTapeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleDelayTapeAudioProcessor();
}
