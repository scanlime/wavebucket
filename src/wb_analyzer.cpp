/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ffft/FFTRealFixLen.h"
#include "wb_analyzer.h"
#include "wb_biquad.h"


namespace wb {

static ffft::FFTRealFixLen<10> fftr;
static int counter, counter2;
static int F0i;
static float F0v;
static float sums[4096];

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

        const int halfLength = fftr.get_length()/2;

        if (++counter2 == 32) {
            counter2 = 0;

            float v1[fftr.get_length()];
            float v2[fftr.get_length()];

            for (int x = 0; x < fftr.get_length(); x++) {
                v1[x] = timeDomain[-x];
            }

            fftr.do_fft(v2, v1);

            // Power spectrum
            for (int x = 0; x != halfLength; ++x) {
                float r = v2[x];
                float i = v2[x + halfLength];
                v2[x] = r*r + i*i;
                v2[x + halfLength] = 0;
            }

            fftr.do_ifft(v2, v1);

            for (int x = 0; x < halfLength; ++x) {
                sums[x] += v1[x];
            }
        }

        if (++counter == 1024) {
            counter = 0;

            int maxI = 0;
            float maxV = 0;
            for (int x = 1; x < halfLength; ++x) {
                float v = sums[x];

                // Sticky
                if (x == F0i) v *= 1.1;

                if (v > maxV && v > sums[x-1] && v > sums[x+1]) {
                    maxI = x;
                    maxV = v;
                }
            }

            F0i = maxI;
            F0v = maxV;

            bool visible = F0v > 1e10;
            if (visible) {
             //   printf("%d, F0 = %.02f Hz, power %f\n", F0i, 44100.0 / F0i, F0v);
            }

            memmove(xxxDebugBuffer, xxxDebugBuffer+1, sizeof xxxDebugBuffer - 1);

            for (int x = 0; x < xxxDebugHeight; ++x) {
                int luma = std::min<float>(200, std::max<float>(0.0, sqrt(sums[x]) * 2e-5 * xxxExposure));
                if (visible && abs(F0i - x) < 2)
                    luma = 255;
                xxxDebugBuffer[(x+1) * xxxDebugWidth - 1] = luma;
            }

            memset(sums, 0, sizeof sums);
        }
    }
}


}  // namespace wb
