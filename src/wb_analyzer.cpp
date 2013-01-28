/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "wb_analyzer.h"

namespace wb {

Analyzer::Analyzer(unsigned sampleRate)
    : xxxExposure(1.0), sampleRate(sampleRate)
{}

void Analyzer::pcmInput(const int16_t *samples, unsigned channels, unsigned frames)
{
    static int y = 0;
    static float sums[xxxDebugWidth];
    memset(sums, 0, sizeof sums);

    // For each frame, mix down to mono.
    while (frames--) {
        int mono = 0;
        for (int c = channels; c; --c) {
            mono += *(samples++);
        }

        // Update integral ring buffer
        ring.push(mono);

        if ((frames & 7) == 0) {
            // Wavelets for each frequency band
            for (unsigned x = 0; x < xxxDebugWidth; ++x) {

                float hz = 13.75 * pow(2, x * (1.0 / 96.0));
                float samplesPerCycle = sampleRate / hz;

                float p1 = -1800;
                float p2 = p1 + samplesPerCycle * 0.5;
                float p0 = p1 - samplesPerCycle * 0.5;

                //printf("%d %f %f %f\n", x, p0, p1, p2);

                int64_t r = - ring.integrate(p0, p1) + ring.integrate(p1, p2);
                int64_t c =   ring.integrate(p0, p1) - ring.integrate(p1, p2);

                sums[x] += (r*r + c*c) / (samplesPerCycle * samplesPerCycle) * 1e-6;
            }
        }
    }

    // Downsampled summary scaleogram
    for (unsigned x = 0; x < xxxDebugWidth; ++x) {
        int luma = std::min<int>(255.0, std::max<int>(0.0, sums[x] * xxxExposure));
        xxxDebugBuffer[y * xxxDebugWidth + x] = luma;
    }
    y++;
    if (y == xxxDebugHeight) y = 0;
}

}  // namespace wb
