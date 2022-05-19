/*
  ==============================================================================

    Wavetable.h
    Created: 13 May 2022 10:21:03am
    Author:  bwhat

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>

/* this class takes as input a phasor and reads a wavetable with simple linear interpolation. */
class Wavetable
{
public:
    Wavetable(const juce::AudioSampleBuffer& tableToUse);
    ~Wavetable();
    float getNextSample(float index);
private:
    const juce::AudioSampleBuffer& wavetable;
    float _index = 0.0f;
    int tableSize;
};