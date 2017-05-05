#ifndef GLBATCH_H
#define GLBATCH_H

#include "glbase.h"
#include "glbatchbase.h"

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

class GLBatch : public GLBatchBase,protected GLBase
{

protected:
    //protected virables

    // If Batch has been built
    bool	bBatchDone;

#ifndef PLATFORM_GL_ES
    //VAO location
    QOpenGLVertexArrayObject * VAO;
#endif

    QOpenGLBuffer * vertexArray;

    QOpenGLBuffer * normalArray;

    QOpenGLBuffer ** texcoordArray;

    // What am I drawing....
    GLenum primitiveType;

    // Number of verticies in this batch
    GLuint nNumVerts;

    // Number of texture coordinate sets
    GLuint nNumTextureUnits;

public:
    //public functions

    GLBatch();

    virtual ~GLBatch();

    // Start populating the array
    void Begin(GLenum primitive, GLuint nVerts, GLuint nTextureUnits = 0);

    // Tell the batch you are done
    void End();

    // Reset the Status of this batch so we can change the content of this batch
    void Reset();

    //copy vertex data in block
    void CopyVertexDataBlock3f(GLfloat * vVerts);
    void CopyNormalDataBlock3f( GLfloat * vNorms);
    void CopyTexCoordDataBlock2f(GLfloat * vTexCoords,GLuint uiTextureLayer);

    //the class's own Draw method
    void draw() Q_DECL_OVERRIDE;


    //easy functions to make predefined objects
    void MakeCube(GLfloat fRadius );

private:
    //private functions

    void add2fv(QVector<GLfloat> &vec,QVector2D vec2);
    void add3fv(QVector<GLfloat> &vec,QVector3D vec3);
    void add4fv(QVector<GLfloat> &vec,QVector4D vec4);
    void add2fv(QVector<GLfloat> &vec,float x,float y);
    void add3fv(QVector<GLfloat> &vec,float x,float y,float z);
    void add4fv(QVector<GLfloat> &vec,float x,float y,float z,float w);
};

#endif // GLBATCH_H

