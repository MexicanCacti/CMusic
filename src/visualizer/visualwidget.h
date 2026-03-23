#ifndef VISUALWIDGET_H
#define VISUALWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector>
#include <QTimer>
#include <QOpenGLShaderProgram>
#include <QTMath>
#include <algorithm>

class VisualWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit VisualWidget(QWidget *parent = nullptr);
    ~VisualWidget() override;

    void setPlaybackTime(float seconds);
    void setFFTBins(const QVector<float> &bins);
    void setHashParams(const QVector<float>& params);
    void setPlaying(bool playing);
    void setLevel(int level);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private slots:
    void onFrameTick();

private:
    void initShader();
    void ensureDefaultData();

    QOpenGLShaderProgram *shaderProgram;
    QTimer *frameTimer;
    QVector<float> fftBins;
    QVector<float> hashParams;
    float playbackTime;
    float level;
    bool isPlaying;
};

#endif // VISUALWIDGET_H
