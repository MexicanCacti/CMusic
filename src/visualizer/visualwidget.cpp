#include "VisualWidget.h"

VisualWidget::VisualWidget(QWidget* parent)
    :QOpenGLWidget(parent),
    shaderProgram(nullptr),
    frameTimer(new QTimer(this)),
    playbackTime(0.0f),
    level(0.6f),
    isPlaying(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //ensureDefaultData();

    connect(frameTimer, &QTimer::timeout,
            this, &VisualWidget::onFrameTick);

    frameTimer->setTimerType(Qt::PreciseTimer);
    frameTimer->start(16);
}


void VisualWidget::setFFTBins(const QVector<float> &bins)
{
    fftBins.fill(0.0f, 64);

    const int count = qMin(64, bins.size());
    for(int i = 0 ; i < count; ++i){
        fftBins[i] = std::clamp(bins[i], 0.0f, 1.0f);
    }

    update();
}

void VisualWidget::setHashParams(const QVector<float>& params)
{
    hashParams.fill(0.0f, 8);
    const int count = qMin(8, params.size());
    for(int i = 0 ; i < count; ++i){
        hashParams[i] = std::clamp(params[i], 0.0f, 1.0f);
    }

    update();
}

void VisualWidget::setPlaying(bool playing)
{
    isPlaying = playing;
    update();
}

void VisualWidget::setLevel(int level)
{
    this->level = std::clamp(level / 100.0f, 0.0f, 1.0f);
    update();
}

void VisualWidget::setPlaybackTime(float seconds)
{
    playbackTime = seconds;
    update();
}

void VisualWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    initShader(defaultVertShader, defaultFragShader);
}

void VisualWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void VisualWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if(!shaderProgram) return;

    shaderProgram->bind();

    shaderProgram->setUniformValue("u_resolution",
                                   QVector2D(float(width()), float(height())));
    shaderProgram->setUniformValue("u_time", playbackTime);
    shaderProgram->setUniformValue("u_level", level);
    shaderProgram->setUniformValue("u_playing", isPlaying ? 1.0f : 0.0f);

    if (!fftBins.isEmpty()) {
        shaderProgram->setUniformValueArray("u_fft", fftBins.constData(), fftBins.size(), 1);
    }

    if (!hashParams.isEmpty()) {
        shaderProgram->setUniformValueArray("u_hashParams", hashParams.constData(), hashParams.size(), 1);
    }

    glDrawArrays(GL_TRIANGLES, 0, 3);

    shaderProgram->release();
}

void VisualWidget::onFrameTick()
{
    qint64 now = elapsedTimer.elapsed();
    float delta = float(now - lastFrameMs) / 1000.0f;
    lastFrameMs = now;

    if(isPlaying) playbackTime += delta;
    update();
}

void VisualWidget::findShaderSource()
{
    QString vertShader = QFileDialog::getOpenFileName(
        this,
        tr("Open Vertex Shader File"),
        QString(),
        tr("Vertex Shader Files (*.vert);;All Files (*)")
        );

    if (vertShader.isEmpty()) {
        vertShader = currentVertShader;
    }
    currentVertShader = vertShader;

    QString fragShader = QFileDialog::getOpenFileName(
        this,
        tr("Open Fragment Shader File"),
        QString(),
        tr("Fragment Shader Files (*.frag);;All Files (*)")
        );

    if (fragShader.isEmpty()) {
        fragShader = currentFragShader;
    }
    currentFragShader = fragShader;

    initShader(vertShader, fragShader);
}

void VisualWidget::reloadShaders()
{
    initShader(currentVertShader, currentFragShader);
}

QByteArray VisualWidget::loadShaderSource(QString filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        return QByteArray();
    }

    QByteArray source = file.readAll();
    return source;
}

void VisualWidget::initShader(QString vertShaderSource, QString fragShaderSource)
{
    delete shaderProgram;
    shaderProgram = new QOpenGLShaderProgram(this);

    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, loadShaderSource(vertShaderSource))) {
        qWarning("Vertex shader compilation failed, trying default...");
        if(!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, loadShaderSource(defaultVertShader))){
            qWarning("Vertex shader compilation failed");
            return;
        }
    }

    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, loadShaderSource(fragShaderSource))) {
        qWarning("Fragment shader compilation failed, trying default...");
        if(!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, loadShaderSource(defaultFragShader))){
            qWarning("Fragment shader compilation failed");
            return;
        }
    }

    currentVertShader = vertShaderSource;
    currentFragShader = fragShaderSource;

    if (!shaderProgram->link()) {
        qWarning("Shader program link failed");
        return;
    }
    update();
}

void VisualWidget::ensureDefaultData()
{
    fftBins.resize(64);
    for (int i = 0; i < fftBins.size(); ++i) {
        const float x = float(i) / float(fftBins.size() - 1);
        fftBins[i] = 0.15f + 0.1f * std::sin(x * 12.0f);
    }

    hashParams = {
        0.12f, 0.45f, 0.76f, 0.31f,
        0.62f, 0.24f, 0.88f, 0.53f
    };
}

VisualWidget::~VisualWidget(){
    delete shaderProgram;
}
