// VisualizerWidget.cpp
#include "visualizerwidget.h"
#include <algorithm>

VisualizerWidget::VisualizerWidget(QWidget *parent)
    : QWidget(parent),
    visTimer(new QTimer(this)),
    visIsPlaying(false),
    visLevel(60),
    visBarCount(20)
{
    setMinimumHeight(120);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    visBars.resize(visBarCount, 10);

    connect(visTimer, &QTimer::timeout, this, &VisualizerWidget::updateBars);
    visTimer->start(80);
}

void VisualizerWidget::setPlaying(bool playing)
{
    visIsPlaying = playing;
    updateBars();
}

void VisualizerWidget::setLevel(int level)
{
    visLevel = std::clamp(level, 0, 100);
    update();
}

void VisualizerWidget::setBarCount(int count)
{
    if (count <= 0) {
        return;
    }

    visBarCount = count;
    visBars.resize(visBarCount);
    updateBars();
}

void VisualizerWidget::updateBars()
{
    for (int i = 0; i < visBarCount; ++i) {
        if (visIsPlaying) {
            int minHeight = 8;
            int maxHeight = 20 + visLevel;
            visBars[i] = QRandomGenerator::global()->bounded(minHeight, maxHeight + 1);
        } else {
            visBars[i] = 8;
        }
    }

    update();
}

void VisualizerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), palette().window());

    if (visBars.isEmpty()) {
        return;
    }

    const int margin = 10;
    const int spacing = 6;
    const int availableWidth = width() - (2 * margin);
    const int totalSpacing = spacing * (visBars.size() - 1);
    const int barWidth = qMax(4, (availableWidth - totalSpacing) / visBars.size());
    const int baseLine = height() - 10;

    int x = margin;

    for (int heightIndex = 0 ; heightIndex < visBarCount; ++heightIndex) {
        int heightValue = visBars[heightIndex];
        QRect barRect(x, baseLine - heightValue, barWidth, heightValue);
        painter.drawRect(barRect);
        x += barWidth + spacing;
    }
}