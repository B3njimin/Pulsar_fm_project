/*
  ==============================================================================

    Rate.h
    Created: 13 May 2022 10:18:09am
    Author:  bwhat

  ==============================================================================
*/
#pragma once

/* phasor multiplier like rate in gen~ */
class Rate
{
public:
    Rate();
    ~Rate();
    float rate(float input, float multiplier);
private:
    float outputPhasor = 0.0f;
    float previousPhasor = 0.0f;
};