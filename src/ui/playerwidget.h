#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

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

#include "../visualizer/visualizerwidget.h"
#include "../audio/playbackcontroller.h"

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = nullptr);

private slots:
    void updateDateTime();
    void onPlayingChanged(bool playing);
    void onCurrentSongChanged(const QString &songTitle);
    void onCurrentTimeChanged(int seconds);
    void onDurationChanged(int seconds);
    void onVolumeChanged(int value);
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

#endif // PLAYERWIDGET_H
