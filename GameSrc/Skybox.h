#ifndef SKYBOX_H
#define SKYBOX_H


#include "glbase.h"
#include "glbatch.h"
#include "glframe.h"

#include <QString>
#include <QImage>
#include <QOpenGLShaderProgram>


class  SkyBox : GLBase
{
private:

    bool loaded;

    QOpenGLShaderProgram * shader;

    GLBatch * batch;

    GLFrame * camera;

    GLuint texID;


public:

    SkyBox() : GLBase()
    {
        this->checkAndInit();

        loaded = false;

        shader = Q_NULLPTR;

        batch = Q_NULLPTR;

        texID = 0;
    }

    ~SkyBox()
    {
        if(this->batch != Q_NULLPTR)
        {
            delete batch;
        }

        gl_ptr->glDeleteTextures(1,&texID);
    }

    void loadDefaultSky(QOpenGLShaderProgram * shad,GLFrame * cam)
    {


        if(loaded)
        {
            qDebug("Sky Texture Already loaded once.Won't load again.");
            return;
        }

        this->shader = shad;

        this->camera = cam;



        gl_ptr->glActiveTexture( GL_TEXTURE0 );
        gl_ptr->glGenTextures(1,&texID);
        gl_ptr->glBindTexture( GL_TEXTURE_CUBE_MAP,texID );
        const char * suffixes[] = { "up", "down", "west","east", "north", "south" };
        GLuint targets[] =
        {
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        };
        for( int i = 0; i < 6; i++ )
        {
            QString fileName = QString( ":/Textures/Nightsky_" ) + suffixes[i] + ".bmp";
            QImage image( fileName );
            gl_ptr->glTexImage2D(targets[i], 0, GL_RGBA,image.width(), image.height(),0, GL_RGBA, GL_UNSIGNED_BYTE, image.mirrored().rgbSwapped().bits());
        }
        // classic cubemap settings
        gl_ptr->glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        gl_ptr->glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        gl_ptr->glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        gl_ptr->glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

#ifndef PLATFORM_GL_ES
        gl_ptr->glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

        // Use the seamless cubemap extensions
        if (  QOpenGLContext::currentContext()->hasExtension( "GL_ARB_seamless_cube_map" ) )
        {
            gl_ptr->glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
        }
        else
        {
            gl_ptr->glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        }
#endif



        //init batch
        batch = new GLBatch();
        batch->MakeCube(4.0f);

        loaded = true;
    }

    void drawSkyBox()
    {
        if(!loaded) return;

        //bind texture
        gl_ptr->glBindTexture(GL_TEXTURE_CUBE_MAP,texID );

        //bind shader and set uniform
        shader->bind();
        shader->setUniformValue("mvpMatrix",camera->getVPRotationMatrix());

        //draw
        batch->draw();

        //release
        shader->release();
    }


};






#endif // SKYBOX_H
