/*
  ==============================================================================

    Wavetable.cpp
    Created: 13 May 2022 10:21:03am
    Author:  bwhat

  ==============================================================================
*/

#include "Wavetable.h"

Wavetable::Wavetable(const juce::AudioSampleBuffer& tableToUse)
    : wavetable(tableToUse),
    tableSize(wavetable.getNumSamples() - 1)
{
    /* this should be here but It created error's when testing on apple machines */
    // jassert(wavetable.getNumChannels() == 1);
}

float Wavetable::getNextSample(float index)
{
    /*
    * interpolate between samples.
    * wrap next index around the size of the table.
    * retreive fractional value by subtracting from the truncated current index.
    * get AudioSampleBuffer pointer and the two sample values.
    * interpolation is the value at the truncated index + the diference between that value and the next multiplied by the fracional value.
    * why is it not just the diference between both values?
    * say value0 is 0.5 and value1 is 0.25 (0.25 - 0.5 = -0.25).
    * we have the right direction but not the correct value, the next between 0.5 and 0.25 is not -0.25.
    * tableDelta is 1.277 so if the current index is 0 + 1.277 index0 is 1. and frac is 1.277 - 1. = 0.277
    * so 0.5 + (0.277 * -0.25) is (-0.69)
    */

    auto index0 = (unsigned int)_index;
    auto index1 = index0 + 1; // no wrapping, compensated for in create wavetable.

    auto frac = _index - (float)index0;

    auto* table = wavetable.getReadPointer(0);
    auto value0 = table[index0];
    auto value1 = table[index1];

    auto currentSample = value0 + frac * (value1 - value0);

    /*
    * in order to perform fm with a unipolar signal I need the index to wrap.
    * This may not be the ideal solution test further.
    */
    _index = abs(fmod(index * (float)tableSize, (float)tableSize));
    
    return currentSample;
}

Wavetable::~Wavetable()
{
}