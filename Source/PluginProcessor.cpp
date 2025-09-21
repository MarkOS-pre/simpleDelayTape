/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "TapeHysteresis.h"


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
#define WOW_SPEED_ID "wowSpeed"
#define WOW_SPEED_NAME "WowSpeed"
#define FLUTTER_DEPTH_ID "flutterDepth"
#define FLUTTER_DEPTH_NAME "FlutterDepth"
#define PRE_GAIN_ID "preGain"
#define PRE_GAIN_NAME "PreGain"
#define FILTER_SATUR_ID "filterSatur"
#define FILTER_SATUR_NAME "FilterSatur"
#define PREAMP_ID "preAmp"
#define PREAMP_NAME "PreAmp"
#define TAPEWEAR_ID "tapeWear"
#define TAPEWEAR_NAME "TapeWear"
#define TAPEMEMORY_ID "tapeMemory"
#define TAPEMEMORY_NAME "TapeMemory"
#define NOISETAPE_ID "noiseTapeGain"
#define NOISETAPE_NAME "NoiseTapeGain"


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
    ), tree(*this, nullptr, "params", { std::make_unique<juce::AudioParameterFloat>(PREAMP_ID, PREAMP_NAME, -30.0f, 3.0f, -3.0f),
                                    std::make_unique<juce::AudioParameterFloat>(TIME_ID_I, TIME_NAME_I, 0.1f, 2000.0f, 500.0f),
                                    std::make_unique<juce::AudioParameterFloat>(TIME_ID_D, TIME_NAME_D, 0.1f, 2000.0f, 500.0f),
                                 std::make_unique<juce::AudioParameterFloat>(FEEDBACK_ID, FEEDBACK_NAME, 0.0f, 0.7f, 0.1f),
                                 std::make_unique<juce::AudioParameterFloat>(TAPEWEAR_ID, TAPEWEAR_NAME, 0.5f, 5.0f, 0.1f),
                                 std::make_unique<juce::AudioParameterFloat>(TAPEMEMORY_ID, TAPEMEMORY_NAME, 0.1f, 0.95f, 0.1f),
                                 std::make_unique<juce::AudioParameterFloat>(LEVEL_ID, LEVEL_NAME, 0.0f, 1.0f, 0.8f),
                                 std::make_unique<juce::AudioParameterFloat>(DRYWET_ID, DRYWET_NAME, 0.0f, 1.0f, 0.5f),
                                std::make_unique<juce::AudioParameterFloat>(WOW_ID, WOW_NAME, 0.0f, 10.0f, 0.1f),
                                std::make_unique<juce::AudioParameterFloat>(WOW_SPEED_ID, WOW_SPEED_NAME, 0.1f, 4.0f, 1.5f),
                                std::make_unique<juce::AudioParameterFloat>(FLUTTER_DEPTH_ID, FLUTTER_DEPTH_NAME, 0.0f, 5.0f, 0.1f),
                                std::make_unique<juce::AudioParameterFloat>(PRE_GAIN_ID, PRE_GAIN_NAME, 1.0f, 50.0f, 1.0f),
                                std::make_unique<juce::AudioParameterFloat>(NOISETAPE_ID, NOISETAPE_NAME, -60.0f, -30.0f, -50.0f),
                                std::make_unique<juce::AudioParameterFloat>(FILTER_SATUR_ID, FILTER_SATUR_NAME, 1500.0f, 10000.0f, 1500.0f),
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
    maxDelaySamples = (int)(maxDelayTimeMs * localSampleRate / 1000.0);


    for (int i = 0; i < 2; ++i)
    {
        
        delayBuffer[i].setSize(1, maxDelaySamples + samplesPerBlock);
        delayBuffer[i].clear();
        writePosition[i] = 0;

        delayLines[i].reset();
        delayLines[i].setMaximumDelayInSamples(maxDelaySamples);
        delayLines[i].prepare({ localSampleRate, (juce::uint32)samplesPerBlock, 1 });

        tapeSat[i].reset();

        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(localSampleRate, 5000.0f);
        *lowPassFilters[i].coefficients = *coeffs;

        
    }

    auto coeffsHigh = juce::dsp::IIR::Coefficients<float>::makeHighPass(localSampleRate, 3000.0f);
    *highPassFilterNoise.coefficients = *coeffsHigh;

    dryWetMixer.setMixingRule(juce::dsp::DryWetMixingRule::sin3dB); // o sinCos para más natural
    dryWetMixer.setWetLatency(0); // importante si el delay genera latencia (este no lo hace)
    dryWetMixer.prepare({ localSampleRate, (juce::uint32)samplesPerBlock, (juce::uint32)getTotalNumOutputChannels() });

    smoothedFeedback.reset(localSampleRate, 0.05);
    smoothedPreamp.reset(localSampleRate, 0.1);
    smoothedPreamp.setCurrentAndTargetValue(1.0f);
    smoothedDelayTimeL.reset(localSampleRate, 0.05);
    smoothedDelayTimeR.reset(localSampleRate, 0.05);
    smoothedWear.reset(localSampleRate, 0.1);
    SmoothFlutterDepth.reset(localSampleRate, 0.05);
    smoothWowDepth.reset(localSampleRate, 0.05);
    TapeMag.reset(new TapeMagProcessor());

    // Reset DSP objects
    TapeMag->Reset(sampleRate, getTotalNumOutputChannels(), OSamount);

    //SATURATION
    /*
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = localSampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    pregain.reset();
    pregain.prepare(spec);
    pregain.setRampDurationSeconds(0.02f);
    pregain.setGainLinear(tree.getRawParameterValue(PRE_GAIN_ID)->load());

    highPassFilter.reset();
    highPassFilter.prepare(spec);
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(localSampleRate, highPassCutoff);
    highPassFilter.coefficients = coeffs;
    lowPassFilter.reset();
    lowPassFilter.prepare(spec);
    

    antiAliasingFilter.reset();
    antiAliasingFilter.prepare(spec);
    */
    
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

        float targetDelaySamples = smoothedDelayTimePingPong.getNextValue() * getSampleRate() / 1000.0f;

        // tape glide: interpolamos suavemente hacia el target
        float glideSpeed = 0.001f; // cuanto más pequeño, más lento el “pitch bend”
        currentDelaySamples[0] += glideSpeed * (targetDelaySamples - currentDelaySamples[0]);

        float delaySamplesWow1 = applyWowFlutter(currentDelaySamples[0], localSampleRate, 0);
        float delaySamplesWow2 = applyWowFlutter(currentDelaySamples[0], localSampleRate, 1);

        float delayedL = lowPassFilters[0].processSample(delayLines[0].popSample(0, delaySamplesWow1));
        float delayedR = lowPassFilters[1].processSample(delayLines[1].popSample(0, delaySamplesWow2));

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

    

    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLengthI = delayBuffer[0].getNumSamples();
    const int delayBufferLengthD = delayBuffer[1].getNumSamples();
    

    for (int channel = 0; channel < numChannels; ++channel)
    {
        /*
        // VERSION SIN DSP
        
        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = delayBuffer[channel].getReadPointer(0);
        //const float* wetBufferData = buffer.getReadPointer(channel);
        float delayMs = (channel == 0 ? smoothedDelayTimeL.getNextValue()
            : smoothedDelayTimeR.getNextValue());
        float delayBufferLength = (channel == 0 ? delayBufferLengthI : delayBufferLengthD);
        float delaySamples = delayMs * getSampleRate() / 1000.0f;

        //Modulamos delaySamples
        float modulatedDelaysamples = applyWowFlutter(delaySamples, getSampleRate(), channel);
        modulatedDelaysamples = juce::jlimit(1.0f, maxDelaySamples - 1.0f, modulatedDelaysamples);

        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData);
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, delaySamples, bufferData, delayBufferData);
        feedbackDelay(channel, bufferLength, delayBufferLength, bufferData);
        */
        //VERSION MUESTRA A MUESTRA CON LAGRANGE
        float* channelData = buffer.getWritePointer(channel);
        float* delayData = delayBuffer[channel].getWritePointer(0);   //Esto de tenerlo a 0, no lo entiendo
        const int delayBufferLength = delayBuffer[channel].getNumSamples();

        for (int n = 0; n < numSamples; ++n)
        {
            // delay time actual (suavizado muestra a muestra)
            float targetDelayMs = (channel == 0 ? smoothedDelayTimeL.getNextValue()
                : smoothedDelayTimeR.getNextValue());       //el tiempo que pide el potenciometro

            float targetDelaySamples = targetDelayMs * getSampleRate() / 1000.0f;
            targetDelaySamples = applyWowFlutter(targetDelaySamples, getSampleRate(), channel);
            targetDelaySamples = juce::jlimit(1.0f, (float)delayBufferLength - 4.0f, targetDelaySamples);

            // tape glide: interpolamos suavemente hacia el target
            float glideSpeed = 0.001f; // cuanto más pequeño, más lento el “pitch bend”
            currentDelaySamples[channel] += glideSpeed * (targetDelaySamples - currentDelaySamples[channel]);

            // posición fraccional de lectura
            float readPos = (float)writePosition[channel] - currentDelaySamples[channel];
            if (readPos < 0)
                readPos += (float)delayBufferLength;

            // interpolación cúbica
            float delayedSample = cubicLagrange(delayData, delayBufferLength, readPos);

            // low-pass para suavizar las repeticiones
            delayedSample = lowPassFilters[channel].processSample(delayedSample);

            // 🎛️ aplicar saturación SOLO a la señal retardada
           // delayedSample = udoDistortion(pregain.processSample( delayedSample));

            // guardar entrada antes de sobrescribir
            float in = channelData[n];

            // salida wet (el dry se mezcla luego con dryWetMixer)
            channelData[n] = delayedSample;

           // float fbIn = delayedSample * smoothedFeedback.getNextValue() * smoothedWear.getNextValue();;
            float fbIn = delayedSample * smoothedFeedback.getNextValue();
           // fbIn = cassetteSat(fbIn);
            tapeSat[channel].setDrive(wear);
            tapeSat[channel].setMemory(memory);
            fbIn = tapeSat[channel].process(fbIn);
            // compensación simple de volumen según wear
            float compensation = 1.0f / juce::jmax(0.3f, wear * 1.2f);
            fbIn *= compensation;
            
            //hiss o ruido de cinta

            float noiseTape  = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // rango [-1, 1]

            noiseTapeGain = juce::Decibels::decibelsToGain(noiseTapeDecibels);

            noiseTape *= noiseTapeGain;

            noiseTape = highPassFilterNoise.processSample(noiseTape);
            
            // escribir en delay buffer: entrada + feedback * salida
            delayData[writePosition[channel]] = in + fbIn + noiseTape;

            // avanzar write pointer
            if (++writePosition[channel] >= delayBufferLength)
                writePosition[channel] = 0;
        }

        ///VERSION CON DSP
        /*
        float* channelData = buffer.getWritePointer(channel);


        for (int i = 0; i < numSamples; ++i)
        {

            float in = channelData[i];
            float delayMs = (channel == 0 ? smoothedDelayTimeL.getNextValue()
                : smoothedDelayTimeR.getNextValue());
            float delaySamples = delayMs * getSampleRate() / 1000.0f;

            //Modulamos delaySamples
            float modulatedDelaysamples = applyWowFlutter(delaySamples, getSampleRate(), channel);
            modulatedDelaysamples = juce::jlimit(1.0f, maxDelaySamples - 1.0f, modulatedDelaysamples);

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
        */
    }

   // saturation(buffer);
    dryWetMixer.mixWetSamples(buffer);
    /*
    writePosition[0] += bufferLength;
    writePosition[1] += bufferLength;
    writePosition[0] %= delayBufferLengthI;   //calculamos el resto para evitar que el nWritter se pase del tamaño del bufferDelay
    writePosition[1] %= delayBufferLengthD;
    */
}

inline float SimpleDelayTapeAudioProcessor::cubicLagrange(const float* buffer, int bufferLength, float readPos)
{
    int index = (int)readPos;
    float frac = readPos - (float)index;

    // índices vecinos con wrap-around
    int i0 = (index - 1 + bufferLength) % bufferLength;
    int i1 = (index + 0) % bufferLength;
    int i2 = (index + 1) % bufferLength;
    int i3 = (index + 2) % bufferLength;

    float xm1 = buffer[i0];
    float x0 = buffer[i1];
    float x1 = buffer[i2];
    float x2 = buffer[i3];

    // polinomio de Lagrange (3er orden)
    return x0 + 0.5f * frac * (x1 - xm1 + frac * (2.0f * xm1 - 5.0f * x0 + 4.0f * x1 - x2
        + frac * (3.0f * (x0 - x1) + x2 - xm1)));
}

//Distorsion pre amp
inline float softClipTanh(float x)
{
    return std::tanh(x);
}
void SimpleDelayTapeAudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData)
{

    //float levelDelay = *tree.getRawParameterValue(FEEDBACK_ID);
    //Copiar los datos del main buffer al delay buffer
    if (delayBufferLength >= bufferLength + writePosition[channel])
    {
        delayBuffer[channel].copyFromWithRamp(0, writePosition[channel], bufferData, bufferLength, 1, 1);

    }
    else {
        const int bufferRemaining = delayBufferLength - writePosition[channel];

        delayBuffer[channel].copyFromWithRamp(0, writePosition[channel], bufferData, bufferRemaining, 1, 1);
        delayBuffer[channel].copyFromWithRamp(0, 0, bufferData + bufferRemaining, bufferLength - bufferRemaining, 1, 1);
    }


}
void SimpleDelayTapeAudioProcessor::getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, int delaySamples, const float* bufferData, const float* delayBufferData)
{
   
    const int readPosition = static_cast<int> (delayBufferLength + writePosition[channel] - delaySamples) % delayBufferLength;

    if (delayBufferLength >= bufferLength + readPosition)
    {
        buffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferLength);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - readPosition;
        buffer.copyFrom(channel, 0, delayBufferData + readPosition, bufferRemaining);
        buffer.copyFrom(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining);
    }
}
void SimpleDelayTapeAudioProcessor::feedbackDelay(int channel, const int bufferLength, const int delayBufferLength, const float* wetBuffer)
{
    float feedback = juce::jlimit(0.0f, 0.7f, static_cast<float>(*tree.getRawParameterValue(FEEDBACK_ID)));;

    if (delayBufferLength > bufferLength + writePosition[channel])
    {
        delayBuffer[channel].addFromWithRamp(0, writePosition[channel], wetBuffer, bufferLength, feedback, feedback);
    }
    else {
        const int bufferReamining = delayBufferLength - writePosition[channel];
        delayBuffer[channel].addFromWithRamp(0, bufferReamining, wetBuffer, bufferReamining, feedback, feedback);
        delayBuffer[channel].addFromWithRamp(0, 0, wetBuffer, bufferLength - bufferReamining, feedback, feedback);
    }
}
float SimpleDelayTapeAudioProcessor::applyWowFlutter(float baseDealySamples, double sampleRate, int channel)
{
     constexpr float twoPi = juce::MathConstants<float>::twoPi;
     wowDepth = (smoothWowDepth.getNextValue() * sampleRate) / 1000.0f;
     flutterDepth = (SmoothFlutterDepth.getNextValue() * sampleRate) / 1000.0f;
     float wowMod;
     float flutterMod;

    //En muestras

    float wowStep = twoPi * wowRate / (float)sampleRate;
    float flutterStep = twoPi * flutterRate / (float)sampleRate;

    //LFOs
    if (channel == 0) 
    {
         wowMod = std::sin(wowPhase1) * wowDepth;
        wowPhase1 += wowStep;

        flutterMod = std::sin(flutterPhase1) * flutterDepth;
        flutterPhase1 += flutterStep;
    }
    else {
         wowMod = std::sin(wowPhase2) * wowDepth;
        wowPhase2 += wowStep;

        flutterMod = std::sin(flutterPhase2) * flutterDepth;
        flutterPhase2 += flutterStep;
    }
    

    //ruido blanco suavizado para irregularidad

    float rnd = (float)rand() / (float)RAND_MAX - 0.5f;

    //filtro paso bajo primer orden para suvizar ruido

    /*
    * 𝛼=0.98 α=0.98 → filtro muy lento, retiene mucho la historia.
        El resultado es ruido correlado de baja frecuencia (a veces se llama “brown noise” o “random walk” en DSP simplificado).
    * El wow & flutter real no es un seno perfecto: los motores y guías de cinta generan fluctuaciones aleatorias.
    */

    flutterNoise = 0.98f * flutterNoise + 0.02f * rnd;


    float modulation = wowMod + flutterMod + flutterNoise * 0.5f;

    //Limitar
    if (wowPhase1 > twoPi)  wowPhase1 -= twoPi;
    if (wowPhase2 > twoPi)  wowPhase2 -= twoPi;
    if (flutterPhase1 > twoPi) flutterPhase1 -= twoPi;
    if (flutterPhase2 > twoPi) flutterPhase2 -= twoPi;

    return baseDealySamples + modulation;
}
void SimpleDelayTapeAudioProcessor::saturation(juce::AudioBuffer<float>& buffer)
{
    float frequencyFilter = tree.getRawParameterValue(FILTER_SATUR_ID)->load();
    lowPassFilter.coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(localSampleRate, frequencyFilter, 0.1f);
    juce::dsp::AudioBlock<float> block{ buffer };

    highPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    antiAliasingFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    pregain.process(juce::dsp::ProcessContextReplacing<float>(block));

    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < block.getNumSamples(); ++sample) {
            float input = channelData[sample];
            channelData[sample] = udoDistortion(input);
        }
    }

    //volume.process(juce::dsp::ProcessContextReplacing<float>(block));
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
}

