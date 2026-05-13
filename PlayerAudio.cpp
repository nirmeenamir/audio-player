#include "PlayerAudio.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>

const juce::Colour playerBgColour{ 0xFF3E3C5F }; // Lighter purple for player sections
const juce::Colour accentYellow{ 0xFFFBF4B1 };   // Sliders, highlights
const juce::Colour accentPink{ 0xFFF9B9DE };     // Buttons, Playhead
const juce::Colour textColour{ 0xFFF0F0F0 };     // Main text


PlayerAudio::PlayerAudio()
    : thumbnailCache(5),
    audioThumbnail(512, formatManager, thumbnailCache),
    thumbnailCache2(5),
    audioThumbnail2(512, formatManager, thumbnailCache2)
{
    formatManager.registerBasicFormats();

    // This ensures they have a valid value before prepareToPlay() is called.
    speedSlider.setRange(0.25, 3.0, 0.01);
    speedSlider.setValue(1.0);

    speedSlider2.setRange(0.25, 3.0, 0.01);
    speedSlider2.setValue(1.0);
    // --- End of Fix ---

    // resamplers feeding transports (owned by this object)
    resamplingSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false);
    resamplingSource2 = std::make_unique<juce::ResamplingAudioSource>(&transportSource2, false);

    // per-player mixers
    mixerSource1.addInputSource(resamplingSource.get(), false);
    mixerSource2.addInputSource(resamplingSource2.get(), false);

    // master mixer combines both players
    masterMixer.addInputSource(&mixerSource1, false);
    masterMixer.addInputSource(&mixerSource2, false);

    // thumbnails
    audioThumbnail.addChangeListener(this);
    audioThumbnail2.addChangeListener(this);

    setAudioChannels(0, 2);
}



PlayerAudio::~PlayerAudio()
{
    shutdownAudio();
}
void PlayerAudio::changeListenerCallback(juce::ChangeBroadcaster* source)
{

    if (source == &audioThumbnail || source == &audioThumbnail2)
    {
        // This is where you would trigger a repaint on the GUI
        // if it was registered as a listener.
        // For simplicity, the GUI's timer is handling repaints.
    }
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    transportSource2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    if (resamplingSource)   resamplingSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
    if (resamplingSource2)  resamplingSource2->prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    masterMixer.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Set initial speed and gain
    // This is now safe because sliders were initialized in the constructor
    resamplingSource->setResamplingRatio(speedSlider.getValue());
    resamplingSource2->setResamplingRatio(speedSlider2.getValue());
    transportSource.setGain(isMuted ? 0.0f : 1.0f);
    transportSource2.setGain(isMuted2 ? 0.0f : 1.0f);
}


void PlayerAudio::Looping(juce::AudioTransportSource& trans, bool shouldLoop, std::unique_ptr<juce::AudioFormatReaderSource>& reader)
{
    if (shouldLoop && reader != nullptr)
    {
        double pos = trans.getCurrentPosition();
        double len = trans.getLengthInSeconds();
        if (len > 0 && pos >= len - 0.01)
            trans.setPosition(0.0);
    }
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    // Use the member variable 'isLooping' from 'this' instance
    Looping(transportSource, this->isLooping, readerSource);
    Looping(transportSource2, this->isLooping2, readerSource2);

    AB_Seg_Looping(transportSource, this->segLooping, readerSource);
	AB_Seg_Looping(transportSource2, this->segLooping2, readerSource2);

    masterMixer.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
    transportSource2.releaseResources();

    if (resamplingSource)    resamplingSource->releaseResources();
    if (resamplingSource2)   resamplingSource2->releaseResources();

    mixerSource1.releaseResources();
    mixerSource2.releaseResources();
    masterMixer.releaseResources();
}


void PlayerAudio::setSpeedValue(double newSpeedValue)
{
    if (newSpeedValue < 0.25)
        newSpeedValue = 0.25;
    if (newSpeedValue > 3.0)
        newSpeedValue = 3.0;

    // Store the value
    speedValue = newSpeedValue;

    if (resamplingSource)
        resamplingSource->setResamplingRatio(speedValue);
}

