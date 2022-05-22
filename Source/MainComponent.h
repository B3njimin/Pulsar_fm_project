#pragma once

#include <JuceHeader.h>
#include "Smooth.h"
#include "Rate.h"
#include "Wavetable.h"
#include "Pulsar.h"
#include "SynthAudioSource.h"
#include "ADSR.h"

//==============================================================================

class MainComponent : public juce::AudioAppComponent,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    void sliderValueChanged(juce::Slider* slider);

    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    void setMidiInput(int index);

private:
    //==============================================================================
    // 'represents' a piano keyboard, keeps track of which keys are pressed.
    juce::MidiKeyboardState keyBoardState;

    // displays a piano keyboard.
    // change broadcaster transpose notes played by keyboardfocus.
    juce::MidiKeyboardComponent keyBoardComponent;

    // synth audio source inherits from AudioSource it's virtual functions must be initialised.
    SynthAudioSource synthAudioSource;
    
    float level = 0.25f;
    double currentSampleRate = 0;

    const unsigned int tableSize = 1 << 9;

    juce::AudioSampleBuffer sineTable;
    juce::AudioSampleBuffer windowTable;

    /* use a JUCE owned array */
    std::unique_ptr<Pulsar> _oscillator;

    juce::AudioBuffer<float> buffer;
    int bufferSize;
    int writePos = 0;

    float fundamental = 220.0f;
    float period = 1.0f, periodSpread = 1.0f;
    float formant = 1.0f;
    float index = 0.0f;
    int maskingPercentage = 0;

    adsrGuiComponent ampAdsr;

    juce::Slider fundamentalSlider;
    juce::Slider periodSlider;
    juce::Slider periodSpreadSlider;
    juce::Slider formantSlider;
    juce::Slider indexSlider;
    juce::Slider stochasticMaskingSlider;

    juce::Label fundamentalLabel;
    juce::Label periodLabel;
    juce::Label periodSpreadLabel;
    juce::Label formantLabel;
    juce::Label indexLabel;
    juce::Label stochasticMaskingLabel;

    juce::ToggleButton keyboardToggle { "Keyboard" };

    bool keyboardControl = false;

    Smooth fundamentalSmooth;
    Smooth periodSmooth, periodSpreadSmooth;
    Smooth formantSmooth, formantSpreadSmooth;

    int lastInputIndex = 0;
    bool isAddingFromMidiInput = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
