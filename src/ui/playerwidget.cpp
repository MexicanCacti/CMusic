#include "playerwidget.h"

PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget(parent),
    playButton(nullptr),
    prevButton(nullptr),
    nextButton(nullptr),
    currentSong(nullptr),
    currentDateTime(nullptr),
    visualizer(nullptr),
    playTime(nullptr),
    volumeSlider(nullptr),
    clockTimer(new QTimer(this)),
    playbackController(new PlaybackController(this))
{
    buildUi();
    connectUi();
    connectController();

    currentDateTime->setDateTime(QDateTime::currentDateTime());

    onPlayingChanged(playbackController->getIsPlaying());
    onCurrentSongChanged(playbackController->currentSong());
    onCurrentTimeChanged(playbackController->getCurrentPositionSeconds());
    onVolumeChanged(playbackController->getVolume());

    volumeSlider->setValue(playbackController->getVolume());

}


void PlayerWidget::buildUi()
{
    playButton = new QPushButton("Play", this);
    prevButton = new QPushButton("Prev", this);
    nextButton = new QPushButton("Next", this);

    currentSong = new QLabel(this);
    currentSong->setText("Current Song: None");
    currentSong->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    currentDateTime = new QDateTimeEdit(this);
    currentDateTime->setReadOnly(true);
    currentDateTime->setDisplayFormat("ddd MMM d yyyy  hh:mm:ss AP");

    visualizer = new VisualizerWidget(this);

    playTime = new QLCDNumber(this);
    playTime->setDigitCount(5);
    playTime->setSegmentStyle(QLCDNumber::Flat);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);

    auto *mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setHorizontalSpacing(12);
    mainLayout->setVerticalSpacing(12);

    mainLayout->addWidget(playButton,      0, 0);
    mainLayout->addWidget(currentSong,     0, 1);
    mainLayout->addWidget(currentDateTime, 0, 2);

    mainLayout->addWidget(visualizer,      1, 0, 1, 3);
    mainLayout->addWidget(playTime,        2, 1);

    mainLayout->addWidget(prevButton,      3, 0);
    mainLayout->addWidget(volumeSlider,    3, 1);
    mainLayout->addWidget(nextButton,      3, 2);

    setLayout(mainLayout);
    setWindowTitle("CMusic");
    resize(700, 320);
}

void PlayerWidget::connectUi()
{
    connect(playButton, &QPushButton::clicked,
            playbackController, &PlaybackController::togglePlayPause);
    connect(prevButton, &QPushButton::clicked,
            playbackController, &PlaybackController::prev);
    connect(nextButton, &QPushButton::clicked,
            playbackController, &PlaybackController::next);
    connect(volumeSlider, &QSlider::valueChanged,
            playbackController, &PlaybackController::setVolume);
    connect(clockTimer, &QTimer::timeout,
            this, &PlayerWidget::updateDateTime);
}

void PlayerWidget::connectController()
{
    connect(playbackController, &PlaybackController::playingChanged,
            this, &PlayerWidget::onPlayingChanged);
    connect(playbackController, &PlaybackController::currentSongChanged,
            this, &PlayerWidget::onCurrentSongChanged);
    connect(playbackController, &PlaybackController::songTimePositionChanged,
            this, &PlayerWidget::onCurrentTimeChanged);
    connect(playbackController, &PlaybackController::volumeChanged,
            this, &PlayerWidget::onVolumeChanged);
}

void PlayerWidget::updateDateTime()
{
    currentDateTime->setDateTime(QDateTime::currentDateTime());
}

void PlayerWidget::onPlayingChanged(bool playing)
{
    playButton->setText(playing ? "Pause" : "Play");
    visualizer->setPlaying(playing);
}

void PlayerWidget::onCurrentSongChanged(const QString &songTitle)
{
    currentSong->setText("Current Song: " + songTitle);
}

void PlayerWidget::onCurrentTimeChanged(int seconds)
{
    updateTrackDisplay(seconds);
}

void PlayerWidget::onVolumeChanged(int value)
{
    visualizer->setLevel(value);
    if(volumeSlider->value() != value){
        volumeSlider->setValue(value);
    }
}

void PlayerWidget::updateTrackDisplay(int seconds)
{
    const int minutes = seconds / 60;
    const int secs = seconds % 60;

    playTime->display(QString("%1:%2")
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(secs, 2, 10, QChar('0')));
}