/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <stdint.h>
#include <assert.h>
#pragma once

namespace wb {


/**
 * Coefficients for a biquadratic digital IIR filter.
 *
 * This filter implements the transfer function:
 *
 *   H(z) = (a0 + a1 z^-1 + a2 z^-2) / (1 + b1 z^-1 + b2 z^-2)
 */

struct Biquad {
    float a0, a1;
    float b1, b2;
};


/**
 * Stateful implementation of a biquadratic digital IIR filter.
 */

struct BiquadState {
    float z1, z2;

    void init()
    {
        z1 = z2 = 0;
    }    

    float next(const Biquad &co, float x)
    {
        float o0 = x * co.a0 + z1;
        float o1 = x * co.a1 + z2 - o0 * co.b1;
        float o2 =                - o0 * co.b2;

        z1 = o1;
        z2 = o2;
        return o0;
    } 
};


/**
 * Chain of biquad filters.
 */

template <unsigned N>
struct BiquadChain {
    BiquadState stages[N];

    void init()
    {
        for (unsigned i = 0; i < N; ++i) {
            stages[i].init();
        }
    }            

    float next(const Biquad *co, float x)
    {
        for (unsigned i = 0; i < N; ++i) {
            x = stages[i].next(co[i], x);
        }
        return x;
    }
};


/**
 * The "All Poles" IIR approximation of an ideal Gammatone filter,
 * using four biquad filter stages.
 *
 * From "An Efficient Implementation of the Patterson-Holdsworth
 * Auditory Filter Bank", Malcom Slaney, Apple Computer Technical Report #35.
 */

struct IIRGammatone {
    Biquad stages[4];

    void init(float centerHz, float sampleHz, float bandwidthHz);
};


}  // namespace wb
