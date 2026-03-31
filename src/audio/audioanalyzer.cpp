#include "AudioAnalyzer.h"

#include <QAudioFormat>
#include <QtMath>
#include <algorithm>
#include <cmath>

AudioAnalyzer::AudioAnalyzer(QObject* parent)
    : QObject(parent),
    window(FFT_SIZE),
    fftIn(FFT_SIZE),
    fftOut(FFT_SIZE / 2 + 1)
{
    for (int i = 0; i < FFT_SIZE; ++i) {
        window[i] = 0.5f * (1.0f - std::cos((2.0 * M_PI * i) / (FFT_SIZE - 1)));
    }

    initFFT();
}

AudioAnalyzer::~AudioAnalyzer()
{
    cleanUpFFT();
}

void AudioAnalyzer::initFFT()
{
    fftCfg = kiss_fftr_alloc(FFT_SIZE, 0, nullptr, nullptr);
}

void AudioAnalyzer::cleanUpFFT()
{
    if (fftCfg) {
        free(fftCfg);
        fftCfg = nullptr;
    }
}

float AudioAnalyzer::clamp01(float v)
{
    return std::max(0.0f, std::min(1.0f, v));
}

void AudioAnalyzer::pushSamples(const float* samples, int count)
{
    for (int i = 0; i < count; ++i) {
        sampleQueue.push_back(samples[i]);
    }

    while (static_cast<int>(sampleQueue.size()) >= FFT_SIZE) {
        analyzeWindow();

        for (int i = 0; i < HOP_SIZE && !sampleQueue.empty(); ++i) {
            sampleQueue.pop_front();
        }
    }
}

QVector<float> AudioAnalyzer::makeBins(const QVector<float>& magnitudes, int outputBins)
{
    QVector<float> bins(outputBins, 0.0f);

    const int startBin = 1;
    const int endBin = magnitudes.size() - 1;

    for (int i = 0; i < outputBins; ++i) {
        float t0 = float(i) / outputBins;
        float t1 = float(i + 1) / outputBins;

        int b0 = startBin + int(std::pow(t0, 2.0f) * (endBin - startBin));
        int b1 = startBin + int(std::pow(t1, 2.0f) * (endBin - startBin));

        b1 = std::max(b1, b0 + 1);
        b1 = std::min(b1, endBin);

        float acc = 0.0f;
        int n = 0;
        for (int b = b0; b < b1; ++b) {
            acc += magnitudes[b];
            ++n;
        }

        float v = (n > 0) ? (acc / float(n)) : 0.0f;

        v = std::log1p(v * 20.0f) / std::log1p(20.0f);
        bins[i] = clamp01(v);
    }

    return bins;
}

void AudioAnalyzer::analyzeWindow()
{
    float rmsAcc = 0.0f;

    for (int i = 0; i < FFT_SIZE; ++i) {
        float s = sampleQueue[i];
        fftIn[i] = s * window[i];
        rmsAcc += s * s;
    }

    kiss_fftr(fftCfg, fftIn.constData(), fftOut.data());

    float rms = std::sqrt(rmsAcc / FFT_SIZE);
    emit levelReady(clamp01(rms * 3.0f));

    QVector<float> magnitudes(FFT_SIZE / 2 + 1, 0.0f);
    for (int i = 0; i < magnitudes.size(); ++i) {
        float re = fftOut[i].r;
        float im = fftOut[i].i;
        magnitudes[i] = std::sqrt(re * re + im * im) / float(FFT_SIZE);
    }

    emit fftReady(makeBins(magnitudes, OUTPUT_BINS));
}

void AudioAnalyzer::processBuffer(const QAudioBuffer& buffer)
{
    if (!buffer.isValid() || buffer.frameCount() <= 0) {
        return;
    }

    const QAudioFormat format = buffer.format();
    const int channels = format.channelCount();
    if (channels <= 0) {
        return;
    }

    QVector<float> mono;
    mono.reserve(buffer.frameCount());

    switch (format.sampleFormat()) {
    case QAudioFormat::Float: {
        const float* data = buffer.constData<float>();
        if (!data) return;

        for (qsizetype frame = 0; frame < buffer.frameCount(); ++frame) {
            float sum = 0.0f;
            for (int ch = 0; ch < channels; ++ch) {
                sum += data[frame * channels + ch];
            }
            mono.push_back(sum / float(channels));
        }
        break;
    }

    case QAudioFormat::Int16: {
        const qint16* data = buffer.constData<qint16>();
        if (!data) return;

        for (qsizetype frame = 0; frame < buffer.frameCount(); ++frame) {
            float sum = 0.0f;
            for (int ch = 0; ch < channels; ++ch) {
                sum += float(data[frame * channels + ch]) / 32768.0f;
            }
            mono.push_back(sum / float(channels));
        }
        break;
    }

    case QAudioFormat::Int32: {
        const qint32* data = buffer.constData<qint32>();
        if (!data) return;

        for (qsizetype frame = 0; frame < buffer.frameCount(); ++frame) {
            float sum = 0.0f;
            for (int ch = 0; ch < channels; ++ch) {
                sum += float(data[frame * channels + ch]) / 2147483648.0f;
            }
            mono.push_back(sum / float(channels));
        }
        break;
    }

    case QAudioFormat::UInt8: {
        const quint8* data = buffer.constData<quint8>();
        if (!data) return;

        for (qsizetype frame = 0; frame < buffer.frameCount(); ++frame) {
            float sum = 0.0f;
            for (int ch = 0; ch < channels; ++ch) {
                sum += (float(data[frame * channels + ch]) - 128.0f) / 128.0f;
            }
            mono.push_back(sum / float(channels));
        }
        break;
    }

    default:
        return;
    }

    if (!mono.isEmpty()) {
        pushSamples(mono.constData(), mono.size());
    }
}

void AudioAnalyzer::reset()
{
    sampleQueue.clear();
}