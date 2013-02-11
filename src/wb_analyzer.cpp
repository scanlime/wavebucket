/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "wb_analyzer.h"
#include "wb_biquad.h"
#include "kiss_fftr.h"

namespace wb {

static kiss_fftr_cfg kfft;
static kiss_fftr_cfg kffti;
static int counter;

Analyzer::Analyzer(unsigned sampleRate)
    : xxxExposure(1.0), sampleRate(sampleRate)
{
    kfft = kiss_fftr_alloc(4096, 0, 0, 0);
    kffti = kiss_fftr_alloc(4096, 1, 0, 0);
}

void Analyzer::pcmInput(const int16_t *samples, unsigned channels, unsigned frames)
{
    // For each frame, mix down to mono.
    while (frames--) {
        int mono = 0;
        for (int c = channels; c; --c) {
            mono += *(samples++);
        }
        timeDomain.push(mono);

        if (++counter == 160) {
            counter = 0;

            kiss_fft_scalar timedata[4096];
            kiss_fft_cpx freqdata[4096];

            for (int x = 0; x < 4096; x++) {
                timedata[x] = timeDomain[-x];
            }

            kiss_fftr(kfft, timedata, freqdata);

            for (int x = 0; x < 4096; x++) {
                freqdata[x].r = freqdata[x].r * freqdata[x].r + freqdata[x].i * freqdata[x].i;
                freqdata[x].i = 0;
            }

            kiss_fftri(kffti, freqdata, timedata);

            memmove(xxxDebugBuffer + xxxDebugWidth, xxxDebugBuffer, sizeof xxxDebugBuffer - xxxDebugWidth);

            int maxI = 0;
            float maxV = 0;
            for (int x = 1; x < xxxDebugWidth; ++x) {
                float v = timedata[x];
                if (v > maxV && timedata[x] > timedata[x-1] && timedata[x] > timedata[x+1]) {
                    maxI = x;
                    maxV = v;
                }
            }
            printf("%d, F0 = %.02f Hz\n", maxI, 44100.0 / maxI);

            for (int x = 0; x < xxxDebugWidth; ++x) {
                int luma = std::min<float>(127.0, std::max<float>(0.0, timedata[x] * 1e-10 * xxxExposure));
                if (maxI == x)
                    luma = 255;
                xxxDebugBuffer[x] = luma;
            }
        }
    }
}


}  // namespace wb