void PlayerAudio::loadAudio(bool what)
{
    chooser = std::make_unique<juce::FileChooser>(
        "Select audio files...",
        juce::File{},
        "*.wav;*.mp3;*.aiff"
    );

    auto fileChooserFlags = juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectFiles |
        juce::FileBrowserComponent::canSelectMultipleItems;

    chooser->launchAsync(fileChooserFlags, [this, what](const juce::FileChooser& fc)
        {
            auto files = fc.getResults();

            if (files.isEmpty())
                return;

            if (what)
                playlist.clear();

            for (auto& file : files)
            {
                if (file.existsAsFile())
                    playlist.push_back(file);
            }

            if (!playlist.empty())
            {
                setSelectedTrack(0);
                loadTrack(0);
            }
        });
}

void PlayerAudio::MetaData(juce::File& slcFile, int playerID)
{
    auto* reader = formatManager.createReaderFor(slcFile);
    if (reader != nullptr)
    {
        double dur = static_cast<double>(reader->lengthInSamples) / reader->sampleRate;
        juce::String fileName = slcFile.getFileNameWithoutExtension();

        if (playerID == 1)
        {
            audioFileName = fileName;
            Duration = dur;
            minutes = static_cast<int>(dur) / 60;
            seconds = static_cast<int>(fmod(dur, 60));
            audioThumbnail.clear();
            audioThumbnail.setSource(new juce::FileInputSource(slcFile));
        }
        else if (playerID == 2)
        {
            audioFileName2 = fileName;
            Duration2 = dur;
            minutes2 = static_cast<int>(dur) / 60;
            seconds2 = static_cast<int>(fmod(dur, 60));
            audioThumbnail2.clear();
            audioThumbnail2.setSource(new juce::FileInputSource(slcFile));
        }

        delete reader;
    }

    // --- TagLib metadata ---
    TagLib::FileRef f(slcFile.getFullPathName().toStdString().c_str());
    if (!f.isNull() && f.tag())
    {
        TagLib::Tag* tag = f.tag();
        AudioMetaData meta;

        juce::String title = tag->title().toCString(true);
        juce::String artist = tag->artist().toCString(true);
        juce::String album = tag->album().toCString(true);

        meta.title = title.isNotEmpty() ? title : slcFile.getFileNameWithoutExtension();
        meta.artist = artist.isNotEmpty() ? artist : "Unknown Artist";
        meta.album = album.isNotEmpty() ? album : "Unknown Album";

        if (playerID == 1)
            trackMeta1 = meta;
        else if (playerID == 2)
            trackMeta2 = meta;
    }
}

void PlayerAudio::playAudio(juce::AudioTransportSource& trans)
{
    // Check if the corresponding reader source exists
    if (&trans == &transportSource && readerSource == nullptr) return;
    if (&trans == &transportSource2 && readerSource2 == nullptr) return;

    if (trans.getCurrentPosition() >= trans.getLengthInSeconds()) {
        trans.setPosition(0.0);
        trans.start();
        return;
    }
    if (trans.getLengthInSeconds() > 0.0)
        trans.start();
}

void PlayerAudio::pauseaudio(juce::AudioTransportSource& trans)
{
    if (trans.isPlaying())
        trans.stop();
    else
        playAudio(trans); // Use playAudio to handle checks
}

void PlayerAudio::togglePlayPause(juce::AudioTransportSource& trans, int playerID)
{
    if (trans.isPlaying())
    {
        pauseaudio(trans);
    }
    else
    {
        playSelectedTrack(trans, playerID);
    }
}

void PlayerAudio::toggleLoop()
{
    isLooping = !isLooping;
}
void PlayerAudio::goToStart(juce::AudioTransportSource& trans) {
    trans.setPosition(0.0);
    // Let's not auto-play, just go to start. User can press play.
    // trans.start();
}
void PlayerAudio::goToEnd(juce::AudioTransportSource& trans, unique_ptr<juce::AudioFormatReaderSource>& rdrsrc) {
    if (rdrsrc.get() != nullptr) {
        double endPosition = trans.getLengthInSeconds() * 0.99;
        trans.setPosition(endPosition);
    }
}

