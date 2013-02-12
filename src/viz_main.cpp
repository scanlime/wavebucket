/*
 * Simple driver program; stream audio from stdin, visualize
 * it and play it in real-time. Uses the wb::Analyzer and wb::GLDebug
 * objects. Runs GLDebug on the main thread, and audio processing
 * on an auxiliary thread.
 *
 * Micah Elizabeth Scott <beth@scanlime.org>
 */

#include <ao/ao.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "tinythread.h"
#include "wb_analyzer.h"
#include "wb_gldebug.h"

struct Viz
{
    static ao_sample_format fmt;
    static wb::Analyzer analyzer;
    static wb::GLDebug debug;
    static ao_device *dev;
    static bool running;

    static void debugThread(void *arg = 0)
    {
        if (debug.setup()) {
            exit(1);
        }

        while (running) {
            if (!debug.isRunning()) {
                running = false;
                break;
            }
            debug.draw();
        }

        debug.shutdown();
    }

    static void audioInit()
    {
        // Must run on the main thread
        ao_initialize();
        dev = ao_open_live(ao_default_driver_id(), &fmt, 0);
        if (!dev) {
            perror("ao_open_live");
            exit(1);
        }
    }

    static void audioShutdown()
    {
        ao_close(dev);
        ao_shutdown();
    }

    static void audioThread(void *arg = 0)
    {
        union {
            char bytes[1024];
            int16_t samples[1];
        } buffer;

        ssize_t offset = 0;
        unsigned frameSize = fmt.channels * sizeof(int16_t);

        while (running) {
            ssize_t numBytes = read(0, buffer.bytes + offset, sizeof buffer - offset);

            if (numBytes <= 0) {
                running = false;
                break;
            }

            // Any partial frames that carried over from before.
            numBytes += offset;
            unsigned frames = numBytes / frameSize;
            unsigned truncBytes = frames * frameSize;
            offset = numBytes - truncBytes;

            analyzer.pcmInput(buffer.samples, fmt.channels, frames);
            analyzer.pcmSynth(buffer.samples, fmt.channels, frames);
            ao_play(dev, buffer.bytes, truncBytes);

            memmove(buffer.bytes, buffer.bytes + truncBytes, offset);
        }
    }
};

ao_sample_format Viz::fmt =
{
    .bits = 16,
    .rate = 44100,
    .channels = 2,
    .byte_format = AO_FMT_LITTLE,
    .matrix = (char*) "L,R",
};

wb::Analyzer Viz::analyzer(Viz::fmt.rate);
wb::GLDebug Viz::debug(&Viz::analyzer);
ao_device *Viz::dev = 0;
bool Viz::running = true;

int main()
{
    Viz::audioInit();
    tthread::thread audioThread(Viz::audioThread, 0);
    Viz::debugThread();
    audioThread.join();
    Viz::audioShutdown();
    return 0;
}
