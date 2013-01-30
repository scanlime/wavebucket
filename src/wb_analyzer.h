/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <stdint.h>
#include "wb_biquad.h"
#pragma once

namespace wb {

class Analyzer {
public:
    Analyzer(unsigned sampleRate);

    /// Handle input PCM data. Expects (channels * frames * sizeof(int16_t) bytes of data.
    void pcmInput(const int16_t *samples, unsigned channels, unsigned frames);

    static const unsigned xxxDebugWidth = 1024;
    static const unsigned xxxDebugHeight = 512;
    uint8_t xxxDebugBuffer[xxxDebugWidth * xxxDebugHeight];
    double xxxExposure;

private:
    unsigned sampleRate;

    IIRGammatone filterSpec[1024];
    BiquadChain<4> filterState[1024];
};

}  // namespace wb
