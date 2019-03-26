/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

float currentSampleRate = 0.0f;
float currentAngle      = 0.0f;
float angleDelta        = 0.0f;
float sinFreq           = 0.0f;

//==============================================================================
SecondApAudioProcessor::SecondApAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SecondApAudioProcessor::~SecondApAudioProcessor()
{
}

//==============================================================================
const String SecondApAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SecondApAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SecondApAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SecondApAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SecondApAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SecondApAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SecondApAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SecondApAudioProcessor::setCurrentProgram (int index)
{
}

const String SecondApAudioProcessor::getProgramName (int index)
{
    return {};
}

void SecondApAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SecondApAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    currentSampleRate = sampleRate;
    sinFreq = 12.0f;
    updateAngleDelta();
    
    String message;
    message <<"Prepring to play " << newLine;
    message <<"Sample Rate is:" << currentSampleRate << newLine;
    message <<"Sine wave frequency is" << sinFreq << 'Hz' << newLine;
    message <<"Audio buffer size"<< samplesPerBlock << newLine;
}

void SecondApAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SecondApAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void SecondApAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    AudioBuffer<float>wetBuffer(totalNumInputChannels, buffer.getNumSamples());
    wetBuffer.makeCopyOf(buffer);
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto* wetData     = wetBuffer.getWritePointer (channel);
        // ..do something to the data...
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            auto currentSinSample = (float) std::sin (currentAngle);
            currentAngle += angleDelta;
            
            
            wetData[sample] = wetData[sample] * currentSinSample;
            
            channelData[sample] = wetData[sample]  * 0.8f + channelData[sample]  * 0.2f;
        }
    }
}

//==============================================================================
bool SecondApAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SecondApAudioProcessor::createEditor()
{
    return new SecondApAudioProcessorEditor (*this);
}

//==============================================================================
void SecondApAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SecondApAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void SecondApAudioProcessor::updateAngleDelta()
{
    auto cyclesPerSample = sinFreq / currentSampleRate;
    
    angleDelta = cyclesPerSample * MathConstants <float>::twoPi;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SecondApAudioProcessor();
}
