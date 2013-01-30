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

namespace wb {

static float gains[1024];

Analyzer::Analyzer(unsigned sampleRate)
    : xxxExposure(1.0), sampleRate(sampleRate)
{
    for (unsigned band = 0; band < 1024; ++band) {
        float hz = 13.75 * pow(2, band * (1.0 / 96.0));
        filterSpec[band].init(hz, sampleRate);
        filterState[band].init();

        gains[band] = exp(15 + band * 0.01228139166210260);

/*
        printf("Band %d: %f Hz, a0=%f a1=%f b1=%f b2=%f\n",
            band, hz, filterSpec[band].stages[0].a0, filterSpec[band].stages[0].a1,
            filterSpec[band].stages[0].b1, filterSpec[band].stages[0].b2);
*/
    }
}

void Analyzer::pcmInput(const int16_t *samples, unsigned channels, unsigned frames)
{
    static double sums[1024];
    memset(sums, 0, sizeof sums);

    // For each frame, mix down to mono.
    while (frames--) {
        int mono = 0;
        for (int c = channels; c; --c) {
            mono += *(samples++);
        }

        // IIR for each frequency band
        for (unsigned band = 0; band < 1024; ++band) {
            double r = gains[band] * filterState[band].next(filterSpec[band].stages, mono);
            sums[band] += r * r;
        }

        if ((frames & 127) == 0) {
            memmove(xxxDebugBuffer + xxxDebugWidth, xxxDebugBuffer, sizeof xxxDebugBuffer - xxxDebugWidth);
            for (unsigned x = 0; x < xxxDebugWidth; ++x) {

                int luma = std::min<float>(255.0, std::max<float>(0.0, log(sums[x]) * 10 * xxxExposure));
                //int luma = std::min<float>(255.0, std::max<float>(0.0, sums[x] * 1e-5 * xxxExposure));

                /*
                if ((x % 96) == 0)
                    luma ^= 0xff;
                else if ((x & 7) == 0)
                    luma ^= 0x40;
                */

                xxxDebugBuffer[x] = luma;
            }
            memset(sums, 0, sizeof sums);
        }
    }
}


}  // namespace wb
