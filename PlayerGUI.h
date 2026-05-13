#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Timer,
    public juce::KeyListener
{
public:
    PlayerGUI(PlayerAudio& audioRef);
    ~PlayerGUI() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;
private:
    PlayerAudio& audio;

    juce::TextButton loadButton{ "Load" };
    juce::TextButton playPauseButton;
	juce::TextButton addTrackButton{ "Add Audio" };
    juce::TextButton gostartButton{ "Go to Start" };
    juce::TextButton endButton{ "End" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton A_SegButton{ "Set A" };
	juce::TextButton B_SegButton{ "Set B" };
    juce::TextButton AB_SegButton{ "A-B Looping" };

    // Areas for layout
    juce::Rectangle<int> playerArea1, playerArea2;
    juce::Rectangle<int> waveformArea, waveformArea2;
    juce::Rectangle<int> infoArea1, infoArea2;
    juce::Rectangle<int> playlistArea, playlistArea2;

    int rM = 0, rS = 0, rM2 = 0, rS2 = 0;
    bool isUserDraggingSlider = false;
    juce::TextButton loadButton2{ "Load" };
    juce::TextButton playPauseButton2;
    juce::TextButton addTrackButton2{ "Add Audio" };
    juce::TextButton gostartButton2{ "Go to Start" };
    juce::TextButton endButton2{ "End" };
    juce::TextButton loopButton2{ "Loop" };
    juce::TextButton muteButton2{ "Mute" };
    juce::TextButton A_SegButton2{ "Set A" };
    juce::TextButton B_SegButton2{ "Set B" };
    juce::TextButton AB_SegButton2{ "A-B Looping" };
    bool isUserDraggingSlider2 = false;
    bool player1Focused = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
