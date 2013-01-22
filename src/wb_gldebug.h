/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <stdint.h>
#pragma once

namespace wb {

class Analyzer;

class GLDebug {
public:
    GLDebug(Analyzer *analyzer);

    /// Start the GL debug display, running in its own thread.
    int start();

    /// Ask the GL display thread to stop, and wait for it.
    void stop();

private:
    Analyzer *analyzer;
};

}  // namespace wb
