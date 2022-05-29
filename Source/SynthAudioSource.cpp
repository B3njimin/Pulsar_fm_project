/*
  ==============================================================================

    SynthAudioSource.cpp
    Created: 13 May 2022 11:31:02am
    Author:  bwhat

  ==============================================================================
*/

#include "SynthAudioSource.h"

//==============================================================================
/*
    PulsarSound is a passive class that describes the sound to a Synthesiser object.
    The audio rendering is done by the SynthesiserVoice class.

    PulsarSound may choose which midinotes and channels trigger the voice.

    I create the wavetables for both the window and the wavelet here, they are public member variables.
    I should make the program safer by creating access functions to these variables so they may not be
    modified but only retreived.
*/

struct PulsarSound : public juce::SynthesiserSound
{
    PulsarSound()
    {
    };

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }

    juce::AudioSampleBuffer sineTable;
    juce::AudioSampleBuffer windowTable;

private:
    const unsigned int tableSize = 1 << 9;
};

//==============================================================================
// A voice plays a single sound at a time but a Synthesiser can hold an array of voices.
struct PulsarVoice : public juce::SynthesiserVoice
{
    /* set adsr sample rate in SineWaveVoice constructor. */
    PulsarVoice()
    {
        adsr.setSampleRate(getSampleRate());
        createSineTable();
        createWindowTable();
        /* I use a unique pointer to handle the deletion of memory for me. */
        _pulsar = std::make_unique<Pulsar>(sineTable, windowTable);
    };

    /* In this case there is only one sound, so returns true if not a nullptr. */
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        /* 
        * dynamic cast accesses the custom SynthesiserSound struct, rather than the base class.
        * so check if sound is not a null pointer and then return true so this voice can play a sound.
        */
        return dynamic_cast <PulsarSound*> (sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
    {
        /* edit this to spread notes over octave (seee the wavetable tutorial) */
        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        cyclesPerSample = (float)cyclesPerSecond / (float)getSampleRate();

        /*
        * calculate 12th root equal tempered scale.
        * there is a midi note to Hz juce function but I want to modify the conversion to include other scales.
        */
        frequency = 440.0 * pow(2.0, (midiNoteNumber - 69.0) / 12.0);

        level = velocity * 0.5f;
        adsr.noteOn();
        
        if (_keyboardControl)
        {
            _pulsar->setFundamental((float)frequency);
        } else 
        {
            _pulsar->setFundamental(_fundamental);
        }
        
        _pulsar->setPeriod(_period, _periodSpread);
        _pulsar->setFormant(_formant);
        _pulsar->setIndex(_index);
        _pulsar->setStochasticMasking(_masking);
    }

    void stopNote(float /*velocity*/, bool)
    {
        adsr.noteOff();
    }

    // pure virtual functions must be initialised.
    void pitchWheelMoved(int)      override {};
    void controllerMoved(int, int) override {};

    // renderNextBlock now uses an adsr.
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {

        /* amplitde envelope params */
        amplitudeParameters.attack  = amplitudeAttack;
        amplitudeParameters.decay   = amplitudeDecay;
        amplitudeParameters.sustain = amplitudeSustain;
        amplitudeParameters.release = amplitudeRelease;
        
        adsr.setParameters(amplitudeParameters);

        while (--numSamples >= 0)
        {

             if (_keyboardControl)
             {
                 _pulsar->setFundamental((float)frequency);
             }
             else
             {
                 auto smoothFundamental = fundamentalSmooth.smooth(_fundamental, numSamples);
                 _pulsar->setFundamental(smoothFundamental);
             }

             auto smoothPeriod = periodSmooth.smooth(_period, numSamples);
             auto smoothSpread = periodSpreadSmooth.smooth(_periodSpread, numSamples);
             auto smoothFormant = formantSmooth.smooth(_formant, numSamples);
             auto smoothIndex = indexSmooth.smooth(_index, numSamples);
             
             _pulsar->setPeriod(smoothPeriod, smoothSpread);
             _pulsar->setFormant(smoothFormant);
             _pulsar->setIndex(smoothIndex);
             _pulsar->setStochasticMasking(_masking);

             float currentSample = _pulsar->getNextSample((float)getSampleRate()) * level * adsr.getNextSample();

             for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
             {
                 outputBuffer.addSample(i, startSample, currentSample);
             }
             
             ++startSample;

             if (!adsr.isActive())
             {
                 /* reset pulsar? */
                 clearCurrentNote();
                 break;
             }
        }
    }

    void createWindowTable()
    {
        windowTable.setSize(1, (int)tableSize + 1);
        windowTable.clear();

        auto* samples = windowTable.getWritePointer(0);

        auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1);
        auto currentAngle = 0.0;

        for (unsigned int i = 0; i < tableSize; ++i)
        {
            auto sample = 0.5 - 0.5 * cos(currentAngle);
            samples[i] = (float)sample;
            currentAngle += angleDelta;
        }

        samples[tableSize] = samples[0];
    }

    void createSineTable()
    {
        /*
        * set table size with 1 channel.
        * get write pointer channel 0.
        */

        sineTable.setSize(1, (int)tableSize + 1);
        sineTable.clear();

        auto* samples = sineTable.getWritePointer(0);

        auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1);
        auto currentAngle = 0.0;

        for (unsigned int i = 0; i < tableSize; ++i)
        {
            auto sample = std::sin(currentAngle);
            samples[i] = (float)sample;
            currentAngle += angleDelta;
        }

