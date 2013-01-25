/*
 * Part of the WaveBucket experimental music visualization library.
 * Copyright (C) 2013 Micah Elizabeth Scott. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <AntTweakBar.h>

#include "wb_gldebug.h"
#include "wb_analyzer.h"

namespace wb {

GLDebug::GLDebug(Analyzer *analyzer)
    : analyzer(analyzer)
    {}

int GLDebug::setup()
{
    glfwInit();

    if (!glfwOpenWindow(600, 600,
            8,8,8,0,24,0, GLFW_WINDOW)) {
        perror("glfwOpenWindow");
        return 1;
    }

    glfwEnable(GLFW_MOUSE_CURSOR);
    glfwEnable(GLFW_KEY_REPEAT);
    glfwSetWindowTitle("Viz Debug UI");

    TwInit(TW_OPENGL, NULL);

    glfwSetWindowSizeCallback(onWindowSize);
    glfwSetMouseButtonCallback((GLFWmousebuttonfun)TwEventMouseButtonGLFW);
    glfwSetMousePosCallback((GLFWmouseposfun)TwEventMousePosGLFW);
    glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
    glfwSetKeyCallback((GLFWkeyfun)TwEventKeyGLFW);
    glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);

    glGenTextures(1, &spectroTexture);
    glBindTexture(GL_TEXTURE_2D, spectroTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return 0;
}

void GLDebug::shutdown()
{
    TwTerminate();
    glfwTerminate();
}

bool GLDebug::isRunning()
{
    return !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
}

void GLDebug::onWindowSize(int width, int height)
{
    glViewport(0, 0, width, height);
    TwWindowSize(width, height);
}

void GLDebug::draw()
{
    static const GLfloat s = 0.90;
    static const GLfloat va[] = {
        0, 0,  -s, -s, 0,
        1, 0,   s, -s, 0,
        1, 1,   s,  s, 0,
        0, 1,  -s,  s, 0,
    };

    glBindTexture(GL_TEXTURE_2D, spectroTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
        analyzer->xxxDebugWidth, analyzer->xxxDebugHeight, 0,
        GL_LUMINANCE, GL_UNSIGNED_BYTE, analyzer->xxxDebugBuffer);
    glEnable(GL_TEXTURE_2D);

    glClearColor(0, 0, 0.3, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glInterleavedArrays(GL_T2F_V3F, 0, va);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    TwDraw();
    glfwSwapBuffers();
}

}  // namespace wb
