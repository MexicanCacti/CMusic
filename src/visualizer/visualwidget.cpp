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

    ensureDefaultData();

    connect(frameTimer, &QTimer::timeout,
            this, &VisualWidget::onFrameTick);

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
    level = std::clamp(level / 100.0f, 0.0f, 1.0f);
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
    initShader();
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
    if(isPlaying) playbackTime += 0.16f;
    update();
}

void VisualWidget::initShader()
{
    delete shaderProgram;
    shaderProgram = new QOpenGLShaderProgram(this);

    static const char *vertexSource = R"(
        #version 330 core

        out vec2 v_uv;

        void main()
        {
            vec2 positions[3] = vec2[](
                vec2(-1.0, -1.0),
                vec2( 3.0, -1.0),
                vec2(-1.0,  3.0)
            );

            vec2 pos = positions[gl_VertexID];
            v_uv = pos * 0.5 + 0.5;

            gl_Position = vec4(pos, 0.0, 1.0);
        }
)";

    static const char *fragmentSource = R"(
        #version 330 core

        in vec2 v_uv;
        out vec4 fragColor;

        uniform vec2 u_resolution;
        uniform float u_time;

        void main()
        {
            vec2 p = v_uv * 2.0 - 1.0;
            p.x *= u_resolution.x / u_resolution.y;

            float r = length(p) * 0.2f;
            float a = atan(p.y, p.x);

            a += cos(u_time);

            float spiral = sin(16.0 * r - 5.0 * a);
            float line = smoothstep(0.12, 0.0, sin(spiral));

            float fade = exp(-1.8 * r);
            float glow = line * fade;

            vec3 base = vec3(0.1, 0.2, 0.8);
            vec3 highlight = vec3(1.0, sin(u_time * a), cos(u_time * 0.2));
            vec3 color = mix(base, highlight, r + 0.3 * tan(u_time * a));

            fragColor = vec4(color * glow, 1.0);
        }
    )";

    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource)) {
        qWarning("Vertex shader compilation failed");
        return;
    }

    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSource)) {
        qWarning("Fragment shader compilation failed");
        return;
    }

    if (!shaderProgram->link()) {
        qWarning("Shader program link failed");
        return;
    }
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
