#ifndef PLAYBACKCONTROLLER_H
#define PLAYBACKCONTROLLER_H
#include <QObject>
#include <QStringList>
#include <QTimer>

class PlaybackController : public QObject
{
    Q_OBJECT

public:
    explicit PlaybackController(QObject* parent = nullptr);

    bool getIsPlaying() const;
    QString currentSong() const;
    int getCurrentPositionSeconds() const;
    int getDurationSeconds() const;
    int getVolume() const;

public slots:
    void play();
    void pause();
    void togglePlayPause();
    void next();
    void prev();
    void setVolume(int val);
    void setTimeSec(int sec);
    void tick();

signals:
    void playingChanged(bool playing);
    void currentSongChanged(const QString& song);
    void songTimePositionChanged(int sec);
    void durationChanged(int sec);
    void volumeChanged(int volume);

private:
    QStringList playlist;
    int currentSongIndex;
    bool isPlaying;
    int positionSeconds;
    int durationSeconds;
    int volume;
    QTimer timer;

};

#endif // PLAYBACKCONTROLLER_H
