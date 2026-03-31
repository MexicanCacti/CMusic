#ifndef PLAYBACKCONTROLLER_H
#define PLAYBACKCONTROLLER_H
#include <QObject>
#include <QStringList>
#include <QTimer>
#include <QFileInfo>
#include <QAudioBufferOutput>
#include "../audio/audioengine.h"

class PlaybackController : public QObject
{
    Q_OBJECT

public:
    explicit PlaybackController(QObject* parent = nullptr);

    void togglePlayPause();
    void next();
    void prev();
    void setVolume(int volume);
    void seek(int seconds);
    void loadFile(const QString& filePath);
    void resetSong();

    bool getIsPlaying() const;
    QString getCurrentSong() const;
    int getCurrentPositionSeconds() const;
    int getDurationSeconds() const;
    int getVolume() const;

public slots:
    void onEnginePlayingChanged(bool playing);
    void onEnginePositionChanged(qint64 positionMs);
    void onEngineDurationChanged(qint64 durationMs);
    void onEngineSourceChanged(const QString& filePath);
    void onEngineErrorOccurred(const QString& message);

signals:
    void playingChanged(bool playing);
    void currentSongChanged(const QString& songTitle);
    void playbackPositionChanged(int seconds);
    void durationChanged(int seconds);
    void volumeChanged(int volume);
    void errorOccurred(const QString& message);
    void fftBinsReady(const QVector<float>& bins);
    void analysisLevelReady(float level);
private:
    AudioEngine *audioEngine;
    QString currentSong;
    bool checkEngineStatus();

};

#endif // PLAYBACKCONTROLLER_H
