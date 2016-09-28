//
// Copyright 2011 Tero Saarni
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>

#include "NoobUtils.hpp"

#include "engine_droid.hpp"

#define LOG_TAG "EglSample"

static GLint vertices[][3] = {
    { -0x10000, -0x10000, -0x10000 },
    {  0x10000, -0x10000, -0x10000 },
    {  0x10000,  0x10000, -0x10000 },
    { -0x10000,  0x10000, -0x10000 },
    { -0x10000, -0x10000,  0x10000 },
    {  0x10000, -0x10000,  0x10000 },
    {  0x10000,  0x10000,  0x10000 },
    { -0x10000,  0x10000,  0x10000 }
};

static GLint colors[][4] = {
    { 0x00000, 0x00000, 0x00000, 0x10000 },
    { 0x10000, 0x00000, 0x00000, 0x10000 },
    { 0x10000, 0x10000, 0x00000, 0x10000 },
    { 0x00000, 0x10000, 0x00000, 0x10000 },
    { 0x00000, 0x00000, 0x10000, 0x10000 },
    { 0x10000, 0x00000, 0x10000, 0x10000 },
    { 0x10000, 0x10000, 0x10000, 0x10000 },
    { 0x00000, 0x10000, 0x10000, 0x10000 }
};

GLubyte indices[] = {
    0, 4, 5,    0, 5, 1,
    1, 5, 6,    1, 6, 2,
    2, 6, 7,    2, 7, 3,
    3, 7, 4,    3, 4, 0,
    4, 7, 6,    4, 6, 5,
    3, 0, 1,    3, 1, 2
};


engine_droid::engine_droid()
    : _msg(MSG_NONE), _display(0), _surface(0), _context(0), _angle(0)
{
    noob::logger::log(noob::importance::INFO, "engine_droid instance created");
    pthread_mutex_init(&_mutex, 0);    
    return;
}

engine_droid::~engine_droid()
{
    noob::logger::log(noob::importance::INFO, "engine_droid instance destroyed");
    pthread_mutex_destroy(&_mutex);
    return;
}

void engine_droid::start()
{
    noob::logger::log(noob::importance::INFO, "Creating renderer thread");
    pthread_create(&_threadId, 0, threadStartCallback, this);
    return;
}

void engine_droid::stop()
{
    noob::logger::log(noob::importance::INFO, "Stopping renderer thread");

    // send message to render thread to stop rendering
    pthread_mutex_lock(&_mutex);
    _msg = MSG_RENDER_LOOP_EXIT;
    pthread_mutex_unlock(&_mutex);    

    pthread_join(_threadId, 0);
    noob::logger::log(noob::importance::INFO, "engine_droid thread stopped");

    return;
}

void engine_droid::setWindow(ANativeWindow *window)
{
    // notify render thread that window has changed
    pthread_mutex_lock(&_mutex);
    _msg = MSG_WINDOW_SET;
    _window = window;
    pthread_mutex_unlock(&_mutex);

    return;
}



void engine_droid::renderLoop()
{
    bool renderingEnabled = true;
    
    noob::logger::log(noob::importance::INFO, "renderLoop()");

    while (renderingEnabled) {

        pthread_mutex_lock(&_mutex);

        // process incoming messages
        switch (_msg) {

            case MSG_WINDOW_SET:
                initialize();
                break;

            case MSG_RENDER_LOOP_EXIT:
                renderingEnabled = false;
                destroy();
                break;

            default:
                break;
        }
        _msg = MSG_NONE;
        
        if (_display) {
            drawFrame();
            if (!eglSwapBuffers(_display, _surface)) {
                noob::logger::log(noob::importance::ERROR, noob::concat("eglSwapBuffers() returned error ", noob::to_string(eglGetError())));
            }
        }
        
        pthread_mutex_unlock(&_mutex);
    }
    
    noob::logger::log(noob::importance::INFO, "Render loop exits");
    
    return;
}

bool engine_droid::initialize()
{
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };
    EGLDisplay display;
    EGLConfig config;    
    EGLint numConfigs;
    EGLint format;
    EGLSurface surface;
    EGLContext context;
    EGLint width;
    EGLint height;
    GLfloat ratio;
    
    noob::logger::log(noob::importance::INFO, "Initializing context");
    
    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
    {
        noob::logger::log(noob::importance::ERROR, noob::concat("eglGetDisplay() returned error ", noob::to_string(eglGetError())));
        return false;
    }
    if (!eglInitialize(display, 0, 0))
    {
        noob::logger::log(noob::importance::ERROR, noob::concat("eglInitialize() returned error ", noob::to_string(eglGetError())));
        return false;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs))
    {
        noob::logger::log(noob::importance::ERROR, noob::concat("eglChooseConfig() returned error ", noob::to_string(eglGetError())));
        destroy();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
    {
        noob::logger::log(noob::importance::ERROR, noob::concat("eglGetConfigAttrib() returned error ", noob::to_string(eglGetError())));
        destroy();
        return false;
    }

    ANativeWindow_setBuffersGeometry(_window, 0, 0, format);

    if (!(surface = eglCreateWindowSurface(display, config, _window, 0)))
    {
        noob::logger::log(noob::importance::ERROR, noob::concat("eglCreateWindowSurface() returned error ", noob::to_string(eglGetError())));
        destroy();
        return false;
    }
    
    if (!(context = eglCreateContext(display, config, 0, 0)))
    {
        noob::logger::log(noob::importance::ERROR, noob::concat("eglCreateContext() returned error ", noob::to_string(eglGetError())));
        destroy();
        return false;
    }
    
    if (!eglMakeCurrent(display, surface, surface, context))
    {
        noob::logger::log(noob::importance::ERROR, noob::concat("eglMakeCurrent() returned error ", noob::to_string(eglGetError())));
        destroy();
        return false;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) || !eglQuerySurface(display, surface, EGL_HEIGHT, &height))
    {
        noob::logger::log(noob::importance::ERROR, noob::concat("eglQuerySurface() returned error ", noob::to_string(eglGetError())));
        destroy();
        return false;
    }

    _display = display;
    _surface = surface;
    _context = context;

    glDisable(GL_DITHER);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glClearColor(0, 0, 0, 0);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    
    glViewport(0, 0, width, height);

    ratio = (GLfloat) width / height;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumf(-ratio, ratio, -1, 1, 1, 10);

    return true;
}

void engine_droid::destroy() {
    noob::logger::log(noob::importance::INFO, "Destroying context");

    eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(_display, _context);
    eglDestroySurface(_display, _surface);
    eglTerminate(_display);
    
    _display = EGL_NO_DISPLAY;
    _surface = EGL_NO_SURFACE;
    _context = EGL_NO_CONTEXT;

    return;
}

void engine_droid::drawFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -3.0f);
    glRotatef(_angle, 0, 1, 0);
    glRotatef(_angle*0.25f, 1, 0, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glFrontFace(GL_CW);
    glVertexPointer(3, GL_FIXED, 0, vertices);
    glColorPointer(4, GL_FIXED, 0, colors);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

    _angle += 1.2f;    
}

void* engine_droid::threadStartCallback(void *myself)
{
    engine_droid *renderer = (engine_droid*)myself;

    renderer->renderLoop();
    pthread_exit(0);
    
    return 0;
}

