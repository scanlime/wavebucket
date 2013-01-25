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

    if (!glfwOpenWindow(400, 800,
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
    static const GLfloat va[] = {
        0, 0,  -0.9, -0.9, 0,
        1, 0,   0.9, -0.9, 0,
        1, 1,   0.9,  0.9, 0,
        0, 1,  -0.9,  0.9, 0,
    };

    glColor4f(1, 1, 1, 1);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, spectroTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 256, 512, 0,
        GL_LUMINANCE, GL_UNSIGNED_BYTE, analyzer->xxxDebugBuffer);

    glClearColor(0, 0, 0.25, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glInterleavedArrays(GL_T2F_V3F, 0, va);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    TwDraw();
    glfwSwapBuffers();
}

}  // namespace wb
