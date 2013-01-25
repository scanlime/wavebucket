/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include "wb_analyzer.h"

namespace wb {

Analyzer::Analyzer(unsigned sampleRate)
    : sampleRate(sampleRate)
    {}

void Analyzer::pcmInput(const int16_t *samples, unsigned channels, unsigned frames)
{
    // xxx
    memset(xxxDebugBuffer, 0x10, sizeof xxxDebugBuffer);
    memcpy(xxxDebugBuffer, samples, frames*channels*2);
}

}  // namespace wb
