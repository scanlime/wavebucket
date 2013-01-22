/*
 * Simple driver program; stream audio from stdin, visualize it and play it in real-time.
 *
 * Micah Elizabeth Scott <beth@scanlime.org>
 */

#include <ao/ao.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "wb_analyzer.h"
#include "wb_gldebug.h"

int main()
{
    ao_initialize();

    char matrix[] = "L,R";
    ao_sample_format fmt = {
        .bits = 16,
        .rate = 44100,
        .channels = 2,
        .byte_format = AO_FMT_LITTLE,
        .matrix = matrix,
    };

    wb::Analyzer analyzer(fmt.rate);
    wb::GLDebug gld(&analyzer);

    ao_device *dev = ao_open_live(ao_default_driver_id(), &fmt, 0);
    if (!dev) {
        perror("ao_open_live");
        return 1;
    }

    if (!gld.start()) {
        perror("Error starting user interface");
        return 1;
    }

    union {
        char bytes[1024];
        int16_t samples[1];
    } buffer;

    ssize_t offset = 0;
    ssize_t numBytes;
    unsigned frameSize = fmt.channels * sizeof(int16_t);

    while ((numBytes = read(0, buffer.bytes + offset, sizeof buffer - offset)) > 0) {

        // Any partial frames that carried over from before.
        numBytes += offset;
        unsigned frames = numBytes / frameSize;
        unsigned truncBytes = frames * frameSize;
        offset = numBytes - truncBytes;

        ao_play(dev, buffer.bytes, truncBytes);
        analyzer.pcmInput(buffer.samples, fmt.channels, frames);

        memmove(buffer.bytes, buffer.bytes + truncBytes, offset);
    }

    gld.stop();
    ao_shutdown();
    return 0;
}
