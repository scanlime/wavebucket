/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <stdint.h>
#pragma once

namespace wb {

class Analyzer {
public:
    Analyzer(unsigned sampleRate);

    /// Handle input PCM data. Expects (channels * frames * sizeof(int16_t) bytes of data.
    void pcmInput(const int16_t *samples, unsigned channels, unsigned frames);

    static const unsigned xxxDebugWidth = 256;
    static const unsigned xxxDebugHeight = 256;
    uint32_t xxxDebugBuffer[xxxDebugWidth * xxxDebugHeight];

private:
    int sampleRate;
};

}  // namespace wb
