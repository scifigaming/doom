// Copyright (c) 2014, Kate Stone
// All rights reserved.
//
// This file is covered by the 3-clause BSD license.
// See the LICENSE file in this program's distribution for details.

#include "global.hpp"

// SDL
#include "SDL.h"

// OpenGL
#ifdef WIN32
#include <Windows.h>
#endif
#include <gl/gl.h>

static SDL_Window *topwindow;
static SDL_GLContext *glcontext;
static int topwinwidth, topwinheight;

bool InitFramework (void)
{
    topwindow = NULL;
    glcontext = NULL;
    
    topwinwidth = topwinheight = 0;
    
    int err = SDL_Init (SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_VIDEO);
    if (err)
        return false;
    
    return true;
}

void QuitFramework (void)
{
    SDL_Quit ();
}

// WINDOW FUNCTIONS

PyObject * PyDoom_GL_CreateWindow (PyObject *self, PyObject *args)
{
    int width, height;
    bool fullscreen;
    
    if (topwindow) // Return False if we already have a window
        Py_RETURN_FALSE;

    int ok = PyArg_ParseTuple(args, "(ii)p", &width, &height, &fullscreen);
    if (!ok)
        return NULL;
    
    int flags = SDL_WINDOW_OPENGL;
    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN;
    
    topwindow = SDL_CreateWindow (
        "PyDoom",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        flags
    );
    
    if (!topwindow) // Could not create a window for some reason
        Py_RETURN_FALSE;
    
    glcontext = new SDL_GLContext;
    *glcontext = SDL_GL_CreateContext (topwindow);
    if (!glcontext)
    {
        SDL_DestroyWindow (topwindow);
        topwindow = NULL;
        Py_RETURN_FALSE;
    }
    
    topwinwidth  = width;
    topwinheight = height;

    Py_RETURN_TRUE;
}

PyObject * PyDoom_GL_DestroyWindow (PyObject *self, PyObject *args)
{
    if (topwindow)
    {
        SDL_DestroyWindow (topwindow);
        topwindow = NULL;
        SDL_GL_DeleteContext (*glcontext);
        delete glcontext;
    }
    
    topwinwidth  = 0;
    topwinheight = 0;
    
    Py_RETURN_NONE;
}

PyObject * PyDoom_GL_HaveWindow (PyObject *self, PyObject *args)
{
    if (!topwindow)
        Py_RETURN_FALSE;
    
    Py_RETURN_TRUE;
}

// DRAWING FUNCTIONS

PyObject * PyDoom_GL_FinishDrawing (PyObject *self, PyObject *args)
{
    if (!topwindow)
        Py_RETURN_NONE;
    
    SDL_GL_SwapWindow (topwindow);
    
    Py_RETURN_NONE;
}

// MODULE DEFINITION

static PyMethodDef PyDoom_GL_Methods[] = {
    // Window handling
    { "CreateWindow",  PyDoom_GL_CreateWindow,  METH_VARARGS,
    "Creates the top-level window. Returns True if a new window was created, False otherwise." },
    { "DestroyWindow", PyDoom_GL_DestroyWindow, METH_NOARGS,
    "Destroys the top-level window if there is one." },
    { "HaveWindow",    PyDoom_GL_HaveWindow,    METH_NOARGS,
    "Returns True if there's a top-level window, False otherwise." },
    
    // OpenGL drawing
    { "FinishDrawing", PyDoom_GL_FinishDrawing, METH_NOARGS,
    "Finishes drawing the OpenGL context and updates the window." },
    
    {NULL, NULL, 0, NULL} // Sentinel
};

static PyModuleDef PyDoom_GL_Module = {
    PyModuleDef_HEAD_INIT,
    "PyDoom_OpenGL",
    "OpenGL rendering context and topmost window control",
    -1,
    PyDoom_GL_Methods
};

PyMODINIT_FUNC PyInit_PyDoom_GL (void)
{
    PyObject *m;
    
    m = PyModule_Create (&PyDoom_GL_Module);
    if (m == NULL)
        return NULL;
    
    return m;
}