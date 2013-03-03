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
#include "ledcomm.h"


namespace wb {

static ffft::FFTRealFixLen<11> fftr;
static int counter, counter2;
static int F0i;
static float F0v;
static float phs;
static float amplitude;
static float sums[4096];
static BiquadChain<4> filterState;
static IIRGammatone filter;
static LEDComm leds;
static Ring<int16_t, 1<<20> delayline;

// compensating for processing, radio, and motor delays
static const int delay = 5000;

Analyzer::Analyzer(unsigned sampleRate)
    : xxxExposure(1.0), sampleRate(sampleRate)
{
    filter.init(100, sampleRate, 1e3);
    filterState.init();
}

float Analyzer::autotune(float hz)
{
    double refHz = 440.0;
    double spacing = 1.0594630943592953;
    return refHz * pow(spacing, round(log(hz / refHz) / log(spacing)));
}

void Analyzer::pcmSynth(int16_t *samples, unsigned channels, unsigned frames)
{
    double f0hz = F0i ? sampleRate / F0i : 0;
    f0hz = autotune(f0hz);
    double phsm = M_PI * 2.0 / sampleRate * f0hz;

    while (frames--) {
        int mono;

        phs = fmod(phs + phsm, 2 * M_PI);
        float targetA = F0v * 1e-9;
        amplitude += (targetA - amplitude) * 0.001;
        amplitude = std::min<float>(0x8000, amplitude);
        mono = sinf(phs) * amplitude;

        // xxx delay
        mono = delayline[int(-delay-frames)];

        for (int c = channels; c; --c) {
            *(samples++) = mono;
        }
    }
}

void Analyzer::pcmInput(const int16_t *samples, unsigned channels, unsigned frames)
{
    // For each frame, mix down to mono.
    while (frames--) {
        int mono = 0;
        for (int c = channels; c; --c) {
            mono += *(samples++);
        }
        delayline.push(mono/2);

        //timeDomain.push(filterState.next(filter.stages, mono) * 1e12);
        timeDomain.push(mono);

        const int halfLength = fftr.get_length()/2;

        static float v1[4096];
        static float v2[4096];

        if (++counter2 == 32) {
            counter2 = 0;


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
                sums[x] += v2[x];
            }
        }

        if (++counter == 1500) {
            counter = 0;

            memmove(xxxDebugBuffer, xxxDebugBuffer+1, sizeof xxxDebugBuffer - 1);

            for (int x = 0; x < xxxDebugHeight; ++x) {
                int i = x*2;
                int luma = std::min<float>(255, std::max<float>(0.0, sqrt(sums[i]) * 1e-6 * xxxExposure));
                xxxDebugBuffer[(x+1) * xxxDebugWidth - 1] = luma;

                int j = i/2;
                if (j < 240)
                    leds.setPixel(j, luma | (luma << 8) | (luma << 16));
            }

            memset(sums, 0, sizeof sums);

            leds.update();

            printf("poke\n");
        }
    }
}


}  // namespace wb
