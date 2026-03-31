#include "AudioEngine.h"


AudioEngine::AudioEngine(QObject *parent)
    : QObject(parent),
    mediaPlayer(new QMediaPlayer(this)),
    audioOutput(new QAudioOutput(this)),
    audioBufferOutput(new QAudioBufferOutput(this)),
    audioAnalyzer(new AudioAnalyzer()),
    isPlaying(false),
    volume(50),
    positionMs(0),
    durationMs(0)
{
    // Register for cross-thread communication
    qRegisterMetaType<QAudioBuffer>("QAudioBuffer");

    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setAudioBufferOutput(audioBufferOutput);

    audioOutput->setVolume(volume / 100.0);

    audioAnalyzer->moveToThread(&analysisThread);

    connect(&analysisThread, &QThread::finished,
            audioAnalyzer, &QObject::deleteLater);

    // Forward audio buffer to thread
    connect(audioBufferOutput, &QAudioBufferOutput::audioBufferReceived,
            this, &AudioEngine::analyzeBuffer, Qt::DirectConnection);

    connect(this, &AudioEngine::analyzeBuffer,
            audioAnalyzer, &AudioAnalyzer::processBuffer, Qt::QueuedConnection);

    // Forward analysis results out
    connect(audioAnalyzer, &AudioAnalyzer::fftReady,
            this, &AudioEngine::fftBinsReady, Qt::QueuedConnection);

    connect(audioAnalyzer, &AudioAnalyzer::levelReady,
            this, &AudioEngine::analysisLevelReady,
            Qt::QueuedConnection);

    // Optional reset path when seeking/loading
    connect(this, &AudioEngine::resetAnalysis,
            audioAnalyzer, &AudioAnalyzer::reset,
            Qt::QueuedConnection);

    analysisThread.start();

    connect(mediaPlayer, &QMediaPlayer::playbackStateChanged,
            this, &AudioEngine::handlePlaybackStateChanged);

    connect(mediaPlayer, &QMediaPlayer::positionChanged,
            this, &AudioEngine::handlePositionChanged);

    connect(mediaPlayer, &QMediaPlayer::durationChanged,
            this, &AudioEngine::handleDurationChanged);

    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged,
            this, &AudioEngine::handleMediaStatusChanged);

    connect(mediaPlayer, &QMediaPlayer::errorChanged,
            this, &AudioEngine::handleErrorChanged);
}

void AudioEngine::play()
{
    if(sourceFile.isEmpty()){
        emit errorOccurred("No audio file loaded");
        return;
    }

    mediaPlayer->play();
}

void AudioEngine::pause()
{
    mediaPlayer->pause();
}

void AudioEngine::setVolume(int volumeValue)
{
    const int clampedVolume = qBound(0, volumeValue, 100);

    if (volume == clampedVolume) {
        return;
    }

    volume = clampedVolume;
    audioOutput->setVolume(volume / 100.0f);
}

void AudioEngine::setPositionMs(qint64 positionMs)
{
    if(positionMs < 0) positionMs = 0;
    const qint64 clampedPosition = qBound<qint64>(0, positionMs, durationMs);

    mediaPlayer->setPosition(clampedPosition);
}

void AudioEngine::loadFile(const QString &filePath)
{
    sourceFile = filePath;
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    emit sourceChanged(filePath);
}


bool AudioEngine::getIsPlaying() const
{
    return mediaPlayer->playbackState() == QMediaPlayer::PlayingState;
}

qint64 AudioEngine::getPositionMs() const
{
    return mediaPlayer->position();
}

qint64 AudioEngine::getDurationMs() const
{
    return mediaPlayer->duration();
}

QString AudioEngine::getSourceFile() const
{
    return sourceFile;
}

int AudioEngine::getVolume() const
{
    return volume;
}

void AudioEngine::handlePlaybackStateChanged()
{
    const bool playing = (mediaPlayer->playbackState() == QMediaPlayer::PlayingState);

    if (isPlaying == playing) {
        return;
    }

    isPlaying = playing;
    emit playingChanged(isPlaying);
}

void AudioEngine::handleDurationChanged(qint64 duration)
{
    if (durationMs == duration) {
        return;
    }

    durationMs = duration;
    emit durationChanged(durationMs);
}

void AudioEngine::handlePositionChanged(qint64 position)
{
    if (positionMs == position) {
        return;
    }

    positionMs = position;
    emit positionChanged(positionMs);
}

void AudioEngine::handleMediaStatusChanged()
{
    switch (mediaPlayer->mediaStatus()) {
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferedMedia:
        break;

    case QMediaPlayer::EndOfMedia:
        if (isPlaying) {
            isPlaying = false;
            emit playingChanged(false);
        }
        break;
    case QMediaPlayer::InvalidMedia:
        emit errorOccurred("Invalid or unsupported media");
        break;

    default:
        break;
    }
}

void AudioEngine::handleErrorChanged()
{
    if (mediaPlayer->error() != QMediaPlayer::NoError) {
        emit errorOccurred(mediaPlayer->errorString());
    }
}