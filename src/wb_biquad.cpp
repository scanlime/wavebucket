/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <math.h>
#include "wb_biquad.h"

namespace wb {


void IIRGammatone::init(float centerHz, float sampleHz)
{
    /*
     * From "An Efficient Implementation of the Patterson-Holdsworth
     * Auditory Filter Bank", Malcom Slaney, Apple Computer Technical Report #35.
     *
     * https://engineering.purdue.edu/~malcolm/apple/tr35/PattersonsEar.pdf
     */

    float t = 1.0 / sampleHz;
    float b = 158.14337595199515 * (1.0 + 0.00437 * centerHz);

    float s = -2.0 * sin(2.0 * centerHz * M_PI * t) / exp(b * t);
    float c = -2.0 * cos(2.0 * centerHz * M_PI * t) / exp(b * t);

    stages[3].a0 = stages[2].a0 = stages[1].a0 = stages[0].a0 = t;
    stages[3].b1 = stages[2].b1 = stages[1].b1 = stages[0].b1 = c;
    stages[3].b2 = stages[2].b2 = stages[1].b2 = stages[0].b2 = exp(-2.0 * b * t);

    stages[0].a1 = 0.5 * t*c + 1.2071067811865475 * t*s;
    stages[1].a1 = 0.5 * t*c - 1.2071067811865475 * t*s;
    stages[2].a1 = 0.5 * t*c + 0.2071067811865474 * t*s;
    stages[3].a1 = 0.5 * t*c - 0.2071067811865474 * t*s;
}


}  // namespace wb
