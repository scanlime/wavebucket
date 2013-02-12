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

static ffft::FFTRealFixLen<11> fftr;
static int counter, counter2;
static int F0i;
static float F0v;
static float phs;
static float amplitude;
static float sums[4096];

static float weights[4096];

Analyzer::Analyzer(unsigned sampleRate)
    : xxxExposure(1.0), sampleRate(sampleRate)
{
    // XXX iso226
    static const float f[] = {
        20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800,
        1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500
    };

    static const float lu[] = {
       -31.6, -27.2, -23.0, -19.1, -15.9, -13.0, -10.3, -8.1, -6.2, -4.5, -3.1,
       -2.0,  -1.1,  -0.4,   0.0,   0.3,   0.5,   0.0, -2.7, -4.1, -1.0,  1.7,
        2.5,   1.2,  -2.1,  -7.1, -11.2, -10.7,  -3.1
    };

    weights[0] = 1;
    for (int i = 1; i < 4096; ++i) {
        float hz = float(sampleRate) / i;

        // Nearest table entries
        int t0 = 0;
        for (int j = 0; j < 29; ++j) {
            if (abs(f[j] - hz) < abs(f[t0] - hz) && f[j] <= hz) t0 = j;
        }

        // Log-linear Interpolate
        float a = (log(hz) - log(f[t0])) / (log(f[t0+1]) - log(f[t0]));
        float db = lu[t0] + a * (lu[t0+1] - lu[t0]);
        weights[i] = pow(10.0, db/10.0);
        //printf("%d %f [%d] %f\n", i, hz, t0, weights[i]);
    }
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
                sums[x] += v1[x]; // * weights[x];
            }
        }

        if (++counter == 700) {
            counter = 0;

            int maxI = 0;
            float maxV = 0;
            for (int x = 1; x < halfLength; ++x) {
                float v = sums[x];

                // Sticky
                //if (x == F0i) v *= 1.5;

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
