// PlaybackController.cpp
#include "playbackcontroller.h"

PlaybackController::PlaybackController(QObject *parent)
    : QObject(parent),
    audioEngine(new AudioEngine(this)),
    currentSong(),
    currentSeconds(0),
    trackLengthSeconds(0),
    volume(60)
{
    connect(audioEngine, &AudioEngine::playingChanged,
            this, &PlaybackController::playingChanged);

    connect(audioEngine, &AudioEngine::positionChanged,
            this, [this](qint64 positionMs)
            {
                const int seconds = static_cast<int>(positionMs / 1000);

                if (currentSeconds == seconds) {
                    return;
                }

                currentSeconds = seconds;
                emit songTimePositionChanged(currentSeconds);
            });

    connect(audioEngine, &AudioEngine::durationChanged,
            this, [this](qint64 durationMs)
            {
                const int seconds = static_cast<int>(durationMs / 1000);

                if (trackLengthSeconds == seconds) {
                    return;
                }

                trackLengthSeconds = seconds;
                emit durationChanged(trackLengthSeconds);
            });

    connect(audioEngine, &AudioEngine::sourceChanged,
            this, [this](const QString &filePath)
            {
                QFileInfo fileInfo(filePath);
                currentSong = fileInfo.completeBaseName().isEmpty()
                                    ? fileInfo.fileName()
                                    : fileInfo.completeBaseName();

                currentSeconds = 0;
                emit currentSongChanged(currentSong);
                emit songTimePositionChanged(currentSeconds);
            });

    connect(audioEngine, &AudioEngine::errorOccurred,
            this, &::PlaybackController::errorOccured);

    audioEngine->setVolume(volume);
}

bool PlaybackController::getIsPlaying() const
{
    return audioEngine->getIsPlaying();
}

QString PlaybackController::getCurrentSong() const
{
    return currentSong;
}

int PlaybackController::getCurrentPositionSeconds() const
{
    return currentSeconds;
}

int PlaybackController::getDurationSeconds() const
{
    return trackLengthSeconds;
}

int PlaybackController::getVolume() const
{
    return volume;
}

void PlaybackController::loadFile(const QString &filePath)
{
    if(filePath.isEmpty()) return;

    audioEngine->loadFile(filePath);

    QFileInfo fileInfo(filePath);
    currentSong = fileInfo.completeBaseName().isEmpty()
                        ? fileInfo.fileName()
                        : fileInfo.completeBaseName();

    currentSeconds = 0;
    trackLengthSeconds = 0;

    emit currentSongChanged(currentSong);
    emit songTimePositionChanged(currentSeconds);
    emit durationChanged(trackLengthSeconds);
}

void PlaybackController::togglePlayPause()
{
    if(audioEngine->getSourceFile().isEmpty()) return;

    audioEngine->getIsPlaying() ? audioEngine->pause() : audioEngine->play();
}

void PlaybackController::next()
{
    // placeholder
}

void PlaybackController::prev()
{
    // placeholder
}

void PlaybackController::setVolume(int value)
{
    value = qBound(0, value, 100);

    if (volume == value) {
        return;
    }

    volume = value;
    audioEngine->setVolume(volume);
    emit volumeChanged(volume);
}

void PlaybackController::seekToSeconds(int seconds)
{
    if(seconds < 0) seconds = 0;

    if(trackLengthSeconds > 0 && seconds > trackLengthSeconds) seconds = trackLengthSeconds;

    audioEngine->setSongPosition(static_cast<qint64>(seconds) * 1000);
}