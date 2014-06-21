/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <GL/glfw.h>
#include <AntTweakBar.h>
#include <stdint.h>
#pragma once

namespace wb {

class Analyzer;

class GLDebug {
public:
    GLDebug(Analyzer *analyzer);

    int setup();
    void shutdown();
    void draw();
    bool isRunning();

private:
    Analyzer *analyzer;
    TwBar *twBar;
    GLuint spectroTexture;

    static void GLFWCALL onWindowSize(int width, int height);
};

}  // namespace wb
