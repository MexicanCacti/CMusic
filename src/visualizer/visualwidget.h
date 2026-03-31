#ifndef VISUALWIDGET_H
#define VISUALWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector>
#include <QTimer>
#include <QOpenGLShaderProgram>
#include <QTMath>
#include <QFileDialog>
#include <QElapsedTimer>

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
    void findShaderSource();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private slots:
    void onFrameTick();

private:
    void initShader(QString vertShaderSource, QString fragShaderSource);
    void ensureDefaultData();
    const QString defaultVertShader = ":/resources/vertexShaders/default.vert";
    const QString defaultFragShader = ":/resources/fragmentShaders/default.frag";
    QString currentVertShader = defaultVertShader;
    QString currentFragShader = defaultFragShader;
    QOpenGLShaderProgram *shaderProgram;
    QTimer *frameTimer;
    QVector<float> fftBins;
    QVector<float> hashParams;
    float playbackTime;
    float level;
    bool isPlaying;
    QByteArray loadShaderSource(QString filePath);
    QElapsedTimer elapsedTimer;
    qint64 lastFrameMs = 0;
};

#endif // VISUALWIDGET_H