float SimpleDelayTapeAudioProcessor::udoDistortion(float input)
{
    float output = 0.0f;
    float absInput = std::fabs(input);
    float signInput = (input >= 0) ? 1.0f : -1.0f;
    float threshold = 1.0f / 3.0f;

    if (absInput < threshold) {
        output = 2.0f * input;
    }
    else if (absInput >= threshold && absInput < (2.0f * threshold)) {
        float base = 2.0f - 3.0f * absInput;
        output = signInput * (3.0f - (base * base)) / 3.0f;
    }
    else {
        output = signInput;
    }

    return output;
}


void SimpleDelayTapeAudioProcessor::preAmp(juce::AudioBuffer<float>& buffer)
{
    
    for (int ch = 0; ch < getTotalNumInputChannels() ; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            // aplicar ganancia de entrada
            float in = channelData[n] * juce::Decibels::decibelsToGain(smoothedPreamp.getNextValue());

            // aplicar soft clipping
            in = std::tanh(in);

            // escribir de nuevo en el buffer
            channelData[n] = in;
        }
    }
}

 float SimpleDelayTapeAudioProcessor::cassetteSat(float x)
{
    // más compresivo, con limitación
    return std::tanh(2.5f * x);
}

 void SimpleDelayTapeAudioProcessor::updateParameters()
 {
     feedback = *tree.getRawParameterValue(FEEDBACK_ID);
     syncEnabled = *tree.getRawParameterValue(SYNC_ID);
     divisionIndexI = *tree.getRawParameterValue(DIVISION_ID_I);
     divisionIndexD = *tree.getRawParameterValue(DIVISION_ID_D);
     dryWet = *tree.getRawParameterValue(DRYWET_ID);
     delayTimeMsI = getDelayTimeMS(syncEnabled, divisionIndexI, TIME_ID_I);
     delayTimeMsD = getDelayTimeMS(syncEnabled, divisionIndexD, TIME_ID_D);
     pingPongMode = *tree.getRawParameterValue(PING_PONG_ID);
     wear = *tree.getRawParameterValue(TAPEWEAR_ID);
     memory = *tree.getRawParameterValue(TAPEMEMORY_ID);
     preampGainDb = *tree.getRawParameterValue(PREAMP_ID);
     wowDepthMs = *tree.getRawParameterValue(WOW_ID);
     wowRate = *tree.getRawParameterValue(WOW_SPEED_ID);
     flutterDepthMs = *tree.getRawParameterValue(FLUTTER_DEPTH_ID);
     noiseTapeDecibels = *tree.getRawParameterValue(NOISETAPE_ID);
     

     smoothedDelayTimeL.setTargetValue(delayTimeMsI);
     smoothedDelayTimeR.setTargetValue(delayTimeMsD);
     smoothedFeedback.setTargetValue(feedback);
     smoothedWear.setTargetValue(wear);
     smoothedPreamp.setTargetValue(preampGainDb);
     smoothWowDepth.setTargetValue(wowDepthMs);
     SmoothFlutterDepth.setTargetValue(flutterDepthMs);
 }
 void SimpleDelayTapeAudioProcessor::setProcessingBuffer(juce::AudioBuffer<float>& buffer, int totalNumInputChannels, int totalNumOutputChannels)
 {
     // Get write pointer to channels
     auto bufferpointer = buffer.getArrayOfWritePointers();


     // Initialise processing buffer, applying buffer to vectorvectorfloat array allows easier duplication if parallel processing is required
     std::vector<std::vector<float>> processingbuffer;

     // Resize processing buffer
     processingbuffer.resize(totalNumOutputChannels);
     for (int i = 0; i < totalNumOutputChannels; ++i)
     {
         processingbuffer[i].resize(buffer.getNumSamples());

     }

     // Apply audio to processing buffer
     for (int channel = 0; channel < totalNumOutputChannels; channel++)
     {
         for (int sample = 0; sample < buffer.getNumSamples(); sample++)
         {
             // if mono stereo apply only input channel to both output channels
             if (totalNumInputChannels == 1)
                 processingbuffer[channel][sample] = bufferpointer[0][sample];
             else
                 processingbuffer[channel][sample] = bufferpointer[channel][sample];
         }
     }
 }
void SimpleDelayTapeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    
    updateParameters();

    //ETAPA PREAMPLIFICADOR
    preAmp(buffer);

    //ETAPA DE MAGNETIZACION
    setProcessingBuffer(buffer, getNumInputChannels(), getNumOutputChannels());
    TapeMag->ProcessBuffer(processingbuffer, BlockSize);

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
