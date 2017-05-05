#ifndef PLATFORMDEFINITION_H
#define PLATFORMDEFINITION_H

//NOTE : this file need to be included in  ALL OpenGL releated classes or files !
//All universival header files GL need
#include <QDebug>
#include <QOpenGLContext>


//define platform specific macros

//**********************************************************************************************************

//comment / uncomment this to run desktop / mobile version
//#define PLATFORM_GL_ES

// standard 4:3 = 1.333333f   standard 16 : 9 = 1.777778f
#define DRAW_ASPECT_RATIO 1.777778f

//**********************************************************************************************************

//DO NOT EDIT BELOW

#ifndef PLATFORM_GL_ES
#define PLATFORM_GL_DESKTOP_3_3
#include <QOpenGLFunctions_3_3_Core>
#else
#include <QOpenGLFunctions>
#endif




#endif // PLATFORMDEFINITION_H
