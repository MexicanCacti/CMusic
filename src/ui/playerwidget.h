#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QGraphicsView>
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

#include "../visualizer/visualizerwidget.h"

class PlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerWidget(QWidget *parent = nullptr);

private slots:
    void togglePlayPause();
    void updateDateTime();
    void previousSong();
    void nextSong();
    void setVolume(int value);
    void tickPlayback();

private:
    void buildUi();
    void connectSignals();
    void updateTrackDisplay();


    QPushButton *playButton;
    QPushButton *prevButton;
    QPushButton *nextButton;

    QLabel *currentSong;
    QDateTimeEdit *currentDateTime;

    VisualizerWidget *visualizer;
    QLCDNumber *playTime;
    QSlider *volumeSlider;

    QTimer *clockTimer;
    QTimer *playbackTimer;

    bool isPlaying;
    int currentSeconds;
    int trackLengthSeconds;
};

#endif // PLAYERWIDGET_H
