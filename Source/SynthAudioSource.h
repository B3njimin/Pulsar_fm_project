/*
  ==============================================================================

    SynthAudioSource.h
    Created: 13 May 2022 11:31:02am
    Author:  bwhat

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Smooth.h"
#include "Pulsar.h"

#pragma once

class SynthAudioSource : public juce::AudioSource
{
public:
    SynthAudioSource(juce::MidiKeyboardState& keyState);
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void setUsingPulsarSound();
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void amplitudeEnvelope(float set_attack, float set_decay, float set_sustain, float set_release);
    void setFundamental(float fundamental);
    void setKeyboardControl(bool keyboardControl);
    void setPeriod(float period);
    void setPeriodSpread(float spread);
    void setFormant(float formant);
    void setIndex(float index);
    void setMasking(int masking);
private:
    // base class for a synthesiser.
    juce::Synthesiser synth;
    juce::MidiKeyboardState& keyboardState;
    int numVoices = 1;
};