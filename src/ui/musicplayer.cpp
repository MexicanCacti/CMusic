#include "musicplayer.h"

MusicPlayer::MusicPlayer(QWidget* parent)
    : QMainWindow(parent),
    playbackController(new PlaybackController(this)),
    loadButton(nullptr),
    refreshShaderButton(nullptr),
    shaderButton(nullptr),
    playButton(nullptr),
    prevButton(nullptr),
    nextButton(nullptr),
    fullscreenButton(nullptr),
    playbackSlider(nullptr),
    volumeSlider(nullptr),
    currentSong(nullptr),
    visualizer(nullptr),
    playTime(nullptr),
    clockTimer(new QTimer(this)),
    visualizerDock(nullptr),
    fullscreenContainer(nullptr),
    visualizerFullscreen(false)
{
    buildUi();
    connectUi();
    connectController();

    //currentDateTime->setDateTime(QDateTime::currentDateTime());

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
    loadButton      = new QPushButton("Load", this);
    shaderButton    = new QPushButton("Load Shader", this);
    refreshShaderButton = new QPushButton("Refresh Shader", this);
    playButton      = new QPushButton("Play", this);
    prevButton      = new QPushButton("Prev", this);
    nextButton      = new QPushButton("Next", this);
    fullscreenButton = new QPushButton("Fullscreen Visualizer", this);

    currentSong = new QLabel(this);
    currentSong->setText("CurrentSong: None");
    currentSong->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    currentSong->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    //currentDateTime = new QDateTimeEdit(this);
    //currentDateTime->setReadOnly(true);
    //currentDateTime->setDisplayFormat("MMMM dd yy hh:mm:s AP");

    playTime = new QLCDNumber(this);
    playTime->setDigitCount(5);
    playTime->setSegmentStyle(QLCDNumber::Filled);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);

    playbackSlider = new QSlider(Qt::Horizontal, this);
    playbackSlider->setRange(0, 0);

    QWidget* central = new QWidget(this);
    auto *mainLayout = new QGridLayout(central);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setHorizontalSpacing(12);
    mainLayout->setVerticalSpacing(6);

    mainLayout->addWidget(loadButton,       0, 0);
    mainLayout->addWidget(playButton,       0, 1);
    mainLayout->addWidget(currentSong,      0, 2);
    mainLayout->addWidget(refreshShaderButton, 0, 3);
    mainLayout->addWidget(shaderButton,     0, 4);
    //mainLayout->addWidget(currentDateTime,  0, 4);

    //mainLayout->addWidget(visualizerDock,       1, 0, 3, 5);

    mainLayout->addWidget(playbackSlider,   4, 0, 1, 5);
    mainLayout->addWidget(playTime,         5, 1, 1, 2);

    mainLayout->addWidget(prevButton,       6, 0);
    mainLayout->addWidget(volumeSlider,     6, 1, 1, 3);
    mainLayout->addWidget(nextButton,       6, 4, 1, 1);
    setCentralWidget(central);

    //setLayout(mainLayout);
    visualizer = new VisualWidget(this);
    visualizerDock = new QDockWidget("Visualizer", this);
    QWidget* titleBar = new QWidget(visualizerDock);
    QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
    QPushButton* visualizerShaderButton = new QPushButton("Load Shader", titleBar);
    titleLayout->setContentsMargins(4, 2, 4, 2);
    QLabel* titleLabel = new QLabel("Visualizer", titleBar);
    titleLayout->setSpacing(6);
    titleLayout->addWidget(visualizerShaderButton, 0, Qt::AlignLeft);
    titleLayout->addWidget(titleLabel, 0, Qt::AlignCenter | Qt::AlignVCenter);
    titleLayout->addWidget(fullscreenButton, 0, Qt::AlignRight);
    titleBar->setLayout(titleLayout);
    visualizerDock->setTitleBarWidget(titleBar);

    visualizerDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    visualizerDock->setFeatures(
        QDockWidget::DockWidgetMovable |
        QDockWidget::DockWidgetFloatable
    );

    visualizerDock->setWidget(visualizer);
    addDockWidget(Qt::BottomDockWidgetArea, visualizerDock);

    setWindowTitle("CMusic");
    resize(800, 400);
}

void MusicPlayer::connectUi()
{
    connect(loadButton, &QPushButton::clicked,
            this, &MusicPlayer::openFileDialog);

    connect(shaderButton, &QPushButton::clicked,
            this, &MusicPlayer::changeShaders);

    connect(refreshShaderButton, &QPushButton::clicked,
            this, &MusicPlayer::refreshShaders);

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

    connect(fullscreenButton, &QPushButton::clicked,
            this, &MusicPlayer::toggleVisualizerFullscreen);
    //connect(clockTimer, &QTimer::timeout,
    //        this, &MusicPlayer::updateDateTime);
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

    connect(playbackController, &PlaybackController::fftBinsReady,
            visualizer, &VisualWidget::setFFTBins);

    connect(playbackController, &PlaybackController::analysisLevelReady,
            visualizer, &VisualWidget::setLevel);

    //connect(playbackController, &PlaybackController::errorOccurred,
    //        this, &MusicPlayer::onErrorOccurred);
}

/*
void MusicPlayer::updateDateTime()
{
    currentDateTime->setDateTime(QDateTime::currentDateTime());
}
*/

void MusicPlayer::toggleVisualizerFullscreen()
{
    if (!visualizerFullscreen)
    {
        visualizerDock->hide();

        fullscreenContainer = new QWidget();
        auto *layout = new QVBoxLayout(fullscreenContainer);
        layout->setContentsMargins(0, 0, 0, 0);

        visualizer->setParent(fullscreenContainer);
        layout->addWidget(visualizer);

        fullscreenContainer->setWindowFlag(Qt::Window, true);
        fullscreenContainer->setWindowTitle("Visualizer");
        fullscreenContainer->showFullScreen();

        visualizerFullscreen = true;
    }
    else
    {
        fullscreenContainer->layout()->removeWidget(visualizer);
        visualizerDock->show();

        fullscreenContainer->close();
        delete fullscreenContainer;
        fullscreenContainer = nullptr;

        visualizerFullscreen = false;
    }
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

void MusicPlayer::refreshShaders()
{
    visualizer->reloadShaders();
}

void MusicPlayer::changeShaders()
{
    visualizer->findShaderSource();
    //playbackController->resetSong();
}
