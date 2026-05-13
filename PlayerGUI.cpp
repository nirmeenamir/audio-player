#include "PlayerGUI.h"
#include"PlayerAudio.h"
#include <cctype>

using namespace juce;
using namespace std;

const juce::Colour bgColour{ 0xFF2C2A4A };        // Dark purple-blue background
const juce::Colour playerBgColour{ 0xFF3E3C5F }; // Lighter purple for player sections
const juce::Colour accentPink{ 0xFFF9B9DE };     // Buttons
const juce::Colour accentYellow{ 0xFFFBF4B1 };   // Sliders, highlights
const juce::Colour accentPurple{ 0xFFD6B5F5 };   // Playlist selection
const juce::Colour accentBlue{ 0xFFC2DFFF };     // Slider track
const juce::Colour textColour{ 0xFFF0F0F0 };     // Main text
const juce::Colour darkTextColour{ 0xFF212121 }; // Text on light backgrounds


PlayerGUI::PlayerGUI(PlayerAudio& audioRef) : audio(audioRef)
{
    juce::TextButton* buttons1[] = { &loadButton, &playPauseButton, &addTrackButton, &gostartButton, &endButton, &loopButton, &muteButton, &A_SegButton, &B_SegButton, &AB_SegButton };
    for (auto* button : buttons1)
    {
        addAndMakeVisible(button);
        button->setColour(juce::TextButton::buttonColourId, accentPink);
        button->setColour(juce::TextButton::textColourOffId, darkTextColour);
        button->setColour(juce::TextButton::buttonOnColourId, accentPink.brighter(0.2f));
        button->setColour(juce::TextButton::textColourOnId, darkTextColour);
        button->setRepaintsOnMouseActivity(true);
    }

    juce::TextButton* buttons2[] = { &loadButton2, &playPauseButton2, &addTrackButton2, &gostartButton2, &endButton2, &loopButton2, &muteButton2, &A_SegButton2, &B_SegButton2, &AB_SegButton2 };
    for (auto* button : buttons2)
    {
        addAndMakeVisible(button);
        button->setColour(juce::TextButton::buttonColourId, accentPink);
        button->setColour(juce::TextButton::textColourOffId, darkTextColour);
        button->setColour(juce::TextButton::buttonOnColourId, accentPink.brighter(0.2f));
        button->setColour(juce::TextButton::textColourOnId, darkTextColour);
        button->setRepaintsOnMouseActivity(true);
    }

    // --- Player 1 Button Actions ---
    loadButton.onClick = [this]() { audio.loadAudio(); };
	playPauseButton.onClick = [this]() { audio.togglePlayPause(audio.getTransportSource(), 1); };
    gostartButton.onClick = [this]() { audio.goToStart(audio.getTransportSource()); };
	addTrackButton.onClick = [this]() { audio.loadAudio(false); };
    endButton.onClick = [this]() { audio.goToEnd(audio.getTransportSource(), audio.getReaderSource()); };
    loopButton.onClick = [this]() { audio.toggleLoop(); };
    muteButton.onClick = [this]() { audio.toggleMute(); };
    A_SegButton.onClick = [this]() { audio.A_Seg(1, !audio.getASeg()); };
	B_SegButton.onClick = [this]() { audio.B_Seg(1, !audio.getBSeg()); };
    AB_SegButton.onClick = [this]() { audio.toggleSegLooping(1); };

    // --- Player 2 Button Actions ---
    loadButton2.onClick = [this]() { audio.loadAudio2(); };
	playPauseButton2.onClick = [this]() { audio.togglePlayPause(audio.getTransportSource2(), 2); };
	addTrackButton2.onClick = [this]() { audio.loadAudio2(false); };
    gostartButton2.onClick = [this]() { audio.goToStart(audio.getTransportSource2()); };
    endButton2.onClick = [this]() { audio.goToEnd(audio.getTransportSource2(), audio.getReaderSource2()); };
    loopButton2.onClick = [this]() { audio.toggleLoop2(); };
    muteButton2.onClick = [this]() { audio.toggleMute2(); };
	A_SegButton2.onClick = [this]() { audio.A_Seg(2, !audio.getA2Seg()); };
	B_SegButton2.onClick = [this]() { audio.B_Seg(2, !audio.getB2Seg()); };
    AB_SegButton2.onClick = [this]() { audio.toggleSegLooping(2); };

    // --- Player 1 Sliders & Labels ---
    auto& timeSlider = audio.getTimeSlider();
    timeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    timeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    timeSlider.setEnabled(false);
    timeSlider.setColour(juce::Slider::trackColourId, accentBlue.darker(0.3f));
    timeSlider.setColour(juce::Slider::backgroundColourId, accentBlue.darker(0.3f));
    timeSlider.setColour(juce::Slider::thumbColourId, accentYellow);
    addAndMakeVisible(timeSlider);

    auto& currentTimeLabel = audio.getCurrentTimeLabel();
    currentTimeLabel.setJustificationType(juce::Justification::centredRight);
    currentTimeLabel.setFont(juce::Font{ 16.0f, juce::Font::bold });
    currentTimeLabel.setText("00:00", juce::dontSendNotification);
    currentTimeLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(currentTimeLabel);

    timeSlider.onDragStart = [this, &timeSlider]() { isUserDraggingSlider = true; };
    timeSlider.onDragEnd = [this, &timeSlider]()
        {
            isUserDraggingSlider = false;
            audio.getTransportSource().setPosition(timeSlider.getValue());
        };

    auto& speedSlider = audio.getSpeedSlider();
    speedSlider.setRange(0.25, 3.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setEnabled(false);
    speedSlider.setColour(juce::Slider::trackColourId, accentBlue.darker(0.3f));
    speedSlider.setColour(juce::Slider::backgroundColourId, accentBlue.darker(0.3f));
    speedSlider.setColour(juce::Slider::thumbColourId, accentYellow);
    addAndMakeVisible(speedSlider);

    auto& speedLabel = audio.getSpeedLabel();
    speedLabel.setText("Speed: 1.00x", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centredRight);
    speedLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(speedLabel);

    speedSlider.onValueChange = [this, &speedSlider, &speedLabel]() {
        double newSpeed = speedSlider.getValue();
        audio.setSpeedValue(newSpeed);
        speedLabel.setText("Speed: " + juce::String(newSpeed, 2) + "x", juce::dontSendNotification);
        };

    // --- Player 2 Sliders & Labels ---
    auto& timeSlider2 = audio.getTimeSlider2();
    timeSlider2.setSliderStyle(juce::Slider::LinearHorizontal);
    timeSlider2.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    timeSlider2.setEnabled(false);
    timeSlider2.setColour(juce::Slider::trackColourId, accentBlue.darker(0.3f));
    timeSlider2.setColour(juce::Slider::backgroundColourId, accentBlue.darker(0.3f));
    timeSlider2.setColour(juce::Slider::thumbColourId, accentYellow);
    addAndMakeVisible(timeSlider2);

    auto& currentTimeLabel2 = audio.getCurrentTimeLabel2();
    currentTimeLabel2.setJustificationType(juce::Justification::centredRight);
    currentTimeLabel2.setFont(juce::Font{ 16.0f, juce::Font::bold });
    currentTimeLabel2.setText("00:00", juce::dontSendNotification);
    currentTimeLabel2.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(currentTimeLabel2);

    timeSlider2.onDragStart = [this, &timeSlider2]() { isUserDraggingSlider2 = true; };
    timeSlider2.onDragEnd = [this, &timeSlider2]()
        {
            isUserDraggingSlider2 = false;
            audio.getTransportSource2().setPosition(timeSlider2.getValue());
        };

    auto& speedSlider2 = audio.getSpeedSlider2();
    speedSlider2.setRange(0.25, 3.0, 0.01);
    speedSlider2.setValue(1.0);
    speedSlider2.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider2.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider2.setEnabled(false);
    speedSlider2.setColour(juce::Slider::trackColourId, accentBlue.darker(0.3f));
    speedSlider2.setColour(juce::Slider::backgroundColourId, accentBlue.darker(0.3f));
    speedSlider2.setColour(juce::Slider::thumbColourId, accentYellow);
    addAndMakeVisible(speedSlider2);

    auto& speedLabel2 = audio.getSpeedLabel2();
    speedLabel2.setText("Speed: 1.00x", juce::dontSendNotification);
    speedLabel2.setJustificationType(juce::Justification::centredRight);
    speedLabel2.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(speedLabel2);

    speedSlider2.onValueChange = [this, &speedSlider2, &speedLabel2]() {
        double newSpeed = speedSlider2.getValue();
        audio.setSpeedValue2(newSpeed);
        speedLabel2.setText("Speed: " + juce::String(newSpeed, 2) + "x", juce::dontSendNotification);
        };

    // --- Keyboard Focus & Timer ---
    setWantsKeyboardFocus(true);
    addKeyListener(this);
    startTimer(30); // 30ms timer for smooth updates
}


void PlayerGUI::paint(juce::Graphics& g)
{
    // --- Main Background ---
    g.fillAll(bgColour);

    // --- Draw Player 1 Background ---
    g.setColour(playerBgColour);
    g.fillRoundedRectangle(playerArea1.toFloat(), 10.0f);

    // --- Draw Player 2 Background ---
    g.setColour(playerBgColour);
    g.fillRoundedRectangle(playerArea2.toFloat(), 10.0f);

    // --- Draw Waveforms ---
    audio.paintWaveform(g, waveformArea);
    audio.paintWaveform2(g, waveformArea2);

    // --- Draw Track Info ---
    g.setColour(textColour);
    g.setFont(juce::Font{ 14.0f });

    juce::String text1 = "Title: " + audio.getTrackMeta(1).title +
        " | Artist: " + audio.getTrackMeta(1).artist +
        " | Album: " + audio.getTrackMeta(1).album;
    juce::String text2 = "Title: " + audio.getTrackMeta(2).title +
        " | Artist: " + audio.getTrackMeta(2).artist +
        " | Album: " + audio.getTrackMeta(2).album;

    g.drawText(text1, infoArea1, juce::Justification::centredLeft, true);
    g.drawText(text2, infoArea2, juce::Justification::centredLeft, true);


    // --- Playlist 1 ---
    g.setColour(textColour);
    g.setFont(juce::Font{ 18.0f, juce::Font::bold });
    g.drawText("Playlist 1", playlistArea.getX(), playlistArea.getY() - 30,
        playlistArea.getWidth(), 30, juce::Justification::left);

    g.setFont(juce::Font{ 14.0f });
    int yPos = playlistArea.getY();
    for (int i = 0; i < audio.getPlaylistSize(); ++i)
    {
        juce::String trackName = audio.getTrackName(i);
        bool isPlaying = (i == audio.getCurrentTrackIndex());
        bool isSelected = (i == audio.getSelectedTrackIndex());

        juce::Rectangle<int> trackRect(playlistArea.getX(), yPos, playlistArea.getWidth(), 22);

        if (isPlaying)
        {
            g.setColour(accentPink); // Playing color
            g.fillRoundedRectangle(trackRect.toFloat().reduced(0, 1), 5.0f);
            g.setColour(darkTextColour); // Text on playing
        }
        else if (isSelected)
        {
            g.setColour(accentPurple); // Selected color
            g.fillRoundedRectangle(trackRect.toFloat().reduced(0, 1), 5.0f);
            g.setColour(darkTextColour); // Text on selected
        }
        else
        {
            g.setColour(textColour); // Normal text
        }

        g.drawText(juce::String(i + 1) + ". " + trackName,
            trackRect.reduced(8, 0),
            juce::Justification::left, true);

        yPos += 24; // Add a little spacing
    }

    // --- Playlist 2 ---
    g.setColour(textColour);
    g.setFont(juce::Font{ 18.0f, juce::Font::bold });
    g.drawText("Playlist 2", playlistArea2.getX(), playlistArea2.getY() - 30,
        playlistArea2.getWidth(), 30, juce::Justification::left);

    g.setFont(juce::Font{ 14.0f });
    int yPos2 = playlistArea2.getY();
    for (int i = 0; i < audio.getPlaylist2Size(); ++i)
    {
        juce::String trackName2 = audio.getTrackName2(i);
        bool isPlaying2 = (i == audio.getCurrentTrackIndex2());
        bool isSelected2 = (i == audio.getSelectedTrackIndex2());

        juce::Rectangle<int> trackRect2(playlistArea2.getX(), yPos2, playlistArea2.getWidth(), 22);

        if (isPlaying2)
        {
            g.setColour(accentPink); // Playing color
            g.fillRoundedRectangle(trackRect2.toFloat().reduced(0, 1), 5.0f);
            g.setColour(darkTextColour); // Text on playing
        }
        else if (isSelected2)
        {
            g.setColour(accentPurple); // Selected color
            g.fillRoundedRectangle(trackRect2.toFloat().reduced(0, 1), 5.0f);
            g.setColour(darkTextColour); // Text on selected
        }
        else
        {
            g.setColour(textColour); // Normal text
        }

        g.drawText(juce::String(i + 1) + ". " + trackName2,
            trackRect2.reduced(8, 0),
            juce::Justification::left, true);

        yPos2 += 24;
    }

    bool p1Loaded = (audio.getReaderSource() != nullptr);
    playPauseButton.setEnabled(p1Loaded);
    gostartButton.setEnabled(p1Loaded);
    endButton.setEnabled(p1Loaded);
    A_SegButton.setEnabled(p1Loaded);
    B_SegButton.setEnabled(p1Loaded);
    AB_SegButton.setEnabled(p1Loaded);

    bool p2Loaded = (audio.getReaderSource2() != nullptr);
    playPauseButton2.setEnabled(p2Loaded);
    gostartButton2.setEnabled(p2Loaded);
    endButton2.setEnabled(p2Loaded);
    A_SegButton2.setEnabled(p2Loaded);
    B_SegButton2.setEnabled(p2Loaded);
    AB_SegButton2.setEnabled(p2Loaded);

    muteButton.setButtonText(audio.ismuted() ? "Unmute" : "Mute");
    muteButton2.setButtonText(audio.ismuted2() ? "Unmute" : "Mute");
	playPauseButton.setButtonText(audio.getTransportSource().isPlaying() ? "Pause" : "Play");
	playPauseButton2.setButtonText(audio.getTransportSource2().isPlaying() ? "Pause" : "Play");
    loopButton.setButtonText(audio.islooping() ? "Unloop" : "Loop");
    loopButton2.setButtonText(audio.islooping2() ? "Unloop" : "Loop");
	AB_SegButton.setButtonText(audio.getSegLooping() ? "Unloop A-B" : "A-B Looping");
	AB_SegButton2.setButtonText(audio.getSegLooping2() ? "Unloop A-B" : "A-B Looping");
	A_SegButton.setButtonText(audio.hasASeg(1) ? "Delete A" : "Set A");
	B_SegButton.setButtonText(audio.hasBSeg(1) ? "Delete B" : "Set B");
	A_SegButton2.setButtonText(audio.hasASeg(2) ? "Delete A" : "Set A");
	B_SegButton2.setButtonText(audio.hasBSeg(2) ? "Delete B" : "Set B");
}

void PlayerGUI::resized()
{
    // --- Overall Layout ---
    setSize(getWidth(), getHeight());
    int margin = 20;
    int playerWidth = (getWidth() - 3 * margin) / 2;
    int playerHeight = getHeight() - (2 * margin);

    // --- Player 1 Areas ---
    playerArea1.setBounds(margin, margin, playerWidth, playerHeight);
    auto& p1 = playerArea1; // Alias for easier use
    int p1ContentX = p1.getX() + margin;
    int p1ContentWidth = p1.getWidth() - (2 * margin);
    int p1Y = p1.getY() + margin;

    int buttonHeight = 35;
    int buttonWidth = (p1ContentWidth - 20) / 3; // 3 buttons per row
    int space = 10;

    // P1: Row 1
    loadButton.setBounds(p1ContentX, p1Y, p1ContentWidth, buttonHeight);
    loadButton.setButtonText("Load Track 1");
    p1Y += buttonHeight + space;

    // P1: Row 2
    playPauseButton.setBounds(p1ContentX, p1Y, buttonWidth, buttonHeight);
    addTrackButton.setBounds(p1ContentX + buttonWidth + space, p1Y, buttonWidth, buttonHeight);
    gostartButton.setBounds(p1ContentX + 2 * (buttonWidth + space), p1Y, buttonWidth, buttonHeight);
    p1Y += buttonHeight + space;

    // P1: Row 3
    endButton.setBounds(p1ContentX, p1Y, buttonWidth, buttonHeight);
    loopButton.setBounds(p1ContentX + buttonWidth + space, p1Y, buttonWidth, buttonHeight);
    muteButton.setBounds(p1ContentX + 2 * (buttonWidth + space), p1Y, buttonWidth, buttonHeight);
    p1Y += buttonHeight + space; 

    A_SegButton.setBounds(p1ContentX, p1Y, buttonWidth, buttonHeight);
    B_SegButton.setBounds(p1ContentX + buttonWidth + space, p1Y, buttonWidth, buttonHeight);
    AB_SegButton.setBounds(p1ContentX + 2 * (buttonWidth + space), p1Y, buttonWidth, buttonHeight);
    p1Y += buttonHeight + space + margin; // Extra margin

    // P1: Waveform & Info
    waveformArea.setBounds(p1ContentX, p1Y, p1ContentWidth, 100); // Taller waveform
    p1Y += 100 + space;

    infoArea1.setBounds(p1ContentX, p1Y, p1ContentWidth - 70, 25);
    audio.getCurrentTimeLabel().setBounds(p1ContentX + p1ContentWidth - 60, p1Y, 60, 25);
    p1Y += 25 + space;

    audio.getTimeSlider().setBounds(p1ContentX, p1Y, p1ContentWidth, 25);
    p1Y += 25 + space;

    // P1: Speed
    audio.getSpeedLabel().setBounds(p1ContentX, p1Y, 100, 25);
    audio.getSpeedSlider().setBounds(p1ContentX + 100 + space, p1Y, p1ContentWidth - 100 - space, 25);
    p1Y += 25 + margin;

    // P1: Playlist
    playlistArea.setBounds(p1ContentX, p1Y, p1ContentWidth, p1.getBottom() - p1Y - margin);

    // --- Player 2 Areas ---
    playerArea2.setBounds(playerArea1.getRight() + margin, margin, playerWidth, playerHeight);
    auto& p2 = playerArea2; // Alias
    int p2ContentX = p2.getX() + margin;
    int p2ContentWidth = p2.getWidth() - (2 * margin);
    int p2Y = p2.getY() + margin;

    // P2: Row 1
    loadButton2.setBounds(p2ContentX, p2Y, p2ContentWidth, buttonHeight);
    loadButton2.setButtonText("Load Track 2");
    p2Y += buttonHeight + space;

    // P2: Row 2
    playPauseButton2.setBounds(p2ContentX, p2Y, buttonWidth, buttonHeight);
    addTrackButton2.setBounds(p2ContentX + buttonWidth + space, p2Y, buttonWidth, buttonHeight);
    gostartButton2.setBounds(p2ContentX + 2 * (buttonWidth + space), p2Y, buttonWidth, buttonHeight);
    p2Y += buttonHeight + space;

    // P2: Row 3
    endButton2.setBounds(p2ContentX, p2Y, buttonWidth, buttonHeight);
    loopButton2.setBounds(p2ContentX + buttonWidth + space, p2Y, buttonWidth, buttonHeight);
    muteButton2.setBounds(p2ContentX + 2 * (buttonWidth + space), p2Y, buttonWidth, buttonHeight);
    p2Y += buttonHeight + space;

    A_SegButton2.setBounds(p2ContentX, p2Y, buttonWidth, buttonHeight);
    B_SegButton2.setBounds(p2ContentX + buttonWidth + space, p2Y, buttonWidth, buttonHeight);
    AB_SegButton2.setBounds(p2ContentX + 2 * (buttonWidth + space), p2Y, buttonWidth, buttonHeight);
    p2Y += buttonHeight + space + margin; // Extra margin

    // P2: Waveform & Info
    waveformArea2.setBounds(p2ContentX, p2Y, p2ContentWidth, 100); // Taller waveform
    p2Y += 100 + space;

    infoArea2.setBounds(p2ContentX, p2Y, p2ContentWidth - 70, 25);
    audio.getCurrentTimeLabel2().setBounds(p2ContentX + p2ContentWidth - 60, p2Y, 60, 25);
    p2Y += 25 + space;

    audio.getTimeSlider2().setBounds(p2ContentX, p2Y, p2ContentWidth, 25);
    p2Y += 25 + space;

    // P2: Speed
    audio.getSpeedLabel2().setBounds(p2ContentX, p2Y, 100, 25);
    audio.getSpeedSlider2().setBounds(p2ContentX + 100 + space, p2Y, p2ContentWidth - 100 - space, 25);
    p2Y += 25 + margin;

    // P2: Playlist
    playlistArea2.setBounds(p2ContentX, p2Y, p2ContentWidth, p2.getBottom() - p2Y - margin);
}


void PlayerGUI::timerCallback()
{
    if (!isUserDraggingSlider && !isUserDraggingSlider2)
        audio.timeCollection();

    repaint();

    if (isShowing())
    {
        grabKeyboardFocus();
    }
}

void PlayerGUI::mouseDown(const juce::MouseEvent& event)
{
    juce::Point<int> clickPos = event.getPosition();
    int trackHeight = 24;

    if (playlistArea.contains(clickPos))
    {
        int trackIndex = (clickPos.y - playlistArea.getY()) / trackHeight;
        if (trackIndex >= 0 && trackIndex < audio.getPlaylistSize())
        {
            audio.setSelectedTrack(trackIndex);
            repaint();
        }
    }
    else if (playlistArea2.contains(clickPos))
    {
        int trackIndex2 = (clickPos.y - playlistArea2.getY()) / trackHeight;
        if (trackIndex2 >= 0 && trackIndex2 < audio.getPlaylist2Size())
        {
            audio.setSelectedTrack2(trackIndex2);
            repaint();
        }
    }
}

void PlayerGUI::mouseDoubleClick(const juce::MouseEvent& event)
{
    juce::Point<int> clickPos = event.getPosition();
    int trackHeight = 24;

    if (playlistArea.contains(clickPos))
    {
        int trackIndex = (clickPos.y - playlistArea.getY()) / trackHeight;

        if (trackIndex >= 0 && trackIndex < audio.getPlaylistSize())
        {
            audio.setSelectedTrack(trackIndex);
            audio.loadTrack(trackIndex);
            audio.playSelectedTrack(audio.getTransportSource(), 1);
            repaint();
        }
    }
    else if (playlistArea2.contains(clickPos))
    {
        int trackIndex2 = (clickPos.y - playlistArea2.getY()) / trackHeight;
        if (trackIndex2 >= 0 && trackIndex2 < audio.getPlaylist2Size())
        {
            audio.setSelectedTrack2(trackIndex2);
            audio.loadTrack2(trackIndex2);
            audio.playSelectedTrack(audio.getTransportSource2(), 2);
            repaint();
        }
    }
}

bool PlayerGUI::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
    int keyCode = key.getKeyCode();
    bool ctrl = key.getModifiers().isCtrlDown(); // check if Ctrl is pressed

    if (keyCode == juce::KeyPress::spaceKey)
    {
        if (!ctrl)
        {
            // Toggle pause for Track 1
            audio.pauseaudio(audio.getTransportSource());
        }
        else
        {
            // Toggle pause for Track 2
            audio.pauseaudio(audio.getTransportSource2());
        }
        return true;
    }
    else if (keyCode == KeyPress::rightKey) {
        if (!ctrl)
        {
            // Toggle pause for Track 1
            double position_now = audio.getTransportSource().getCurrentPosition();
            if (position_now + 5 <= audio.getTransportSource().getLengthInSeconds()) {
                audio.getTransportSource().setPosition(position_now + 5);
            }
            else
                audio.getTransportSource().setPosition(audio.getTransportSource().getLengthInSeconds());
        }
        else
        {
            // Toggle pause for Track 2
            double position_now = audio.getTransportSource2().getCurrentPosition();
            if (position_now + 5 <= audio.getTransportSource2().getLengthInSeconds()) {
                audio.getTransportSource2().setPosition(position_now + 5);
            }
            else
                audio.getTransportSource2().setPosition(audio.getTransportSource2().getLengthInSeconds());
        }
        return true;
    }
    else if (keyCode == KeyPress::leftKey) {
        if (!ctrl)
        {
            // Toggle pause for Track 1
            double position_now = audio.getTransportSource().getCurrentPosition();
            if (position_now - 5 >= 0) {
                audio.getTransportSource().setPosition(position_now - 5);
            }
            else
                audio.getTransportSource().setPosition(0);
        }
        else
        {
            // Toggle pause for Track 2
            double position_now = audio.getTransportSource2().getCurrentPosition();
            if (position_now - 5 >= 0) {
                audio.getTransportSource2().setPosition(position_now - 5);
            }
            else
                audio.getTransportSource2().setPosition(0);
        }
        return true;
    }

    switch (tolower(keyCode))
    {
    case 'd': // Load
        if (!ctrl)
        {
            // Load Track 1
            audio.loadAudio();
        }
        else
        {
            // Load Track 2
            audio.loadAudio2();
        }
        return true;
    case 'p':
        if (!ctrl)
        {
            // Play Track 1
            audio.playSelectedTrack(audio.getTransportSource(), 1);
        }
        else
        {
            // Play Track 2
            audio.playSelectedTrack(audio.getTransportSource2(), 2);
        }
        return true;
    case 's': // Go To Start
        if (!ctrl)
        {
            // Go To Start Track 1
            audio.goToStart(audio.getTransportSource());
        }
        else
        {
            // Go To Start Track 2
            audio.goToStart(audio.getTransportSource2());
        }
        return true;
    case 'm': //Mute
        if (!ctrl)
        {
            // Mute Track 1
            audio.toggleMute();
        }
        else
        {
            // Mute Track 2
            audio.toggleMute2();
        }
        return true;
    case 'l': // Loop
        if (!ctrl)
        {
            // Loop Track 1
            audio.toggleLoop();
        }
        else
        {
            // Loop Track 2
            audio.toggleLoop2();
        }
        return true;
    case ']':
    {
        if (!ctrl)
        {
            // Control Track 1
            double faster = audio.getSpeedSlider().getValue();
            audio.getSpeedSlider().setValue(faster + 0.1);
        }
        else
        {
            // Control Track 2
            double faster = audio.getSpeedSlider2().getValue();
            audio.getSpeedSlider2().setValue(faster + 0.1);
        }
        return true;
    }
    case '[':
    {
        if (!ctrl)
        {
            // Control Track 1
            double slower = audio.getSpeedSlider().getValue();
            audio.getSpeedSlider().setValue(slower - 0.1);
        }
        else
        {
            // Control Track 2
            double slower = audio.getSpeedSlider2().getValue();
            audio.getSpeedSlider2().setValue(slower - 0.1);
        }
        return true;
    }
    case 'e':
        if (!ctrl)
        {
            // Go To End Track 1
            audio.goToEnd(audio.getTransportSource(), audio.getReaderSource());
        }
        else
        {
            // Go To End Track 2
            audio.goToEnd(audio.getTransportSource2(), audio.getReaderSource2());
        }
        return true;
    case 'r':
        if (!ctrl)
        {
            // Reset Speed Track 1
            audio.getSpeedSlider().setValue(1.00);
        }
        else
        {
            // Reset Speed Track 2
            audio.getSpeedSlider2().setValue(1.00);
        }
        return true;

    case 'a': // Set/Delete A Segment
        if (!ctrl)
        {
            bool hasA = audio.hasASeg(1);
            audio.A_Seg(1, !hasA);
        }
        else
        {
            bool hasA2 = audio.hasASeg(2);
            audio.A_Seg(2, !hasA2);
        }
        return true;

    case 'b': // Set/Delete B Segment
        if (!ctrl)
        {
            bool hasB = audio.hasBSeg(1);
            audio.B_Seg(1, !hasB);
        }
        else
        {
            bool hasB2 = audio.hasBSeg(2);
            audio.B_Seg(2, !hasB2);
        }
        return true;

    case 'k': // Toggle A-B Segment Looping
        if (!ctrl)
        {
            audio.toggleSegLooping(1);
        }
        else
        {
            audio.toggleSegLooping(2);
        }
        return true;
    }
    return false; // unhandled key
}
