#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <QObject>
#include <QUrl>
#include <QMediaPlayer>
#include <QAudioOutput>

class AudioEngine : public QObject
{
    Q_OBJECT
public:
    explicit AudioEngine(QObject *parent = nullptr);

    void loadFile(const QString& filePath);
    void play();
    void pause();
    void stop();
    void setVolume(int value);
    void setSongPosition(qint64 positionMs);

    bool getIsPlaying() const;
    qint64 getSongPositon() const;
    qint64 getSongDuration() const;
    QString getSourceFile() const;

signals:
    void playingChanged(bool playing);
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void sourceChanged(const QString &filePath);
    void errorOccurred(const QString &message);

private slots:
    void handlePlaybackStateChanged();
    void handleErrorChanged();

private:
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    QString sourceFile;
};

#endif // AUDIOENGINE_H
