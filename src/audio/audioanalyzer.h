#ifndef AUDIOANALYZER_H
#define AUDIOANALYZER_H

#include <QObject>
#include <QAudioBuffer>
#include <QVector>
#include <QThread>
#include <deque>

extern "C" {
#include "kiss_fftr.h"
}

class AudioAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit AudioAnalyzer(QObject* parent = nullptr);
    ~AudioAnalyzer() override;

    void doAnalyze();

public slots:
    void processBuffer(const QAudioBuffer& buffer);
    void reset();

signals:
    void fftReady(const QVector<float>& bins);
    void levelReady(int level);

private:
    void initFFT();
    void cleanUpFFT();
    void pushSamples(const float* samples, int count);
    void analyzeWindow();
    QVector<float> makeBins(const QVector<float>& magnitudes, int outputBins);

    static float clamp01(float v);

    static constexpr int FFT_SIZE = 2048;
    static constexpr int HOP_SIZE = 512;
    static constexpr int OUTPUT_BINS = 64;

    std::deque<float> sampleQueue;
    QVector<float> window;

    kiss_fftr_cfg fftCfg = nullptr;
    QVector<float> fftIn;
    QVector<kiss_fft_cpx> fftOut;
};

#endif // AUDIOANALYZER_H
