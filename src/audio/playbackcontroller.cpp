// PlaybackController.cpp
#include "playbackcontroller.h"

PlaybackController::PlaybackController(QObject *parent)
    : QObject(parent),
    playlist({"Song 1", "Song 2", "Song 3"}),
    currentSongIndex(0),
    isPlaying(false),
    positionSeconds(0),
    durationSeconds(245),
    volume(60)
{
    connect(&timer, &QTimer::timeout, this, &PlaybackController::tick);
    timer.start(1000);
}

bool PlaybackController::getIsPlaying() const
{
    return isPlaying;
}

QString PlaybackController::currentSong() const
{
    return playlist.value(currentSongIndex);
}

int PlaybackController::getCurrentPositionSeconds() const
{
    return positionSeconds;
}

int PlaybackController::getDurationSeconds() const
{
    return durationSeconds;
}

int PlaybackController::getVolume() const
{
    return volume;
}

void PlaybackController::play()
{
    if (isPlaying) {
        return;
    }

    isPlaying = true;
    emit playingChanged(isPlaying);
}

void PlaybackController::pause()
{
    if (!isPlaying) {
        return;
    }

    isPlaying = false;
    emit playingChanged(isPlaying);
}

void PlaybackController::togglePlayPause()
{
    isPlaying ? pause() : play();
}

void PlaybackController::next()
{
    currentSongIndex = (currentSongIndex + 1) % playlist.size();
    positionSeconds = 0;
    emit currentSongChanged(currentSong());
    emit songTimePositionChanged(positionSeconds);
}

void PlaybackController::prev()
{
    currentSongIndex = (currentSongIndex - 1 + playlist.size()) % playlist.size();
    positionSeconds = 0;
    emit currentSongChanged(currentSong());
    emit songTimePositionChanged(positionSeconds);
}

void PlaybackController::setVolume(int value)
{
    if (volume == value) {
        return;
    }

    volume = value;
    emit volumeChanged(volume);
}

void PlaybackController::setTimeSec(int seconds)
{
    if (seconds < 0) {
        seconds = 0;
    }
    if (seconds > durationSeconds) {
        seconds = durationSeconds;
    }

    if (positionSeconds == seconds) {
        return;
    }

    positionSeconds = seconds;
    emit songTimePositionChanged(positionSeconds);
}

void PlaybackController::tick()
{
    if (!isPlaying) {
        return;
    }

    if (++positionSeconds > durationSeconds) {
        next();
        return;
    }

    emit songTimePositionChanged(positionSeconds);
}