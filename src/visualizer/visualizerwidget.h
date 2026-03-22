#ifndef VISUALIZERWIDGET_H
#define VISUALIZERWIDGET_H

#include <QWidget>
#include <QVector>
#include <QTimer>
#include <QPainter>
#include <QtGlobal>
#include <QRandomGenerator>

class VisualizerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VisualizerWidget(QWidget *parent = nullptr);

    void setPlaying(bool playing);
    void setLevel(int level);
    void setBarCount(int count);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateBars();

private:
    QVector<int> visBars;
    QTimer *visTimer;
    bool visIsPlaying;
    int visLevel;
    int visBarCount;
};

#endif // VISUALIZERWIDGET_H
