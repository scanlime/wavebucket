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
    // oh the horror
    memmove(xxxDebugBuffer, xxxDebugBuffer+1, sizeof(xxxDebugBuffer)-1);

    double exposure = xxxExposure * 2e-4;

    // For each frame, mix down to mono.
    while (frames--) {
        int mono = 0;
        for (int c = channels; c; --c) {
            mono += *(samples++);
        }

        // Update feedback-form comb filter (resonant)
        for (int y = 0; y < xxxDebugHeight; ++y) {
            int c = combFeedback[y][-y];
            int comb =  c - (c / 8) + mono; 
            combFeedback[y].push(comb);
        }
    }

    // Integrate the energy in each comb filter's buffer
    for (int y = 0; y < xxxDebugHeight; ++y) {

            long energy = 0;
            for (int j = 0; j < y*2; ++j) {
                int x = combFeedback[y][-j];
                energy += x * x;
            }

            uint8_t luma = std::max(0.001, std::min(255.99, sqrt(energy) * exposure));

            xxxDebugBuffer[(y+1) * xxxDebugWidth - 1] = luma;
    }
}

}  // namespace wb
