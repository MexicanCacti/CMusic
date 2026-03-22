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
    playbackTimer(new QTimer(this)),
    isPlaying(false),
    currentSeconds(0),
    trackLengthSeconds(245),
    playbackTickCount(0)
{
    buildUi();
    connectSignals();

    currentSong->setText("Current Song: None");
    currentDateTime->setDateTime(QDateTime::currentDateTime());
    volumeSlider->setValue(50);

    updateTrackDisplay();
    updateVisualizer();

    clockTimer->start(1000);
    playbackTimer->start(250);

}


void PlayerWidget::buildUi()
{
    playButton = new QPushButton("Play", this);
    prevButton = new QPushButton("Prev", this);
    nextButton = new QPushButton("Next", this);

    currentSong = new QLabel("None", this);

    currentDateTime = new QDateTimeEdit(this);
    currentDateTime->setReadOnly(true);
    currentDateTime->setDisplayFormat("ddd MMM d yyyy  hh:mm:ss AP");

    visualizer = new QGraphicsView(this);
    visualizer->setMinimumHeight(140);
    visualizer->setScene(new QGraphicsScene(visualizer));

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

void PlayerWidget::connectSignals()
{
    connect(playButton, &QPushButton::clicked,
            this, &PlayerWidget::togglePlayPause);
    connect(prevButton, &QPushButton::clicked,
            this, &PlayerWidget::previousSong);
    connect(nextButton, &QPushButton::clicked,
            this, &PlayerWidget::nextSong);
    connect(volumeSlider, &QSlider::valueChanged,
            this, &PlayerWidget::setVolume);
    connect(clockTimer, &QTimer::timeout,
            this, &PlayerWidget::updateDateTime);
    connect(playbackTimer, &QTimer::timeout,
            this, &PlayerWidget::tickPlayback);
}

void PlayerWidget::togglePlayPause()
{
    isPlaying = !isPlaying;
    playButton->setText(isPlaying ? "Pause" : "Play");
}

void PlayerWidget::updateDateTime()
{
    currentDateTime->setDateTime(QDateTime::currentDateTime());
}

void PlayerWidget::previousSong()
{
    currentSong = new QLabel("PrevSong", this);
    currentSeconds = 0;
    updateTrackDisplay();
    updateVisualizer();
}

void PlayerWidget::nextSong()
{
    currentSong = new QLabel("NextSong", this);
    currentSeconds = 0;
    updateTrackDisplay();
    updateVisualizer();
}

void PlayerWidget::setVolume(int)
{
    updateVisualizer();
}

void PlayerWidget::tickPlayback()
{
    if(!isPlaying){
        return;
    }

    if(++playbackTickCount % 4 == 0) {
        if(++currentSeconds > trackLengthSeconds){
            currentSeconds = 0;
            nextSong();
            return;
        }
    }

    updateTrackDisplay();
    updateVisualizer();
}

void PlayerWidget::updateTrackDisplay()
{
    const int min = currentSeconds / 60;
    const int sec = currentSeconds % 60;

    playTime->display(QString("%1:%2")
                          .arg(min, 2, 10, QChar('0'))
                          .arg(sec, 2, 10, QChar('0')));
}

void PlayerWidget::updateVisualizer()
{
    QGraphicsScene *scene = visualizer->scene();
    scene->clear();

    const int barCount = 20;
    const int barWidth = 12;
    const int spacing = 6;
    const int maxHeight = 100;
    const int baseY = 110;

    int x = 10;

    for(int i = 0 ; i < barCount; ++i){
        int height = isPlaying
                         ? QRandomGenerator::global()->bounded(15, maxHeight) : 10;

        scene->addRect(x, baseY - height, barWidth, height);
        x += barWidth + spacing;
    }

    scene->setSceneRect(0, 0, x + 10, 130);
}