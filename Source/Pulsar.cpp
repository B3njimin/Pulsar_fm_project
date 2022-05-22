/*
  ==============================================================================

    Pulsar.cpp
    Created: 13 May 2022 10:20:18am
    Author:  bwhat

    Simple Pulsar implementation with a hann window and parallel frequency modulated wavelet.

  ==============================================================================
*/

#include "Pulsar.h"

/*
* Fill each array with the window and waveform table to use create each pulsaret.
* Using a juce::OwnedArray takes care of deleting each Wavetable pointer for me.
*/

Pulsar::Pulsar(const juce::AudioSampleBuffer& waveTableToUse, const juce::AudioSampleBuffer& windowTableToUse)
{
    for (int i = 0; i < numWavelets; ++i)
    {
        auto* window       = new Wavetable (windowTableToUse);
        auto* wavelet      = new Wavetable (waveTableToUse);
        auto* modulatorOne = new Wavetable (waveTableToUse);
        auto* modulatorTwo = new Wavetable (waveTableToUse);

        wavelets.add      (wavelet);
        windows.add       (window);
        modulatorsOne.add (modulatorOne);
        modulatorsTwo.add (modulatorTwo);

        /* initialise float vectors. */
        _formants           .push_back(0.0f);
        waveletPhasors      .push_back(0.0f);
        windowPhasors       .push_back(0.0f);
        modulatorOnePhasors .push_back(0.0f);
        modulatorTwoPhasors .push_back(0.0f);
        carrierPhasors      .push_back(0.0f);
    }
}

/* destructor */
Pulsar::~Pulsar()
{
}

/*
* access functions, set Pulsar member variables or synthesis parameters.
*/
void Pulsar::setFundamental(float fundamental)
{
    _fundamental = fundamental;
}

void Pulsar::setPeriod(float period, float spread)
{
    _period = period;
    _periodSpread = spread;
}

void Pulsar::setFormant(float formant)
{
    for (int i = 0; i < numWavelets; ++i)
    {
        _formants[i] = (1.0f / formant);
    }
}

void Pulsar::setIndex(float index)
{
    _index = index;
}

void Pulsar::setStochasticMasking(int maskingPercentage)
{
    _maskingPercentage = maskingPercentage;
}


float Pulsar::getNextSample(float sampleRate)
{
    /* Phasor used to trigger or 'spawn' pulsars. */
    phasor += _fundamental * (1.0f / sampleRate);
    phasor -= (int)phasor;

    /*
    * when the direction is negative the phasor has returned to zero.
    * If negative spawn returns true.
    */
    bool spawn = (phasor - previousPhasor > 0.0) ? true : false;

    /* Keep track of the difference between successive samples. 'delta' */
    previousPhasor = phasor;

    /* 'Drive' the window at the fundamental frequency of the pulsar system. */
    fundamentalPhasor += _fundamental * (1.0f / sampleRate);
   
    /* 
    * Create random number between 0 and 99, only reset if the random number is greater than the masking value.
    * When the inner conditional returns true reset all the phasors.
    * Previously I had used the Rate object to multiply the frequency of each phasor but this proved
    * problematic for frequency modulation.
    */
    if (!spawn)
    {
        if (random.nextInt(100) >= _maskingPercentage)
        {
            fundamentalPhasor = 0.0f;

            for (int i = 0; i < numWavelets; ++i)
            {
                modulatorOnePhasors [i] = 0.0f;
                modulatorTwoPhasors [i] = 0.0f;
                carrierPhasors      [i] = 0.0f;
            }
        }
    }
    
    for (int i = 0; i < numWavelets; ++i)
    {
        /*
        * i + 1 so that the first fundamental is multiplied by 1 and not 0.
        * each phasor's frequency is also multiplied by the formant and then a ratio that
        * you might expect to see in a frequency modulation system.
        * 
        * Here the ratio's are fixed, this of course needs parametising in the future.
        * This is a parallel modulation system as opposed to a seriers or stacked system.
        */

        modulatorTwoPhasors[i] += (((_fundamental * _formants[i]) * _period) * ratioOne) * (1.0f / sampleRate);
        auto modTwo = modulatorsTwo[i]->getNextSample(modulatorTwoPhasors[i]);

        modulatorOnePhasors[i] += ((((_fundamental * _formants[i]) * _period) * ratioTwo)) * (1.0f / sampleRate);
        auto modOne = modulatorsOne[i]->getNextSample(modulatorOnePhasors[i]);
 
        /* modulators are summed and scaled before added to carrier frequency. */
        auto modOnePlusTwo = modOne * (indexOne * _index) + modTwo * (indexTwo * _index);

        auto carrierFrequency = (_fundamental * _formants[i]) * _period * pow((i + 1) * _periodSpread, 1.5f);
        
        carrierPhasors[i] += (carrierFrequency + modOnePlusTwo) * (1.0f / sampleRate);

        /* ensure the phasor does not exceed one, a clamp to squish the window. */
        windowPhasors[i] = (fundamentalPhasor * _formants[i] > 1.0f) ? 1.0f : fundamentalPhasor * _formants[i];
    }

    /* clear output. */
    auto output = 0.0f;

    /* 'window' the resulting waveforms and scale output by number of pulsarets. */
    for (int i = 0; i < numWavelets; ++i)
    {
        output += wavelets[i]->getNextSample(carrierPhasors[i]) * windows[i]->getNextSample(windowPhasors[i]);
    }

    return output * (1.0f / (float)numWavelets);
}