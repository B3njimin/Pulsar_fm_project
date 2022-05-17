#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() : keyBoardComponent(keyBoardState, juce::MidiKeyboardComponent::horizontalKeyboard),
                                 bufferSize(1024),
                                 synthAudioSource(keyBoardState),
                                 ampAdsr(synthAudioSource)
{
    int displayNum = 2;

    getLookAndFeel().setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

    addAndMakeVisible(ampAdsr);

    addAndMakeVisible(fundamentalSlider);
    addAndMakeVisible(fundamentalLabel);

    fundamentalLabel.attachToComponent(&fundamentalSlider, false);
    fundamentalLabel.setText("Fundamental", juce::dontSendNotification);
    fundamentalLabel.setJustificationType(juce::Justification::centred);
    fundamentalLabel.setMouseClickGrabsKeyboardFocus(false);

    fundamentalSlider.setRange(1., 400.0);
    fundamentalSlider.setSkewFactorFromMidPoint(50);
    fundamentalSlider.setValue(fundamental, juce::dontSendNotification);
    fundamentalSlider.setNumDecimalPlacesToDisplay(displayNum);
    fundamentalSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
    fundamentalSlider.setSliderStyle(juce::Slider::LinearVertical);
    fundamentalSlider.addListener(this);
    fundamentalSlider.onValueChange = [this] { fundamental = (float)fundamentalSlider.getValue(); };
    fundamentalSlider.setMouseClickGrabsKeyboardFocus(false);

    addAndMakeVisible(formantSlider);
    addAndMakeVisible(formantLabel);

    formantLabel.attachToComponent(&formantSlider, false);
    formantLabel.setText("Formant", juce::dontSendNotification);
    formantLabel.setJustificationType(juce::Justification::centred);
    formantLabel.setMouseClickGrabsKeyboardFocus(false);

    formantSlider.setRange(1, 12.0);
    formantSlider.setValue(formant, juce::dontSendNotification);
    formantSlider.setNumDecimalPlacesToDisplay(displayNum);
    formantSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
    formantSlider.setSliderStyle(juce::Slider::LinearVertical);
    formantSlider.addListener(this);
    formantSlider.onValueChange = [this] { formant = (float)formantSlider.getValue(); };
    formantSlider.setMouseClickGrabsKeyboardFocus(false);

    addAndMakeVisible(formantSpreadSlider);
    addAndMakeVisible(formantSpreadLabel);

    formantSpreadLabel.attachToComponent(&formantSpreadSlider, false);
    formantSpreadLabel.setText("Spread", juce::dontSendNotification);
    formantSpreadLabel.setJustificationType(juce::Justification::centred);
    formantSpreadLabel.setMouseClickGrabsKeyboardFocus(false);

    formantSpreadSlider.setRange(1, 6.0);
    formantSpreadSlider.setValue(formantSpread, juce::dontSendNotification);
    formantSpreadSlider.setNumDecimalPlacesToDisplay(displayNum);
    formantSpreadSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
    formantSpreadSlider.setSliderStyle(juce::Slider::LinearVertical);
    formantSpreadSlider.addListener(this);
    formantSpreadSlider.onValueChange = [this] { formantSpread = (float)formantSpreadSlider.getValue(); };
    formantSpreadSlider.setMouseClickGrabsKeyboardFocus(false);

    addAndMakeVisible(dutyCycleSlider);
    addAndMakeVisible(dutyCycleLabel);

    dutyCycleLabel.attachToComponent(&dutyCycleSlider, false);
    dutyCycleLabel.setText("DutyCycle", juce::dontSendNotification);
    dutyCycleLabel.setJustificationType(juce::Justification::centred);
    dutyCycleLabel.setMouseClickGrabsKeyboardFocus(false);

    dutyCycleSlider.setRange(0.01, 1.0);
    dutyCycleSlider.setSkewFactor(0.4);
    dutyCycleSlider.setValue(dutyCycle, juce::dontSendNotification);
    dutyCycleSlider.setNumDecimalPlacesToDisplay(displayNum);
    dutyCycleSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
    dutyCycleSlider.setSliderStyle(juce::Slider::LinearVertical);
    dutyCycleSlider.addListener(this);
    dutyCycleSlider.onValueChange = [this] { dutyCycle = (float)dutyCycleSlider.getValue(); };
    dutyCycleSlider.setMouseClickGrabsKeyboardFocus(false);

    addAndMakeVisible(indexSlider);
    addAndMakeVisible(indexLabel);

    indexLabel.attachToComponent(&indexSlider, false);
    indexLabel.setText("Index", juce::dontSendNotification);
    indexLabel.setJustificationType(juce::Justification::centred);
    indexLabel.setMouseClickGrabsKeyboardFocus(false);
    
    indexSlider.setRange(0.0, 1.0);
    indexSlider.setValue(0, juce::dontSendNotification);
    indexSlider.setNumDecimalPlacesToDisplay(displayNum);
    indexSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
    indexSlider.setSliderStyle(juce::Slider::LinearVertical);
    indexSlider.addListener(this);
    indexSlider.onValueChange = [this] { index = (float)indexSlider.getValue(); };
    indexSlider.setMouseClickGrabsKeyboardFocus(false);

    /* will not require interpolation */
    addAndMakeVisible(stochasticMaskingSlider);
    addAndMakeVisible(stochasticMaskingLabel);

    stochasticMaskingLabel.attachToComponent(&stochasticMaskingSlider, false);
    stochasticMaskingLabel.setText("Masking", juce::dontSendNotification);
    stochasticMaskingLabel.setJustificationType(juce::Justification::centred);
    stochasticMaskingLabel.setMouseClickGrabsKeyboardFocus(false);
    
    stochasticMaskingSlider.setRange(0, 100, 1);
    stochasticMaskingSlider.setSkewFactor(0.7);
    stochasticMaskingSlider.setValue(0, juce::dontSendNotification);
    stochasticMaskingSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
    stochasticMaskingSlider.setSliderStyle(juce::Slider::LinearVertical);
    stochasticMaskingSlider.addListener(this);
    stochasticMaskingSlider.onValueChange = [this] { maskingPercentage = (int)stochasticMaskingSlider.getValue(); };
    stochasticMaskingSlider.setMouseClickGrabsKeyboardFocus(false);

    addAndMakeVisible(keyboardToggle);
    keyboardToggle.onClick = [this] { keyboardControl = keyboardToggle.getToggleState();  };
    keyboardToggle.setMouseClickGrabsKeyboardFocus(false);

    addAndMakeVisible(keyBoardComponent);

    setSize(800, 500);
    setAudioChannels(0, 2);

    // give focus to the keyboard.
    startTimer(400);

    buffer.setSize(1, (int)bufferSize + 1);

    auto* _samples = buffer.getWritePointer(0);

    auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1);
    auto currentAngle = 0.0;

    for (int i = 0; i < bufferSize; ++i)
    {
        // juce::Random random;
        auto sample = std::sin(currentAngle);
        _samples[i] = float(sample);
        currentAngle += angleDelta;
    }

    _samples[bufferSize] = _samples[0];

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(2, 2);
    }
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    currentSampleRate = sampleRate;

    _oscillator = std::make_unique<Pulsar>(sineTable, windowTable);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    synthAudioSource.getNextAudioBlock  (bufferToFill);
    synthAudioSource.amplitudeEnvelope  (ampAdsr.getAttack(), ampAdsr.getDecay(), ampAdsr.getSustain(), ampAdsr.getRelease());
    synthAudioSource.setKeyboardControl (keyboardControl);
    synthAudioSource.setFundamental     (fundamental);
    synthAudioSource.setFormant         (formant);
    synthAudioSource.setFormantSpread   (formantSpread);
    synthAudioSource.setDutyCycle       (dutyCycle);
    synthAudioSource.setIndex           (index);
    synthAudioSource.setMasking         (maskingPercentage);
}