void PlayerAudio::toggleMute()
{
    isMuted = !isMuted;
    transportSource.setGain(isMuted ? 0.0f : 1.0f);
}

void PlayerAudio::loadTrack(int trackIndex)
{
    if (trackIndex < 0 || trackIndex >= playlist.size())
    {
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();
        Duration = 0.0;
        timeSlider.setEnabled(false);
        speedSlider.setEnabled(false);
        audioFileName = "";
        return;
    }

    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();

    selectedFile = playlist[trackIndex];
    currentTrackIndex = trackIndex;

    // create a reader (raw pointer owned by unique_ptr)
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(selectedFile));
    if (reader != nullptr)
    {
        // capture sample rate BEFORE releasing the reader
        double sr = reader->sampleRate;

        // transfer ownership of the reader into readerSource
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader.release(), true);
        transportSource.setSource(readerSource.get(), 0, nullptr, sr);

        // update waveform and UI
        MetaData(selectedFile, 1); // This now handles the thumbnail

        Duration = static_cast<double>(transportSource.getLengthInSeconds());
        timeSlider.setRange(0.0, Duration, 0.01);
        timeSlider.setValue(0.0, juce::dontSendNotification);
        timeSlider.setEnabled(true);
        speedSlider.setEnabled(true);
        // Set speed from slider value, not default
        setSpeedValue(speedSlider.getValue());
    }
    else
    {
        Duration = 0.0;
        timeSlider.setEnabled(false);
        speedSlider.setEnabled(false);
        audioFileName = "Failed to load track";
    }
}



void PlayerAudio::timeCollection()
{
    // Update player 1
    if (readerSource != nullptr)
    {
        const double currentPosition = transportSource.getCurrentPosition();
        timeSlider.setValue(currentPosition, juce::dontSendNotification);
        currentTimeLabel.setText(formatTime(currentPosition), juce::dontSendNotification);
        rM = static_cast<int>(currentPosition / 60) % 60;
        rS = static_cast<int>(currentPosition) % 60;
    }

    // Update player 2
    if (readerSource2 != nullptr)
    {
        const double currentPosition2 = transportSource2.getCurrentPosition();
        timeSlider2.setValue(currentPosition2, juce::dontSendNotification);
        currentTimeLabel2.setText(formatTime(currentPosition2), juce::dontSendNotification);
        rM2 = static_cast<int>(currentPosition2 / 60) % 60;
        rS2 = static_cast<int>(currentPosition2) % 60;
    }
}

juce::String PlayerAudio::formatTime(double timeInSeconds)
{
    // Fix warning by using a different variable name
    int numMinutes = static_cast<int>(timeInSeconds) / 60;
    int numSecs = static_cast<int>(timeInSeconds) % 60;

    return juce::String::formatted("%02d:%02d", numMinutes, numSecs);
}

