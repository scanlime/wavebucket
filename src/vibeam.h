/*
 * Very simple serial Vibeam remote client.
 */

#include <stdint.h>
#include <time.h>
#pragma once

class Vibeam {
public:
    Vibeam();
    ~Vibeam();
    void txPower(int value);

private:
    int fd;
    int tokens;
    time_t bootTime;
};
