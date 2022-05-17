/*
  ==============================================================================

    ADSR.cpp
    Created: 15 May 2022 12:24:22am
    Author:  bwhat

    Unsure about this implementation.
    Should I return the parameter array, or dispense with the paramters array alltogether?

    The label component is still to be fully implemented
    attaching a label to an index in an array is unsuccessful.

  ==============================================================================
*/

#include "ADSR.h"

//==============================================================================
adsrGuiComponent::adsrGuiComponent(SynthAudioSource& synthSource) : synthAudioSource(synthSource)
{
    m_destination = 0;
    audioSourceptr = &synthSource;

    for (int i = 0; i < 4; ++i)
    {
        sliders.add(new juce::Slider);
        addAndMakeVisible(sliders[i]);

        sliders[i]->setSliderStyle(juce::Slider::LinearVertical);
        sliders[i]->setTextBoxIsEditable(false);
        sliders[i]->setTextBoxStyle(juce::Slider::NoTextBox, true, sliders[i]->getTextBoxWidth(), 30);
        sliders[i]->setRange(0.0, 1.0, 0.001);
        sliders[i]->addListener(this);
        sliders[i]->setValue(0.5);
        parameters[i] = (float)sliders[i]->getValue();
    }

    addAndMakeVisible(A);
    A.attachToComponent(sliders[0], false);
    A.setText("A", juce::dontSendNotification);

    // addAndMakeVisible(D);
    // D.attachToComponent(sliders[1], false);
    // D.setText("D", juce::dontSendNotification);
    // 
    // addAndMakeVisible(A);
    // S.attachToComponent(sliders[2], false);
    // S.setText("S", juce::dontSendNotification);
    // 
    // addAndMakeVisible(R);
    // R.attachToComponent(sliders[3], false);
    // R.setText("R", juce::dontSendNotification);
}

void adsrGuiComponent::resized()
{
    for (int i = 0; i < 4; i++)
    {
        sliders[i]->setBounds((getWidth() / 4) * i, 0, getWidth() / 4, getHeight());
    }

    // A.setBounds(0, 0,  getWidth() / 4, getHeight());
    // D.setBounds(0, 0, 20, 10);
}

void adsrGuiComponent::sliderValueChanged(juce::Slider* slider)
{
    for (int i = 0; i < 4; i++)
    {
        if (slider == sliders[i]) { parameters[i] = (float)slider->getValue(); }
    }
}

float adsrGuiComponent::getAttack()
{
    return parameters[0];
}

float adsrGuiComponent::getDecay()
{
    return parameters[1];
}

float adsrGuiComponent::getSustain()
{
    return parameters[2];
}

float adsrGuiComponent::getRelease()
{
    return parameters[3];
}

void adsrGuiComponent::setAudioSource(SynthAudioSource& audioSource)
{
    audioSourceptr = &audioSource;
}

adsrGuiComponent::~adsrGuiComponent()
{
}