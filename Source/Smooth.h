/*
  ==============================================================================

    Smooth.h
    Created: 13 May 2022 9:50:53am
    Author:  bwhat

  ==============================================================================
*/

#pragma once

//==============================================================================
class Smooth
{
public:
    Smooth();
    ~Smooth();
    float smooth(float input, int numSamples);
private:
    float current = 0.0f;
};