void MainComponent::releaseResources()
{
    synthAudioSource.releaseResources();
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    float grey = 0.1f;
    g.fillAll(juce::Colour::fromFloatRGBA(grey, grey, grey, 1.0));
}

void MainComponent::resized()
{
    keyBoardComponent.setBounds(0, getHeight() - 100, getWidth(), 100);

    int margin = 40;

    int width = getWidth();
    int height = getHeight() - keyBoardComponent.getHeight();

    int adsrWidth = width / 4;
    int adsrHeight = height / 2;

    int sliderDistance = (width) / 6;

    int sliderWidth  = sliderDistance;
    int sliderHeight = height / 2;

    int toggleWidth = 90;
    int toggleHeight = 30;

    keyboardToggle.setBounds(0, 0, toggleWidth, toggleHeight);
    ampAdsr.setBounds(width / 2 - (adsrWidth / 2), 0, adsrWidth, adsrHeight - margin);

    fundamentalSlider.setBounds       (0,                  adsrHeight, sliderWidth, sliderHeight - margin);
    formantSlider.setBounds           (sliderDistance,     adsrHeight, sliderWidth, sliderHeight - margin);
    formantSpreadSlider.setBounds     (sliderDistance * 2, adsrHeight, sliderWidth, sliderHeight - margin);
    indexSlider.setBounds             (sliderDistance * 4, adsrHeight, sliderWidth, sliderHeight - margin);
    dutyCycleSlider.setBounds         (sliderDistance * 3, adsrHeight, sliderWidth, sliderHeight - margin);
    stochasticMaskingSlider.setBounds (sliderDistance * 5, adsrHeight, sliderWidth, sliderHeight - margin);
}

//==============================================================================
void MainComponent::sliderValueChanged(juce::Slider*)
{
}

//==============================================================================
void MainComponent::timerCallback()
{
    // give the computer keyboard focus to this component. 
    keyBoardComponent.grabKeyboardFocus();
    keyBoardComponent.setKeyPressBaseOctave(3);
   
    stopTimer();
}