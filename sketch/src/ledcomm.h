/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <stdint.h>
#include <time.h>
#pragma once

class LEDComm {
public:
    LEDComm();
    ~LEDComm();

    void setPixel(int led, int color) {
        // GRB order, left-justified. Our display memory is planar.

        int32_t bits = (color & 0xFF0000) | ((color & 0x00FF00) << 16) | ((color & 0x0000FF) << 8);
        uint8_t *ptr = buffer + (led * 24);

        for (unsigned i = 24; i; --i) {
            if (bits < 0)
                *ptr |= 1;
            else
                *ptr &= ~1;
            bits <<= 1;
            ptr++;
        }
    }

    void update();

private:
    int fd;
    bool busy;
    uint8_t buffer[240*6*4 + 1];
};
