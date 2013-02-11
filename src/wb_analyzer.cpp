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

Analyzer::Analyzer(unsigned sampleRate)
    : xxxExposure(1.0), sampleRate(sampleRate)
{}

void Analyzer::pcmInput(const int16_t *samples, unsigned channels, unsigned frames)
{
    // For each frame, mix down to mono.
    while (frames--) {
        int mono = 0;
        for (int c = channels; c; --c) {
            mono += *(samples++);
        }
        timeDomain.push(mono);

        if ((frames & 127) == 0) {
            memmove(xxxDebugBuffer + xxxDebugWidth, xxxDebugBuffer, sizeof xxxDebugBuffer - xxxDebugWidth);
            for (int x = 0; x < xxxDebugWidth; ++x) {
                float acf = 0;

                for (int win = 0; win < 1024; win++) {
                    acf += timeDomain[-win] * timeDomain[-win-x*2];
                }

                int luma = std::min<float>(255.0, std::max<float>(0.0, acf * 1e-10 * xxxExposure));
                xxxDebugBuffer[x] = luma;
            }
        }
    }
}


}  // namespace wb
