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
#include <QDockWidget>
#include <QMainWindow>


#include "../visualizer/visualWidget.h"
#include "../audio/playbackcontroller.h"

class MusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MusicPlayer(QWidget *parent = nullptr);

private slots:
    //void updateDateTime();
    void onPlayingChanged(bool playing);
    void onCurrentSongChanged(const QString& songTitle);
    void onPlaybackPositionChanged(int seconds);
    void onDurationChanged(int seconds);
    void onVolumeChanged(int value);
    void onErrorOccurred(const QString &message);
    void openFileDialog();
    void changeShaders();
    void toggleVisualizerFullscreen();
    void refreshShaders();


private:
    void buildUi();
    void connectUi();
    void connectController();
    void updateTrackDisplay(int seconds);

    QPushButton *loadButton;
    QPushButton* refreshShaderButton;
    QPushButton *shaderButton;
    QPushButton *playButton;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QPushButton *fullscreenButton;

    QLabel *currentSong;
    //QDateTimeEdit *currentDateTime;

    VisualWidget *visualizer;
    QLCDNumber *playTime;
    QSlider *playbackSlider;
    QSlider *volumeSlider;

    QTimer *clockTimer;
    PlaybackController *playbackController;

    QDockWidget* visualizerDock;
    QWidget* fullscreenContainer;
    bool visualizerFullscreen;

};

#endif // MUSICPLAYER_H
