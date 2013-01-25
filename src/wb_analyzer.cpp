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
    static int x = 0;

    // For each frame, mix down to mono.
    while (frames--) {
        int mono = 0;
        for (int c = channels; c; --c) {
            mono += *(samples++);
        }

        monoTimeDomain.push(mono);

        if (!(frames & 127)) {
            for (int y = 0; y < xxxDebugHeight; ++y) {
                int combWidth = y;

                double monoE = 0;
                double combE = 0;
                
                // energy integrator
                for (int j = 0; j < combWidth * 2; j++) {
                    int td = monoTimeDomain[-j];

                    // comb filter
                    int comb = td + monoTimeDomain[-j-combWidth];

                    double tdF = td;
                    double combF = comb;

                    monoE += tdF < 0 ? -tdF : tdF;
                    combE += combF < 0 ? -combF : combF;
                }

                double energy = std::max(0.0, monoE - combE) / combWidth;
                uint8_t luma = std::max(0.001, std::min(255.99, energy * xxxExposure));

                xxxDebugBuffer[x + y * xxxDebugWidth] = luma;
            }
            if (++x == xxxDebugWidth) x = 0;
        }
    }
}

}  // namespace wb
