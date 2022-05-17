/*
  ==============================================================================

    Rate.cpp
    Created: 13 May 2022 10:18:09am
    Author:  bwhat

  ==============================================================================
*/

#include "Rate.h"
#include <cmath>

Rate::Rate()
{
}

Rate::~Rate()
{
}

float Rate::rate(float input, float multiplier)
{
    auto inputPhasor = input;
    float difference = (inputPhasor - previousPhasor);
    bool reset = (difference < 0.0f) ? true : false;
    difference = (difference > 0.0f) ? difference : 0.0f;
    difference *= multiplier;

    if (reset) outputPhasor = 0.0;
    outputPhasor += difference;
    outputPhasor -= (int)outputPhasor;
    previousPhasor = inputPhasor;

    return outputPhasor;
}