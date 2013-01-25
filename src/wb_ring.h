/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <stdint.h>
#include <assert.h>
#pragma once

namespace wb {


/***
 * A ring buffer, overwriting old entries, with an arbitrary power-of-two size.
 * Initial contents are undefined.
 */

template <typename T, unsigned size>
class Ring {
private:
    unsigned last;      //< Index of the last item in the ring
    T buffer[size];

public:
    void push(const T &item)
    {
        assert(last < size);
        assert((size & (size-1)) == 0);

        last++;
        if (last == size)
            last = 0;
        buffer[last] = item;
    }

    /***
     * Index the ring relative to its last item:
     * 0 is the last item, -1 is the next to last, etc.
     */
    T& operator[] (int rel)
    {
        unsigned n = last + rel;
        return buffer[n & (size - 1)];
    }
};


}  // namespace wb
