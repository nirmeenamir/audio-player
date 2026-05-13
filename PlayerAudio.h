#pragma once
#include <JuceHeader.h>
#include <vector>

using namespace juce;
using namespace std;

//==============================================================
// Struct to hold metadata (globally visible)
//==============================================================
struct AudioMetaData
{
    juce::String title;
    juce::String artist;
    juce::String album;
};

//==============================================================
// PlayerAudio Class
//==============================================================
class PlayerAudio : public juce::AudioAppComponent,
    public juce::ChangeListener
{
public:
    //==========================================================
    // Constructor / Destructor
    //==========================================================
    PlayerAudio();
    ~PlayerAudio() override;

    //==========================================================
    // JUCE Overrides
    //==========================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    //==========================================================
    // Core Player Functions
    //==========================================================
    void loadAudio(bool what = true);
    void loadAudio2(bool what = true);
    void loadTrack(int trackIndex);
    void loadTrack2(int trackIndex);
    void playAudio(juce::AudioTransportSource& trans);
    void pauseaudio(juce::AudioTransportSource& trans);
    void togglePlayPause(juce::AudioTransportSource& trans, int playerID);
    void goToStart(juce::AudioTransportSource& trans);
    void goToEnd(juce::AudioTransportSource& trans, std::unique_ptr<juce::AudioFormatReaderSource>& rdrsrc);

    void playSelectedTrack(juce::AudioTransportSource& trans, int playerID)
    {
        if (playerID == 1)
        {
            if (selectedTrackIndex != currentTrackIndex)
                loadTrack(selectedTrackIndex);
        }
        else if (playerID == 2)
        {
            if (selectedTrackIndex2 != currentTrackIndex2)
                loadTrack2(selectedTrackIndex2);
        }
        playAudio(trans);
    }

    //==========================================================
    // Looping / Segment Handling
    //==========================================================
    void toggleLoop();
    void toggleLoop2();
    void toggleSegLooping(int PlayerID);
    void A_Seg(int PlayerID, bool seg = true);
    void B_Seg(int PlayerID, bool seg = true);
    void AB_Seg_Looping(juce::AudioTransportSource& trans, bool shouldLoop,
        std::unique_ptr<juce::AudioFormatReaderSource>& reader);

    //==========================================================
    // Audio State & Control
    //==========================================================
    void toggleMute();
    void toggleMute2();
    void setSpeedValue(double speedValue);
    void setSpeedValue2(double newSpeedValue);
    void sliderValueChanged();
    void timeCollection();

    //==========================================================
    // Metadata
    //==========================================================
    void MetaData(File& slcFile, int PlayerID);
    AudioMetaData getTrackMeta(int playerID) const
    {
        return (playerID == 1) ? trackMeta1 : trackMeta2;
    }

    //==========================================================
    // Getters (Audio Transport & Properties)
    //==========================================================
    juce::AudioTransportSource& getTransportSource() { return transportSource; }
    juce::AudioTransportSource& getTransportSource2() { return transportSource2; }

    std::unique_ptr<juce::AudioFormatReaderSource>& getReaderSource() { return readerSource; }
    std::unique_ptr<juce::AudioFormatReaderSource>& getReaderSource2() { return readerSource2; }

    juce::AudioThumbnail& getAudioThumbnail() { return audioThumbnail; }
    juce::AudioThumbnail& getAudioThumbnail2() { return audioThumbnail2; }

    double getSpeedValue() const { return speedValue; }
    bool getSegLooping() const { return segLooping; }
    bool getSegLooping2() const { return segLooping2; }
    bool ismuted() const { return isMuted; }
    bool ismuted2() const { return isMuted2; }
    bool islooping() const { return isLooping; }
    bool islooping2() const { return isLooping2; }

    bool hasASeg(int PlayerID) const { return (PlayerID == 1) ? ASeg : A2Seg; }
    bool hasBSeg(int PlayerID) const { return (PlayerID == 1) ? BSeg : B2Seg; }
    bool getASeg() const { return ASeg; }
    bool getBSeg() const { return BSeg; }
    bool getA2Seg() const { return A2Seg; }
    bool getB2Seg() const { return B2Seg; }

    //==========================================================
    // Time, Duration & Playlist Info
    //==========================================================
    int getDurationInSeconds() const { return static_cast<int>(Duration); }
    int getCurrentTrackIndex() const { return currentTrackIndex; }
    int getCurrentTrackIndex2() const { return currentTrackIndex2; }

    int getPlaylistSize() const { return static_cast<int>(playlist.size()); }
    int getPlaylist2Size() const { return static_cast<int>(playlist2.size()); }

    juce::String getTrackName(int index) const
    {
        if (index >= 0 && index < playlist.size())
            return playlist[index].getFileName();
        return "Invalid Track";
    }

    juce::String getTrackName2(int index) const
    {
        if (index >= 0 && index < playlist2.size())
            return playlist2[index].getFileName();
        return "Invalid Track";
    }

    void setSelectedTrack(int index) { selectedTrackIndex = index; }
    void setSelectedTrack2(int index) { selectedTrackIndex2 = index; }

    int getSelectedTrackIndex() const { return selectedTrackIndex; }
    int getSelectedTrackIndex2() const { return selectedTrackIndex2; }

    //==========================================================
    // Time Formatting
    //==========================================================
    juce::String formatTime(double seconds);
    juce::String formatTime2(double seconds);

    int getMinutes() const { return minutes; }
    int getSeconds() const { return seconds; }
    int getMinutes2() const { return minutes2; }
    int getSeconds2() const { return seconds2; }
    int getRM2() const { return rM2; }
    int getRS2() const { return rS2; }

    //==========================================================
    // Sliders & Labels
    //==========================================================
    juce::Slider& getTimeSlider() { return timeSlider; }
    juce::Slider& getSpeedSlider() { return speedSlider; }
    juce::Label& getCurrentTimeLabel() { return currentTimeLabel; }
    juce::Label& getSpeedLabel() { return speedLabel; }

    juce::Slider& getTimeSlider2() { return timeSlider2; }
    juce::Slider& getSpeedSlider2() { return speedSlider2; }
    juce::Label& getCurrentTimeLabel2() { return currentTimeLabel2; }
    juce::Label& getSpeedLabel2() { return speedLabel2; }

    //==========================================================
    // Rendering
    //==========================================================
    void paintWaveform(juce::Graphics& g, juce::Rectangle<int> area);
    void paintWaveform2(juce::Graphics& g, juce::Rectangle<int> area);

private:
    //==========================================================
    // Internal Data
    //==========================================================
    AudioMetaData trackMeta1, trackMeta2;

    // Track indexing and time values
    int minutes = 0, seconds = 0, rM = 0, rS = 0;
    int minutes2 = 0, seconds2 = 0, rM2 = 0, rS2 = 0;
    int currentTrackIndex = 0, selectedTrackIndex = 0;
    int currentTrackIndex2 = 0, selectedTrackIndex2 = 0;

    // Durations and speed
    double Duration = 0.0, Duration2 = 0.0;
    double speedValue = 1.0, speedValue2 = 1.0;
    double currentASeg = -1.0, currentBSeg = -1.0;
    double currentASeg2 = -1.0, currentBSeg2 = -1.0;

    // State flags
    bool isLooping = false, isMuted = false, segLooping = false, ASeg = false, BSeg = false;
    bool isLooping2 = false, isMuted2 = false, segLooping2 = false, A2Seg = false, B2Seg = false;

    // Playlist and files
    std::vector<juce::File> playlist, playlist2;
    juce::File selectedFile, selectedFile2;

    // Audio infrastructure
    juce::AudioFormatManager formatManager;
    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioSourcePlayer sourcePlayer;

    // Transport & readers
    juce::AudioTransportSource transportSource, transportSource2;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource, readerSource2;
    std::unique_ptr<juce::FileChooser> chooser;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource, resamplingSource2;

    // Mixer
    juce::MixerAudioSource mixerSource1, mixerSource2, masterMixer;

    // Thumbnails
    juce::AudioThumbnailCache thumbnailCache{ 10 }, thumbnailCache2{ 10 };
    juce::AudioThumbnail audioThumbnail{ 512, formatManager, thumbnailCache };
    juce::AudioThumbnail audioThumbnail2{ 512, formatManager, thumbnailCache2 };

    // UI elements
    juce::StringArray metadatalines;
    juce::String audioFileName, audioFileName2;
    juce::Slider timeSlider, speedSlider, timeSlider2, speedSlider2;
    juce::Label currentTimeLabel, speedLabel, currentTimeLabel2, speedLabel2;

    // Track metadata
    juce::String trackTitle, trackArtist, trackAlbum;
    juce::String trackTitle2, trackArtist2, trackAlbum2;

    //==========================================================
    // Helpers
    //==========================================================
    void Looping(juce::AudioTransportSource& transport,
        bool isLooping,
        std::unique_ptr<juce::AudioFormatReaderSource>& reader);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
