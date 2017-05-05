#ifndef GLBASE_H
#define GLBASE_H

#include "platformdefinition.h"

enum GLBASE_SHADER_ATTRIBUTE
{
    GLBASE_ATTRIBUTE_VERTEX = 0,
    GLBASE_ATTRIBUTE_COLOR,
    GLBASE_ATTRIBUTE_NORMAL,
    GLBASE_ATTRIBUTE_TEXTURE0,
    GLBASE_ATTRIBUTE_TEXTURE1,
    GLBASE_ATTRIBUTE_TEXTURE2,
    GLBASE_ATTRIBUTE_TEXTURE3,
    GLBASE_ATTRIBUTE_LAST
};

enum GLBASE_STOCK_SHADER
{
    GLBASE_SHADER_IDENTITY = 0,
    GLBASE_SHADER_FLAT,
    GLBASE_SHADER_SHADED,
    GLBASE_SHADER_DEFAULT_LIGHT,
    GLBASE_SHADER_POINT_LIGHT_DIFF,
    GLBASE_SHADER_TEXTURE_REPLACE,
    GLBASE_SHADER_TEXTURE_MODULATE,
    GLBASE_SHADER_TEXTURE_POINT_LIGHT_DIFF,
    GLBASE_SHADER_TEXTURE_RECT_REPLACE,
    GLBASE_SHADER_LAST
};


class GLBase
{

protected:

#ifndef PLATFORM_GL_ES
    QOpenGLFunctions_3_3_Core * gl_ptr;
#else
    QOpenGLFunctions * gl_ptr;
#endif
    QOpenGLContext * context_ptr;

public:

    GLBase()
    {
        gl_ptr = Q_NULLPTR;

        context_ptr = Q_NULLPTR;
    }

    virtual ~GLBase()
    {
        //NOTHING needs to be deleted,for all pointer in the instance is not actually newed.
        /*
        //If the context this class instance lives in is no longer valid,then will delete nothing.(to avoid delete null pointers)
        if(!this->isContextValid() || this->context_ptr != QOpenGLContext::currentContext())
        {
            qWarning("While deleting GLBase based instance,current context not match the context used to create this,may cause leak memory !");
            return;
        }

        qDebug("GLBase deconstructed.");

        //no need to delete gl_ptr,because gl_ptr is not a newed object.
        if(gl_ptr != Q_NULLPTR)
        {
            //delete gl_ptr;
        }
        */
    }

    virtual void checkAndInit()
    {
        if(this->isContextValid())
        {
            this->context_ptr = QOpenGLContext::currentContext();
            //qDebug("Context is valid,initializing GLBase based class instance...");

#ifdef PLATFORM_GL_ES
            if(QOpenGLContext::currentContext()->format().majorVersion()<2)
            {
                qFatal("This Program requries OpenGL ES version 2.0 or higher,please update your GLES version. ");
            }
            qDebug("Using GLES 2.0 functions.");
            this->gl_ptr = QOpenGLContext::currentContext()->functions();
#else
            //check for GL version
            if(QOpenGLContext::currentContext()->format().majorVersion()>2 && QOpenGLContext::currentContext()->format().minorVersion()>2)
            {
                this->gl_ptr = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
                if(gl_ptr &&  gl_ptr->initializeOpenGLFunctions() )
                {
                    //qDebug("Using GL 3.3 core functions.");
                }
                else
                {
                    qFatal("GL 3.3 functions initialization failed.Aborting.");
                }
            }
            else
            {
                qFatal("This Program requries OpenGL version 3.3 or higher,please update your OpenGL version.");
            }
#endif
        }
        else
        {
            qFatal("Current Context not valid,aborting.");

            return;
        }
    }

    virtual bool isContextValid()
    {
        if( QOpenGLContext::currentContext() && QOpenGLContext::currentContext()->isValid() )
        {
            return true;
        }
        return false;
    }

};

#endif // GLBASE_H
