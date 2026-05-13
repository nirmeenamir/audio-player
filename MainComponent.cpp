#include "MainComponent.h"

MainComponent::MainComponent()
    : playerGUI(playerAudio)
{
    addAndMakeVisible(playerGUI);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void MainComponent::resized()
{
    playerGUI.setBounds(getLocalBounds());

}
