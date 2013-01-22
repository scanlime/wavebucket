/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include "wb_gldebug.h"
#include "wb_analyzer.h"

namespace wb {

GLDebug::GLDebug(Analyzer *analyzer)
    : analyzer(analyzer)
    {}

int GLDebug::start()
{
    return 0;
}

void GLDebug::stop()
{
}

}  // namespace wb
