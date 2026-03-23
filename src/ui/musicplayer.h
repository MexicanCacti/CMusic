#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QWidget>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QLCDNumber>
#include <QSlider>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QDateTime>
#include <QRandomGenerator>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QSignalBlocker>

#include "../visualizer/visualizerwidget.h"
#include "../audio/playbackcontroller.h"

class MusicPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit MusicPlayer(QWidget *parent = nullptr);

private slots:
    void updateDateTime();
    void onPlayingChanged(bool playing);
    void onCurrentSongChanged(const QString& songTitle);
    void onPlaybackPositionChanged(int seconds);
    void onDurationChanged(int seconds);
    void onVolumeChanged(int value);
    void onErrorOccurred(const QString &message);
    void openFileDialog();


private:
    void buildUi();
    void connectUi();
    void connectController();
    void updateTrackDisplay(int seconds);

    QPushButton *loadButton;
    QPushButton *playButton;
    QPushButton *prevButton;
    QPushButton *nextButton;

    QLabel *currentSong;
    QDateTimeEdit *currentDateTime;

    VisualizerWidget *visualizer;
    QLCDNumber *playTime;
    QSlider *playbackSlider;
    QSlider *volumeSlider;

    QTimer *clockTimer;
    PlaybackController *playbackController;
};

#endif // MUSICPLAYER_H