void PlayerAudio::paintWaveform(juce::Graphics& g, juce::Rectangle<int> area)
{
    // --- Marker Colors (distinct yet matching palette) ---
    const juce::Colour aMarkerColour = accentYellow.brighter(1.5f).withHue(0.48f);  // Bright aqua
    const juce::Colour bMarkerColour = accentPink.darker(0.4f).withHue(0.9f);       // Deep magenta

    // --- Background ---
    g.setColour(playerBgColour.brighter(0.1f));
    g.fillRoundedRectangle(area.toFloat(), 6.0f);

    g.setColour(playerBgColour.brighter(0.3f));
    g.drawRoundedRectangle(area.toFloat(), 6.0f, 1.0f);

    if (audioThumbnail.getNumChannels() > 0)
    {
        double audioLength = audioThumbnail.getTotalLength();

        if (transportSource.getLengthInSeconds() > 0.0)
        {
            // --- Draw the waveform ---
            g.setColour(accentYellow);
            audioThumbnail.drawChannel(g, area.reduced(2), 0.0, audioLength, 0, 1.0f);

            // --- Draw the playhead ---
            double currentPosition = transportSource.getCurrentPosition();
            double totalLength = transportSource.getLengthInSeconds();

            if (totalLength > 0.0)
            {
                float playheadPosition = (float)((currentPosition / totalLength) * area.getWidth());
                g.setColour(accentPink);
                g.drawLine(area.getX() + playheadPosition, (float)area.getY(),
                    area.getX() + playheadPosition, (float)area.getBottom(), 2.0f);

                juce::Path playheadTriangle;
                playheadTriangle.addTriangle(area.getX() + playheadPosition - 5, (float)area.getY(),
                    area.getX() + playheadPosition + 5, (float)area.getY(),
                    area.getX() + playheadPosition, (float)area.getY() + 10);
                g.fillPath(playheadTriangle);
            }
        }
        else if (audioThumbnail.isFullyLoaded())
        {
            g.setColour(accentYellow.withAlpha(0.5f));
            audioThumbnail.drawChannel(g, area.reduced(2), 0.0, audioLength, 0, 1.0f);
        }
        else
        {
            g.setColour(textColour);
            g.setFont(juce::Font(16.0f));
            g.drawFittedText("Generating waveform...", area, juce::Justification::centred, 1);
        }
    }
    else
    {
        g.setColour(textColour.withAlpha(0.7f));
        g.setFont(juce::Font(16.0f));
        g.drawFittedText("No audio loaded", area, juce::Justification::centred, 1);
    }

    // --- A/B Marker Drawing ---
    if (currentASeg >= 0.0 || currentBSeg >= 0.0)
    {
        double totalLength = transportSource.getLengthInSeconds();
        if (totalLength > 0.0)
        {
            auto timeToX = [&](double time)
                {
                    return (float)(area.getX() + (time / totalLength) * area.getWidth());
                };

            // --- A Marker ---
            if (currentASeg >= 0.0)
            {
                float xA = timeToX(currentASeg);
                g.setColour(aMarkerColour);
                g.drawLine(xA, (float)area.getY(), xA, (float)area.getBottom(), 2.0f);
                g.setColour(textColour);
                g.drawFittedText("A", juce::Rectangle<int>((int)xA - 8, area.getY() - 15, 16, 16),
                    juce::Justification::centred, 1);
            }

            // --- B Marker ---
            if (currentBSeg >= 0.0)
            {
                float xB = timeToX(currentBSeg);
                g.setColour(bMarkerColour);
                g.drawLine(xB, (float)area.getY(), xB, (float)area.getBottom(), 2.0f);
                g.setColour(textColour);
                g.drawFittedText("B", juce::Rectangle<int>((int)xB - 8, area.getY() - 15, 16, 16),
                    juce::Justification::centred, 1);
            }
        }
    }
}

void PlayerAudio::loadAudio2(bool what)
{
    chooser = std::make_unique<juce::FileChooser>(
        "Select audio files for Track 2...",
        juce::File{},
        "*.wav;*.mp3;*.aiff"
    );

    auto fileChooserFlags2 = juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectFiles |
        juce::FileBrowserComponent::canSelectMultipleItems;

    chooser->launchAsync(fileChooserFlags2, [this, what](const juce::FileChooser& fc)
        {
            auto files = fc.getResults();

            if (files.isEmpty())
                return;

            if (what)
                playlist2.clear();

            for (auto& file : files)
            {
                if (file.existsAsFile())
                    playlist2.push_back(file);
            }

            if (!playlist2.empty())
            {
                setSelectedTrack2(0);
                loadTrack2(0);
            }
        });
}

