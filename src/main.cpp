/*
 * Simple driver program; stream audio from stdin, visualize it and play it in real-time.
 *
 * Micah Elizabeth Scott <beth@scanlime.org>
 */

#include <ao/ao.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

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

    ao_device *dev = ao_open_live(ao_default_driver_id(), &fmt, 0);
    if (!dev) {
        perror("ao_open_live");
        return 1;
    }

    char buffer[1024];
    ssize_t numBytes;
    ssize_t count = 0;

    while ((numBytes = read(0, buffer, sizeof buffer)) > 0) {
        ao_play(dev, buffer, numBytes);
        count += numBytes;
        printf("%ld\n", count);
    }

    return 0;
}