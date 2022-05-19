/*
  ==============================================================================

    Pulsar.h
    Created: 13 May 2022 10:20:18am
    Author:  bwhat

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include "Wavetable.h"

class Pulsar
{
public:
    Pulsar(const juce::AudioSampleBuffer& waveTableToUse, const juce::AudioSampleBuffer& windowTableToUse);
    ~Pulsar();
    float getNextSample(float sampleRate);
    void setFundamental(float fundamental);
    void setFormant(float formant, float spread);
    void setDutyCycle(float dutyCycle);
    void setIndex(float index);
    void setStochasticMasking(int maskingPercentage);
private:
    /* number of waveforms within a single envelope. */
    int numWavelets = 5;
    
    juce::OwnedArray<Wavetable> wavelets;
    juce::OwnedArray<Wavetable> windows;
    juce::OwnedArray<Wavetable> modulatorsOne, modulatorsTwo;

    std::vector<float> _dutyCycles;
    std::vector<float> waveletPhasors;
    std::vector<float> windowPhasors;
    std::vector<float> carrierPhasors, modulatorOnePhasors, modulatorTwoPhasors;
   
    float phasor = 0.0f, previousPhasor = 0.0f;
    float fundamentalPhasor = 0.0f;

    float _fundamental = 0.0f;
    float _formant = 0.0f, _formantSpread = 0.0f;
    float _index = 0.0f;
    int _maskingPercentage = 50;

    /* I chose these values to create a formant, vowel like sound. */
    float ratioOne = 2.0f;
    float ratioTwo = 9.0f;

    float indexOne = 1500.0f;
    float indexTwo = 1250.0f;

    juce::Random random;
};