void PlayerAudio::loadTrack2(int trackIndex)
{
    if (trackIndex < 0 || trackIndex >= playlist2.size())
    {
        transportSource2.stop();
        transportSource2.setSource(nullptr);
        readerSource2.reset();
        Duration2 = 0.0;
        timeSlider2.setEnabled(false);
        speedSlider2.setEnabled(false);
        audioFileName2 = "";
        return;
    }

    transportSource2.stop();
    transportSource2.setSource(nullptr);
    readerSource2.reset();

    selectedFile2 = playlist2[trackIndex];
    currentTrackIndex2 = trackIndex;

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(selectedFile2));
    if (reader != nullptr)
    {
        double sr = reader->sampleRate;

        readerSource2 = std::make_unique<juce::AudioFormatReaderSource>(reader.release(), true);
        transportSource2.setSource(readerSource2.get(), 0, nullptr, sr);

        // update waveform 2 and UI
        MetaData(selectedFile2, 2); // This now handles the thumbnail

        Duration2 = static_cast<double>(transportSource2.getLengthInSeconds());
        timeSlider2.setRange(0.0, Duration2, 0.01);
        timeSlider2.setValue(0.0, juce::dontSendNotification);
        timeSlider2.setEnabled(true);
        speedSlider2.setEnabled(true);
        // Set speed from slider value
        setSpeedValue2(speedSlider2.getValue());
    }
    else
    {
        Duration2 = 0.0;
        timeSlider2.setEnabled(false);
        speedSlider2.setEnabled(false);
        audioFileName2 = "Failed to load track";
    }
}



void PlayerAudio::setSpeedValue2(double newSpeedValue)
{
    if (newSpeedValue < 0.25)
        newSpeedValue = 0.25;
    if (newSpeedValue > 3.0)
        newSpeedValue = 3.0;

    speedValue2 = newSpeedValue;

    if (resamplingSource2)
        resamplingSource2->setResamplingRatio(speedValue2);
}

void PlayerAudio::paintWaveform2(juce::Graphics& g, juce::Rectangle<int> area)
{
    // --- Marker Colors (consistent palette, high contrast) ---
    const juce::Colour aMarkerColour = accentYellow.brighter(1.5f).withHue(0.48f);  // Bright aqua
    const juce::Colour bMarkerColour = accentPink.darker(0.4f).withHue(0.9f);       // Deep magenta

    // --- Background ---
    g.setColour(playerBgColour.brighter(0.1f));
    g.fillRoundedRectangle(area.toFloat(), 6.0f);

    g.setColour(playerBgColour.brighter(0.3f));
    g.drawRoundedRectangle(area.toFloat(), 6.0f, 1.0f);

    if (audioThumbnail2.getNumChannels() > 0)
    {
        double audioLength = audioThumbnail2.getTotalLength();

        if (transportSource2.getLengthInSeconds() > 0.0)
        {
            // --- Draw the waveform ---
            g.setColour(accentYellow);
            audioThumbnail2.drawChannel(g, area.reduced(2), 0.0, audioLength, 0, 1.0f);

            // --- Draw the playhead ---
            double currentPosition = transportSource2.getCurrentPosition();
            double totalLength = transportSource2.getLengthInSeconds();

            if (totalLength > 0.0)
            {
                float playheadPosition = (float)((currentPosition / totalLength) * area.getWidth());
                g.setColour(accentPink);
                g.drawLine(area.getX() + playheadPosition, (float)area.getY(),
                    area.getX() + playheadPosition, (float)area.getBottom(), 2.0f);

                juce::Path playheadTriangle;
                playheadTriangle.addTriangle(area.getX() + playheadPosition - 5, (float)area.getY(),
                    area.getX() + playheadPosition + 5, (float)area.getY(),
                    area.getX() + playheadPosition, (float)area.getY() + 10);
                g.fillPath(playheadTriangle);
            }
        }
        else if (audioThumbnail2.isFullyLoaded())
        {
            g.setColour(accentYellow.withAlpha(0.5f));
            audioThumbnail2.drawChannel(g, area.reduced(2), 0.0, audioLength, 0, 1.0f);
        }
        else
        {
            g.setColour(textColour);
            g.setFont(juce::Font(16.0f));
            g.drawFittedText("Generating waveform...", area, juce::Justification::centred, 1);
        }
    }
    else
    {
        g.setColour(textColour.withAlpha(0.7f));
        g.setFont(juce::Font(16.0f));
        g.drawFittedText("No audio loaded", area, juce::Justification::centred, 1);
    }

    // --- A/B Marker Drawing ---
    if (currentASeg2 >= 0.0 || currentBSeg2 >= 0.0)
    {
        double totalLength = transportSource2.getLengthInSeconds();
        if (totalLength > 0.0)
        {
            auto timeToX = [&](double time)
                {
                    return (float)(area.getX() + (time / totalLength) * area.getWidth());
                };

            // --- A Marker ---
            if (currentASeg2 >= 0.0)
            {
                float xA = timeToX(currentASeg2);
                g.setColour(aMarkerColour);
                g.drawLine(xA, (float)area.getY(), xA, (float)area.getBottom(), 2.0f);
                g.setColour(textColour);
                g.drawFittedText("A", juce::Rectangle<int>((int)xA - 8, area.getY() - 15, 16, 16),
                    juce::Justification::centred, 1);
            }

            // --- B Marker ---
            if (currentBSeg2 >= 0.0)
            {
                float xB = timeToX(currentBSeg2);
                g.setColour(bMarkerColour);
                g.drawLine(xB, (float)area.getY(), xB, (float)area.getBottom(), 2.0f);
                g.setColour(textColour);
                g.drawFittedText("B", juce::Rectangle<int>((int)xB - 8, area.getY() - 15, 16, 16),
                    juce::Justification::centred, 1);
            }
        }
    }
}

