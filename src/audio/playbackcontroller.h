#ifndef PLAYBACKCONTROLLER_H
#define PLAYBACKCONTROLLER_H
#include <QObject>
#include <QStringList>
#include <QTimer>
#include <QFileInfo>

#include "../audio/audioengine.h"

class PlaybackController : public QObject
{
    Q_OBJECT

public:
    explicit PlaybackController(QObject* parent = nullptr);

    bool getIsPlaying() const;
    QString getCurrentSong() const;
    int getCurrentPositionSeconds() const;
    int getDurationSeconds() const;
    int getVolume() const;

public slots:
    void togglePlayPause();
    void loadFile(const QString&filePath);
    void next();
    void prev();
    void setVolume(int value);
    void seekToSeconds(int seconds);

signals:
    void playingChanged(bool playing);
    void currentSongChanged(const QString& song);
    void songTimePositionChanged(int sec);
    void durationChanged(int sec);
    void volumeChanged(int volume);
    void errorOccured(const QString& message);

private:
    AudioEngine *audioEngine;
    QString currentSong;
    int currentSeconds;
    int trackLengthSeconds;
    int volume;

};

#endif // PLAYBACKCONTROLLER_H
