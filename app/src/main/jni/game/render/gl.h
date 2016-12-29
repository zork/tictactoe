////
// gl.h
////

#pragma once

#include "base/platform.h"

#if OS_IOS
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#elif OS_OSX
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#elif OS_CHROME
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glIsVertexArray glIsVertexArrayOES

#elif OS_ANDROID
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#elif OS_WIN
#include <gl/gl.h>
#include "platform/win/bindings/gl_extensions.h"

#else
#error "Please add your platform"
#endif