void PlayerAudio::toggleLoop2()
{
    isLooping2 = !isLooping2;
}

void PlayerAudio::toggleMute2()
{
    isMuted2 = !isMuted2;
    transportSource2.setGain(isMuted2 ? 0.0f : 1.0f);
}

void PlayerAudio::toggleSegLooping(int PlayerID) {
    if (PlayerID == 1) {
        segLooping = !segLooping;
    }
    else if (PlayerID == 2) {
        segLooping2 = !segLooping2;
	}
}

void PlayerAudio::A_Seg(int PlayerID, bool seg) {
    if(seg){
        if (PlayerID == 1) {
            currentASeg = transportSource.getCurrentPosition();
			ASeg = true;
        }
        else if (PlayerID == 2) {
            currentASeg2 = transportSource2.getCurrentPosition();
			A2Seg = true;
        }
    }
    else{
        if (PlayerID == 1) {
            currentASeg = -1.0;
            ASeg = false;
        }
        else if (PlayerID == 2) {
            currentASeg2 = -1.0;
            A2Seg = false;
        }
	}
}
void PlayerAudio::B_Seg(int PlayerID, bool seg) {
    if (seg) {
        if (PlayerID == 1) {
            currentBSeg = transportSource.getCurrentPosition();
			BSeg = true;
        }
        else if (PlayerID == 2) {
            currentBSeg2 = transportSource2.getCurrentPosition();
			B2Seg = true;
        }
    }
    else {
        if (PlayerID == 1) {
            currentBSeg = -1.0;
			BSeg = false;
        }
        else if (PlayerID == 2) {
            currentBSeg2 = -1.0;
			B2Seg = false;
        }
    }
}
void PlayerAudio::AB_Seg_Looping(juce::AudioTransportSource& trans, bool shouldLoop,
    unique_ptr<juce::AudioFormatReaderSource>& reader)
{
    bool isSecond = (&trans == &transportSource2);
    double A = isSecond ? currentASeg2 : currentASeg;
    double B = isSecond ? currentBSeg2 : currentBSeg;

    double lower_bound = min(A, B);
    double upper_bound = max(A, B);
    auto ABSegment = std::make_pair(lower_bound, upper_bound);

    if (shouldLoop && reader != nullptr && A >= 0 && B >= 0)
    {
        double pos = trans.getCurrentPosition();
        if (ABSegment.first >= 0 && pos >= ABSegment.second - 0.01)
            trans.setPosition(ABSegment.first);
    }
}
