/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <stdint.h>
#include "wb_ring.h"
#pragma once

namespace wb {

class Analyzer {
public:
    Analyzer(unsigned sampleRate);

    /// Handle input PCM data. Expects (channels * frames * sizeof(int16_t) bytes of data.
    void pcmInput(const int16_t *samples, unsigned channels, unsigned frames);

    // xxx
    void pcmSynth(int16_t *samples, unsigned channels, unsigned frames);

    static const unsigned xxxDebugWidth = 1024;
    static const unsigned xxxDebugHeight = 512;
    uint8_t xxxDebugBuffer[xxxDebugWidth * xxxDebugHeight];
    double xxxExposure;

private:
    unsigned sampleRate;

    float autotune(float hz);

    Ring<float, 16384> timeDomain;
};

}  // namespace wb
