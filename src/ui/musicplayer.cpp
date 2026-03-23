#include "musicplayer.h"

MusicPlayer::MusicPlayer(QWidget* parent)
    : QWidget(parent),
    playbackController(new PlaybackController(this)),
    loadButton(nullptr),
    playButton(nullptr),
    prevButton(nullptr),
    nextButton(nullptr),
    playbackSlider(nullptr),
    volumeSlider(nullptr),
    currentSong(nullptr),
    currentDateTime(nullptr),
    visualizer(nullptr),
    playTime(nullptr),
    clockTimer(new QTimer(this))
{
    buildUi();
    connectUi();
    connectController();

    currentDateTime->setDateTime(QDateTime::currentDateTime());

    onPlayingChanged(playbackController->getIsPlaying());
    onCurrentSongChanged(playbackController->getCurrentSong());
    onPlaybackPositionChanged(playbackController->getCurrentPositionSeconds());
    onDurationChanged(playbackController->getDurationSeconds());
    onVolumeChanged(playbackController->getVolume());

    playbackSlider->setValue(playbackController->getCurrentPositionSeconds());
    volumeSlider->setValue(playbackController->getVolume());

    clockTimer->start(1000);
}

void MusicPlayer::buildUi()
{
    loadButton = new QPushButton("Load", this);
    playButton = new QPushButton("Play", this);
    prevButton = new QPushButton("Prev", this);
    nextButton = new QPushButton("Next", this);

    currentSong = new QLabel(this);
    currentSong->setText("CurrentSong: None");
    currentSong->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    currentSong->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    currentDateTime = new QDateTimeEdit(this);
    currentDateTime->setReadOnly(true);
    currentDateTime->setDisplayFormat("ddd MMM yy hh:mm:ss AP");

    visualizer = new VisualWidget(this);

    playTime = new QLCDNumber(this);
    playTime->setDigitCount(5);
    playTime->setSegmentStyle(QLCDNumber::Outline);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);

    playbackSlider = new QSlider(Qt::Horizontal, this);
    playbackSlider->setRange(0, 0);

    auto *mainLayout = new QGridLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setHorizontalSpacing(12);
    mainLayout->setVerticalSpacing(6);

    mainLayout->addWidget(loadButton,       0, 0);
    mainLayout->addWidget(playButton,       0, 1);
    mainLayout->addWidget(currentSong,      0, 2);
    mainLayout->addWidget(currentDateTime,  0, 3);

    mainLayout->addWidget(visualizer,       1, 0, 2, 4);

    mainLayout->addWidget(playbackSlider,   3, 0, 1, 4);
    mainLayout->addWidget(playTime,         4, 1, 1, 2);

    mainLayout->addWidget(prevButton,       5, 0);
    mainLayout->addWidget(volumeSlider,     5, 1, 1, 2);
    mainLayout->addWidget(nextButton,       5, 3);

    setLayout(mainLayout);
    setWindowTitle("CMusic");
    resize(800, 400);
}

void MusicPlayer::connectUi()
{
    connect(loadButton, &QPushButton::clicked,
            this, &MusicPlayer::openFileDialog);

    connect(playButton, &QPushButton::clicked,
            playbackController, &PlaybackController::togglePlayPause);

    connect(prevButton, &QPushButton::clicked,
            playbackController, &PlaybackController::prev);

    connect(nextButton, &QPushButton::clicked,
            playbackController, &PlaybackController::next);

    connect(volumeSlider, &QSlider::valueChanged,
            playbackController, &PlaybackController::setVolume);

    connect(playbackSlider, &QSlider::sliderMoved,
            this, &MusicPlayer::updateTrackDisplay);

    connect(playbackSlider, &QSlider::sliderReleased,
            this, [this]()
            {
                playbackController->seek(playbackSlider->value());
            });

    connect(clockTimer, &QTimer::timeout,
            this, &MusicPlayer::updateDateTime);
}

void MusicPlayer::connectController()
{
    connect(playbackController, &PlaybackController::playingChanged,
            this, &MusicPlayer::onPlayingChanged);

    connect(playbackController, &PlaybackController::currentSongChanged,
            this, &MusicPlayer::onCurrentSongChanged);

    connect(playbackController, &PlaybackController::playbackPositionChanged,
            this, &MusicPlayer::onPlaybackPositionChanged);

    connect(playbackController, &PlaybackController::durationChanged,
            this, &MusicPlayer::onDurationChanged);

    connect(playbackController, &PlaybackController::volumeChanged,
            this, &MusicPlayer::onVolumeChanged);

    connect(playbackController, &PlaybackController::errorOccurred,
            this, &MusicPlayer::onErrorOccurred);
}

void MusicPlayer::updateDateTime()
{
    currentDateTime->setDateTime(QDateTime::currentDateTime());
}

void MusicPlayer::onPlayingChanged(bool playing)
{
    playButton->setText(playing ? "Pause" : "Play");
    visualizer->setPlaying(playing);
}

void MusicPlayer::onCurrentSongChanged(const QString &songTitle)
{
    currentSong->setText("Current Song: " + (songTitle.isEmpty() ? QString("None") : songTitle));
}

void MusicPlayer::onPlaybackPositionChanged(int seconds)
{
    if(playbackSlider->isSliderDown()) return;
    updateTrackDisplay(seconds);

    if(playbackSlider->value() != seconds) {
        QSignalBlocker blocker(playbackSlider);
        playbackSlider->setValue(seconds);
    }
    visualizer->setPlaybackTime(static_cast<float>(seconds));
}

void MusicPlayer::onDurationChanged(int seconds)
{
    const int clampedSeconds = seconds > 0 ? seconds : 0;

    QSignalBlocker blocker(playbackSlider);
    playbackSlider->setRange(0, clampedSeconds);
}

void MusicPlayer::onVolumeChanged(int value)
{
    visualizer->setLevel(value);

    if (volumeSlider->value() != value) {
        QSignalBlocker blocker(volumeSlider);
        volumeSlider->setValue(value);
    }
}

void MusicPlayer::onErrorOccurred(const QString &message)
{
    QMessageBox::warning(this, "Playback Error", message);
}

void MusicPlayer::updateTrackDisplay(int seconds)
{
    const int minutes = seconds / 60;
    const int secs = seconds % 60;

    playTime->display(QString("%1:%2")
                          .arg(minutes, 2, 10, QChar('0'))
                          .arg(secs, 2, 10, QChar('0')));
}

void MusicPlayer::openFileDialog()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Audio File"),
        QString(),
        tr("Audio Files (*.mp3 *.wav *.flac *.ogg *.m4a *.aac);;All Files (*)")
        );

    if (filePath.isEmpty()) {
        return;
    }

    playbackController->loadFile(filePath);
}
