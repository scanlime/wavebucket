/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <stdint.h>
#include <assert.h>
#pragma once

namespace wb {


/**
 * A ring buffer, overwriting old entries, with an arbitrary power-of-two size.
 * Initial contents are undefined.
 */

template <typename T, unsigned tSize>
class Ring {
private:
    unsigned last;      //< Index of the last item in the ring
    T buffer[tSize];

public:
    /**
     * Push a new literal value onto the end of the ring, overwriting
     * older data if it exists.
     */
    void push(const T &item)
    {
        assert(last < tSize);
        assert((tSize & (tSize-1)) == 0);

        last++;
        if (last == tSize)
            last = 0;
        buffer[last] = item;
    }

    /**
     * Index the ring relative to its last item:
     * 0 is the last item, -1 is the next to last, etc.
     */
    const T& operator[] (int rel) const
    {
        assert(rel <= 0);
        unsigned n = last + rel;
        return buffer[n & (tSize - 1)];
    }

    /**
     * Linearly interpolate between two indices
     */
    T operator[] (float rel) const
    {
        assert(rel <= 0);
        int ip = rel;
        float fp = rel - ip;

        if (fp < 1e-6) {
            // Edge case, close enough to an integer. Avoids evaluating
            // index [1] when computing [0.0f].
            return (*this)[ip];
        }

        return (*this)[ip] * (1.0f - fp) + (*this)[ip+1] * fp;
    }
};


/**
 * A ring buffer which stores the integral of an incoming signal,
 * making definite integrals across arbitrary ranges a constant-time
 * operation.
 */

template <typename T, unsigned tSize>
class IntegralRing
{
private:
    Ring<T, tSize> ring;
    T c;

public:
    IntegralRing() : c(0) {}

    void push(const T &item)
    {
        c += item;
        ring.push(c);
    }

    T integrate(int from, int to) const
    {
        return ring[to] - ring[from];
    }

    T integrate(float from, float to) const
    {
        return ring[to] - ring[from];
    }
};


}  // namespace wb
