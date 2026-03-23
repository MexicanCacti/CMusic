// PlaybackController.cpp
#include "playbackcontroller.h"

PlaybackController::PlaybackController(QObject *parent)
    : QObject(parent),
    audioEngine(new AudioEngine(this)),
    currentSong()
{
    connect(audioEngine, &AudioEngine::playingChanged,
            this, &PlaybackController::onEnginePlayingChanged);

    connect(audioEngine, &AudioEngine::positionChanged,
            this, &PlaybackController::onEnginePositionChanged);

    connect(audioEngine, &AudioEngine::durationChanged,
            this, &PlaybackController::onEngineDurationChanged);

    connect(audioEngine, &AudioEngine::sourceChanged,
            this, &PlaybackController::onEngineSourceChanged);

    connect(audioEngine, &AudioEngine::errorOccurred,
            this, &::PlaybackController::onEngineErrorOccurred);
}

void PlaybackController::togglePlayPause()
{
    if(!checkEngineStatus()) return;

    audioEngine->getIsPlaying() ? audioEngine->pause() : audioEngine->play();
}

void PlaybackController::next()
{
    // Placeholder
    emit errorOccurred("Next not implemented");
}

void PlaybackController::prev()
{
    // Placeholder
    emit errorOccurred("Prev not implemented");
}

void PlaybackController::setVolume(int volume)
{
    if(!checkEngineStatus()) return;

    audioEngine->setVolume(volume);
}

void PlaybackController::seek(int seconds)
{
    if(!checkEngineStatus()) return;

    audioEngine->setPositionMs(static_cast<qint64>(seconds) * 1000);
}

void PlaybackController::loadFile(const QString& filePath)
{
    if(!checkEngineStatus()) return;

    if(filePath.isEmpty()) {
        emit errorOccurred("File path is empty");
        return;
    }

    audioEngine->loadFile(filePath);
}

void PlaybackController::onEnginePlayingChanged(bool playing)
{
    emit playingChanged(playing);
}

void PlaybackController::onEnginePositionChanged(qint64 positionMs)
{
    emit playbackPositionChanged(static_cast<int>(positionMs / 1000));
}

void PlaybackController::onEngineDurationChanged(qint64 durationMs)
{
    emit durationChanged(static_cast<int>(durationMs / 1000));
}

void PlaybackController::onEngineSourceChanged(const QString& filePath)
{
    QString songTitle;

    QFileInfo fileInfo(filePath);
    songTitle = fileInfo.completeBaseName().isEmpty()
                      ? fileInfo.fileName()
                      : fileInfo.completeBaseName();

    if(songTitle.isEmpty()) songTitle = filePath;

    if(currentSong == songTitle) return;

    currentSong = songTitle;
    emit currentSongChanged(currentSong);
    emit playbackPositionChanged(0);
}

void PlaybackController::onEngineErrorOccurred(const QString& message)
{
    emit errorOccurred(message);
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
    return static_cast<int>(audioEngine->getPositionMs() / 1000);
}

int PlaybackController::getDurationSeconds() const
{
    return static_cast<int>(audioEngine->getDurationMs() / 1000);
}

int PlaybackController::getVolume() const
{
    return audioEngine->getVolume();
}

bool PlaybackController::checkEngineStatus()
{
    return audioEngine ? true : false;
}