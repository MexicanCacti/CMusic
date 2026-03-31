#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <QObject>
#include <QUrl>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QAudioBufferOutput>
#include <QFileInfo>
#include <QMetaType>
#include "../audio/audioanalyzer.h"

class AudioEngine : public QObject
{
    Q_OBJECT

public:
    explicit AudioEngine(QObject *parent = nullptr);

    void play();
    void pause();
    void setVolume(int volume);
    void setPositionMs(qint64 positionMs);
    void loadFile(const QString& filePath);

    bool getIsPlaying() const;
    int getVolume() const;
    qint64 getPositionMs() const;
    qint64 getDurationMs() const;
    QString getSourceFile() const;

signals:
    void playingChanged(bool playing);
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 trackLengthMs); // durationChanged()
    void sourceChanged(const QString& filePath);
    void errorOccurred(const QString& message);

    void fftBinsReady(const QVector<float>& bins);
    void analysisLevelReady(int level);

    void analyzeBuffer(const QAudioBuffer& buffer);
    void resetAnalysis();

private slots:
    void handlePlaybackStateChanged();
    void handlePositionChanged(qint64 position);
    void handleDurationChanged(qint64 trackLength); // handleDurationChanged()
    void handleMediaStatusChanged();
    void handleErrorChanged();

private:
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QAudioBufferOutput *audioBufferOutput;
    AudioAnalyzer* audioAnalyzer;
    QString sourceFile;
    QThread analysisThread;

    bool isPlaying;
    int volume;
    qint64 positionMs;
    qint64 durationMs;
};

#endif // AUDIOENGINE_H
