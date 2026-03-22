#include "AudioEngine.h"


AudioEngine::AudioEngine(QObject *parent)
    : QObject(parent),
    mediaPlayer(new QMediaPlayer(this)),
    audioOutput(new QAudioOutput(this))
{
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.6f);

    connect(mediaPlayer, &QMediaPlayer::positionChanged,
            this, &AudioEngine::positionChanged);

    connect(mediaPlayer, &QMediaPlayer::durationChanged,
            this, &AudioEngine::durationChanged);

    connect(mediaPlayer, &QMediaPlayer::playbackStateChanged,
            this, &AudioEngine::handlePlaybackStateChanged);

    connect(mediaPlayer, &QMediaPlayer::errorChanged,
            this, &AudioEngine::handleErrorChanged);
}

void AudioEngine::loadFile(const QString &filePath)
{
    sourceFile = filePath;
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    emit sourceChanged(filePath);
}

void AudioEngine::play()
{
    mediaPlayer->play();
}

void AudioEngine::pause()
{
    mediaPlayer->pause();
}

void AudioEngine::stop()
{
    mediaPlayer->stop();
}

void AudioEngine::setVolume(int value)
{
    const float normalized = qBound(0, value, 100) / 100.0f;
    audioOutput->setVolume(normalized);
}

bool AudioEngine::getIsPlaying() const
{
    return mediaPlayer->playbackState() == QMediaPlayer::PlayingState;
}

qint64 AudioEngine::getSongPositon() const
{
    return mediaPlayer->position();
}

qint64 AudioEngine::getSongDuration() const
{
    return mediaPlayer->duration();
}

void AudioEngine::setSongPosition(qint64 positionMs)
{
    if(positionMs < 0) positionMs = 0;

    mediaPlayer->setPosition(positionMs);
}

QString AudioEngine::getSourceFile() const
{
    return sourceFile;
}

void AudioEngine::handlePlaybackStateChanged()
{
    emit playingChanged(getIsPlaying());
}

void AudioEngine::handleErrorChanged()
{
    if (mediaPlayer->error() != QMediaPlayer::NoError) {
        emit errorOccurred(mediaPlayer->errorString());
    }
}