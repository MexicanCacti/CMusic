#include "playerwidget.h"

PlayerWidget::PlayerWidget(QWidget *parent)
    : QWidget(parent),
    loadButton(nullptr),
    playButton(nullptr),
    prevButton(nullptr),
    nextButton(nullptr),
    currentSong(nullptr),
    currentDateTime(nullptr),
    visualizer(nullptr),
    playTime(nullptr),
    volumeSlider(nullptr),
    playbackSlider(nullptr),
    clockTimer(new QTimer(this)),
    playbackController(new PlaybackController(this))
{
    buildUi();
    connectUi();
    connectController();

    currentDateTime->setDateTime(QDateTime::currentDateTime());

    onPlayingChanged(playbackController->getIsPlaying());
    onCurrentSongChanged(playbackController->getCurrentSong());
    onCurrentTimeChanged(playbackController->getCurrentPositionSeconds());
    onVolumeChanged(playbackController->getVolume());

    playbackSlider->setValue(playbackController->getCurrentPositionSeconds());
    volumeSlider->setValue(playbackController->getVolume());

}


void PlayerWidget::buildUi()
{
    loadButton = new QPushButton("Load", this);
    playButton = new QPushButton("Play", this);
    prevButton = new QPushButton("Prev", this);
    nextButton = new QPushButton("Next", this);

    currentSong = new QLabel(this);
    currentSong->setText("Current Song: None");
    currentSong->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    currentDateTime = new QDateTimeEdit(this);
    currentDateTime->setReadOnly(true);
    currentDateTime->setDisplayFormat("ddd MMM d yyyy  hh:mm:ss AP");
    currentSong->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    visualizer = new VisualizerWidget(this);

    playTime = new QLCDNumber(this);
    playTime->setDigitCount(5);
    playTime->setSegmentStyle(QLCDNumber::Flat);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);

    playbackSlider = new QSlider(Qt::Horizontal, this);
    playbackSlider->setRange(0, 0);
    playbackSlider->toolTip();

    auto *mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setHorizontalSpacing(12);
    mainLayout->setVerticalSpacing(12);

    mainLayout->addWidget(loadButton,      0, 0);
    mainLayout->addWidget(playButton,      0, 1);
    mainLayout->addWidget(currentSong,     0, 2);
    mainLayout->addWidget(currentDateTime, 0, 3);

    mainLayout->addWidget(visualizer,      1, 0, 2, 4);
    mainLayout->addWidget(playbackSlider,  3, 0, 1, 4);
    mainLayout->addWidget(playTime,        4, 1, 1, 2);

    mainLayout->addWidget(prevButton,      5, 0);
    mainLayout->addWidget(volumeSlider,    5, 1, 1, 2);
    mainLayout->addWidget(nextButton,      5, 3);

    setLayout(mainLayout);
    setWindowTitle("CMusic");
    resize(700, 320);
}

void PlayerWidget::connectUi()
{
    connect(loadButton, &QPushButton::clicked,
            this, &PlayerWidget::openFileDialog);
    connect(playButton, &QPushButton::clicked,
            playbackController, &PlaybackController::togglePlayPause);
    connect(prevButton, &QPushButton::clicked,
            playbackController, &PlaybackController::prev);
    connect(nextButton, &QPushButton::clicked,
            playbackController, &PlaybackController::next);
    connect(volumeSlider, &QSlider::valueChanged,
            playbackController, &PlaybackController::setVolume);
    connect(playbackSlider, &QSlider::sliderMoved,
            playbackController, &PlaybackController::seekToSeconds);
    connect(playbackSlider, &QSlider::sliderReleased,
            this, [this]()
            {
                playbackController->seekToSeconds(playbackSlider->value());
            });
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
    connect(playbackController, &PlaybackController::durationChanged,
            this, &::PlayerWidget::onDurationChanged);
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

    if(!playbackSlider->isSliderDown() && playbackSlider->value() != seconds){
        playbackSlider->setValue(seconds);
    }
}

void PlayerWidget::onDurationChanged(int seconds)
{
    playbackSlider->setRange(0, seconds > 0 ? seconds : 0);
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

void PlayerWidget::openFileDialog()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Audio File"),
        QString(),
        tr("Audio Files (*.mp3 *.wav *.flac *.ogg *.m4a *.acc);;All Files(*)")
        );

    playbackController->loadFile(filePath);
}