        samples[tableSize] = samples[0];
    }
    float amplitudeAttack = 0.0f, amplitudeDecay = 0.0f, amplitudeSustain = 0.0f, amplitudeRelease = 0.0f;
    float _fundamental = 100.0f;
    bool _keyboardControl = false;
    float _period = 0.0f, _periodSpread = 0.0f;
    float _formant = 0.0f;
    float _index = 0.0f;
    int   _masking = 0;
private:
    juce::ADSR::Parameters amplitudeParameters { 0.1f, 0.1f, 0.5f, 0.1f };
    juce::ADSR adsr;
    std::unique_ptr<Pulsar> _pulsar;
    float level = 0.0f;
    float cyclesPerSample = 0.0f;
    juce::AudioSampleBuffer sineTable;
    juce::AudioSampleBuffer windowTable;
    const unsigned int tableSize = 1 << 9;
    juce::Random random;
    Smooth fundamentalSmooth, periodSmooth, periodSpreadSmooth, formantSmooth, indexSmooth;
    double frequency = 0.0;
};

//==============================================================================

SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState) : keyboardState(keyState)
{
    for (auto i = 0; i < numVoices; ++i)
    {
        synth.addVoice(new PulsarVoice);
    }

    synth.addSound(new PulsarSound);
}

void SynthAudioSource::setUsingPulsarSound()
{
    synth.clearSounds();
}

// I don't need to include the override keyword in the declaration
void SynthAudioSource::prepareToPlay(int, double sampleRate)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SynthAudioSource::releaseResources()
{
}

// AudioSourceChannelInfo is a struct used by getNextAudioBlock. 
void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo& buffertToFill)
{
    buffertToFill.clearActiveBufferRegion();

    juce::MidiBuffer incomingMidi;
    keyboardState.processNextMidiBuffer (incomingMidi, buffertToFill.startSample, buffertToFill.numSamples, true);
    synth.renderNextBlock (*buffertToFill.buffer, incomingMidi, buffertToFill.startSample, buffertToFill.numSamples);
}

void SynthAudioSource::amplitudeEnvelope(float set_attack, float set_decay, float set_sustain, float set_release)
{ 
   for (auto i = 0; i < synth.getNumVoices(); ++i)
   {
       juce::SynthesiserVoice* voicePtr{ synth.getVoice(i) };
       PulsarVoice* PulsarVoicePtr{ dynamic_cast<PulsarVoice*> (voicePtr) };                 
       
       PulsarVoicePtr->amplitudeAttack  = set_attack;
       PulsarVoicePtr->amplitudeDecay   = set_decay;
       PulsarVoicePtr->amplitudeSustain = set_sustain;
       PulsarVoicePtr->amplitudeRelease = set_release;
   }
}

void SynthAudioSource::setFundamental(float fundamental)
{
    for (auto i = 0; i < synth.getNumVoices(); ++i)
    {
        juce::SynthesiserVoice* voicePtr{ synth.getVoice(i) };
        PulsarVoice* PulsarVoicePtr{ dynamic_cast<PulsarVoice*> (voicePtr) };
       
        PulsarVoicePtr->_fundamental = fundamental;
    }
}

void SynthAudioSource::setKeyboardControl(bool keyboardControl)
{
    for (auto i = 0; i < synth.getNumVoices(); ++i)
    {
        juce::SynthesiserVoice* voicePtr{ synth.getVoice(i) };
        PulsarVoice* PulsarVoicePtr{ dynamic_cast<PulsarVoice*> (voicePtr) };

        PulsarVoicePtr->_keyboardControl = keyboardControl;
    }
}

void SynthAudioSource::setPeriod(float period)
{
    for (auto i = 0; i < synth.getNumVoices(); ++i)
    {
        juce::SynthesiserVoice* voicePtr{ synth.getVoice(i) };
        PulsarVoice* PulsarVoicePtr{ dynamic_cast<PulsarVoice*> (voicePtr) };

        PulsarVoicePtr->_period = period;
    }
}

void SynthAudioSource::setPeriodSpread(float spread)
{
    for (auto i = 0; i < synth.getNumVoices(); ++i)
    {
        juce::SynthesiserVoice* voicePtr{ synth.getVoice(i) };
        PulsarVoice* PulsarVoicePtr{ dynamic_cast<PulsarVoice*> (voicePtr) };

        PulsarVoicePtr->_periodSpread = spread;
    }
}

void SynthAudioSource::setFormant(float formant)
{
    for (auto i = 0; i < synth.getNumVoices(); ++i)
    {
        juce::SynthesiserVoice* voicePtr{ synth.getVoice(i) };
        PulsarVoice* PulsarVoicePtr{ dynamic_cast<PulsarVoice*> (voicePtr) };

        PulsarVoicePtr->_formant  = formant;
    }
}

void SynthAudioSource::setIndex(float index)
{
    for (auto i = 0; i < synth.getNumVoices(); ++i)
    {
        juce::SynthesiserVoice* voicePtr{ synth.getVoice(i) };
        PulsarVoice* PulsarVoicePtr{ dynamic_cast<PulsarVoice*> (voicePtr) };

        PulsarVoicePtr->_index = index;
    }
}

void SynthAudioSource::setMasking(int masking)
{
    for (auto i = 0; i < synth.getNumVoices(); ++i)
    {
        juce::SynthesiserVoice* voicePtr{ synth.getVoice(i) };
        PulsarVoice* PulsarVoicePtr{ dynamic_cast<PulsarVoice*> (voicePtr) };

        PulsarVoicePtr->_masking = masking;
    }
}