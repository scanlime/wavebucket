/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include "wb_analyzer.h"

namespace wb {

Analyzer::Analyzer(unsigned sampleRate)
    : sampleRate(sampleRate)
    {}

void Analyzer::pcmInput(const int16_t *samples, unsigned channels, unsigned frames)
{
    // For each frame, mix down to mono.
    while (frames--) {
        int mono = 0;
        for (int c = channels; c; --c) {
            mono += *(samples++);
        }

        static int y = 0;
        xxxDebugBuffer[y++] = 127 + mono / 512; 
        if (y == sizeof xxxDebugBuffer) y = 0;
    }
}

}  // namespace wb
