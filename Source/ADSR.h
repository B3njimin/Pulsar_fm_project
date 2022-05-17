/*
  ==============================================================================

    ADSR.h
    Created: 15 May 2022 12:24:22am
    Author:  bwhat

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SynthAudioSource.h"

#pragma once

//==============================================================================

class adsrGuiComponent : public juce::Component,
    public juce::Slider::Listener
{
public:
    adsrGuiComponent() = default;
    adsrGuiComponent(SynthAudioSource& synthSource);
    ~adsrGuiComponent() override;

    float getAttack();
    float getDecay();
    float getSustain();
    float getRelease();

    void sliderValueChanged(juce::Slider* slider) override;
    void resized();
    void setAudioSource(SynthAudioSource& audioSource);
private:
    SynthAudioSource& synthAudioSource;
    SynthAudioSource* audioSourceptr;
    int m_destination;
    juce::OwnedArray<juce::Slider> sliders;
    juce::Label A, D, S, R;
    std::array<float, 4> parameters;
};