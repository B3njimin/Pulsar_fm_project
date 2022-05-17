/*
  ==============================================================================

    Smooth.cpp
    Created: 13 May 2022 9:50:53am
    Author:  bwhat

  ==============================================================================
*/

#include "Smooth.h"

Smooth::Smooth()
{
}

Smooth::~Smooth()
{
}

float Smooth::smooth(float input, int numSamples)
{
    float localTarget = input;
    if (localTarget != current)
    {
        float increment = (localTarget - current) / numSamples;
        current += increment;
    }
    return current;
}