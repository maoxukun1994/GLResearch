#include "glbatch.h"

GLBatch::GLBatch() : GLBase()
{
#ifndef PLATFORM_GL_ES
    VAO = Q_NULLPTR;
#endif

    vertexArray = Q_NULLPTR;

    normalArray = Q_NULLPTR;

    texcoordArray = Q_NULLPTR;

    nNumTextureUnits = 0;
    nNumVerts = 0;
    bBatchDone = false;

    //check the context status
    this->checkAndInit();
}

GLBatch::~GLBatch()
{
    //check context
    if(this->isContextValid())
    {
        if(vertexArray != Q_NULLPTR)
        {
            vertexArray->destroy();
            delete vertexArray;
        }
        if(normalArray != Q_NULLPTR)
        {
            normalArray->destroy();
            delete normalArray;
        }
        if(texcoordArray != Q_NULLPTR)
        {
            for(unsigned int i = 0; i < nNumTextureUnits; i++)
            {

                if(texcoordArray[i] != Q_NULLPTR)
                {
                    texcoordArray[i]->destroy();
                    delete texcoordArray[i];
                }
            }
            delete texcoordArray;
        }
#ifndef PLATFORM_GL_ES
        if(VAO != Q_NULLPTR)
        {
            VAO->destroy();
            VAO->deleteLater();
        }
#endif
    }
}

// Start the primitive batch.
void GLBatch::Begin(GLenum primitive, GLuint nVerts, GLuint nTextureUnits)
{
    // Vertex Array object for this Array if not in OpenGLES
#ifndef PLATFORM_GL_ES
    if(VAO == Q_NULLPTR)
    {
        VAO = new QOpenGLVertexArrayObject();
    }
    else
    {
        VAO->destroy();
    }
    if(!VAO->create())
    {
        qFatal("VAO creation failed.");
    }
    else
    {
        VAO->bind();
    }
#endif

    //vbo -vertex
    if(vertexArray == Q_NULLPTR)
    {
        vertexArray = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        vertexArray->setUsagePattern(QOpenGLBuffer::DynamicDraw);
        if(vertexArray->create() && vertexArray->bind() )
        {
            //qDebug("vertexArray created and binded.");
            vertexArray->allocate(sizeof(GLfloat) * 3 * nVerts);
        }
        else
        {
            qDebug("vertexArray creation or bind failed.");
        }
    }
    else
    {
        if(nVerts > nNumVerts)
        {
            vertexArray->destroy();
            if(vertexArray->create() && vertexArray->bind() )
            {
                //qDebug("vertexArray created and binded.");
                vertexArray->allocate(sizeof(GLfloat) * 3 * nVerts);
            }
            else
            {
                qDebug("vertexArray creation or bind failed.");
            }
        }
    }

    //vbo-normal
    // First time, create the buffer object, allocate the space
    if(normalArray == Q_NULLPTR)
    {
        normalArray = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        normalArray->setUsagePattern(QOpenGLBuffer::DynamicDraw);
        if( normalArray->create() &&  normalArray->bind() )
        {
            //qDebug("normalArray created and binded.");
            normalArray->allocate(sizeof(GLfloat) * 3 * nVerts);
        }
        else
        {
            qDebug("normalArray creation or bind failed.");
        }
    }
    else
    {
        if(nVerts > nNumVerts)
        {
            normalArray->destroy();
            if( normalArray->create() &&  normalArray->bind() )
            {
                //qDebug("normalArray created and binded.");
                normalArray->allocate(sizeof(GLfloat) * 3 * nVerts);
            }
            else
            {
                qDebug("normalArray creation or bind failed.");
            }
        }
    }

    // Limit to four texture units
    if(nTextureUnits > 4)
    {
        nTextureUnits = 4;
    }

    //vbos-textureUV
    if(nTextureUnits > 0)
    {
        if(nTextureUnits <= nNumTextureUnits)
        {
            if(nVerts > nNumVerts)
            {
                for(unsigned int i=0;i<nTextureUnits;i++)
                {
                    texcoordArray[i]->destroy();
                    if(texcoordArray[i]->create() && texcoordArray[i]->bind())
                    {
                        //qDebug("texcoordArray created and binded.");
                        texcoordArray[i]->allocate(sizeof(GLfloat) * 2 * nVerts);
                    }
                    else
                    {
                        qDebug("texcoordArray creation or bind failed.");
                    }
                }
            }
        }
        else
        {
            for(unsigned int i=0;i<nNumTextureUnits;i++)
            {
                texcoordArray[i]->destroy();
                delete texcoordArray[i];
            }
            delete texcoordArray;
            texcoordArray = new QOpenGLBuffer * [nTextureUnits];
            for(unsigned int i=0;i<nTextureUnits;i++)
            {
                texcoordArray[i] = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
                texcoordArray[i]->setUsagePattern(QOpenGLBuffer::DynamicDraw);
                if(texcoordArray[i]->create() && texcoordArray[i]->bind())
                {
                    //qDebug("texcoordArray created and binded.");
                    texcoordArray[i]->allocate(sizeof(GLfloat) * 2 * nVerts);
                }
                else
                {
                    qDebug("texcoordArray creation or bind failed.");
                }
            }
        }
    }

    primitiveType = primitive;

    nNumVerts = nVerts;

    nNumTextureUnits = nTextureUnits;

}

// Bind everything up in a little package
void GLBatch::End(void)
{

#ifndef PLATFORM_GL_ES
    // Set up the vertex array object
    VAO->bind();
#endif

    if(vertexArray != Q_NULLPTR)
    {
        gl_ptr->glEnableVertexAttribArray(GLBASE_ATTRIBUTE_VERTEX);
        gl_ptr->glBindBuffer(GL_ARRAY_BUFFER, vertexArray->bufferId());
        gl_ptr->glVertexAttribPointer(GLBASE_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    if(normalArray != Q_NULLPTR)
    {
        gl_ptr->glEnableVertexAttribArray(GLBASE_ATTRIBUTE_NORMAL);
        gl_ptr->glBindBuffer(GL_ARRAY_BUFFER, normalArray->bufferId());
        gl_ptr->glVertexAttribPointer(GLBASE_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    // How many texture units
    for(unsigned int i = 0; i < nNumTextureUnits; i++)
    {
        if(texcoordArray[i] != Q_NULLPTR)
        {
            gl_ptr->glEnableVertexAttribArray(GLBASE_ATTRIBUTE_TEXTURE0 + i);
            gl_ptr->glBindBuffer(GL_ARRAY_BUFFER, texcoordArray[i]->bufferId());
            gl_ptr->glVertexAttribPointer(GLBASE_ATTRIBUTE_TEXTURE0 + i, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
    }
#ifndef PLATFORM_GL_ES
    VAO->release();
#endif
    bBatchDone = true;
}

// Just start over. No reallocations, etc.
void GLBatch::Reset(void)
{
    bBatchDone = false;
}

// Block Copy in vertex data
void GLBatch::CopyVertexDataBlock3f(GLfloat * vVerts)
{
    // Just bind to existing object
    vertexArray->bind();
    // Copy the data in
    vertexArray->write(0,vVerts, sizeof(GLfloat) * 3 * nNumVerts);
}

// Block copy in normal data
void GLBatch::CopyNormalDataBlock3f(GLfloat * vNorms)
{
    // Just bind to existing object
    normalArray->bind();
    // Copy the data in
    normalArray->write(0,vNorms,sizeof(GLfloat) * 3 * nNumVerts);
}

//Block copy in texture coord data
void GLBatch::CopyTexCoordDataBlock2f(GLfloat * vTexCoords, GLuint uiTextureLayer)
{
    // Just bind to existing object
    texcoordArray[uiTextureLayer]->bind();
    // Copy the data in
    texcoordArray[uiTextureLayer]->write(0,vTexCoords,sizeof(GLfloat) * 2 * nNumVerts);
}

void GLBatch::draw(void)
{
    if(!bBatchDone)
    {
        return;
    }

#ifndef PLATFORM_GL_ES
    // Set up the vertex array object
    VAO->bind();
#else
    if(vertexArray != Q_NULLPTR)
    {
        gl_ptr->glEnableVertexAttribArray(GLBASE_ATTRIBUTE_VERTEX);
        gl_ptr->glBindBuffer(GL_ARRAY_BUFFER, vertexArray->bufferId());
        gl_ptr->glVertexAttribPointer(GLBASE_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    if(normalArray != 0)
    {
        gl_ptr->glEnableVertexAttribArray(GLBASE_ATTRIBUTE_NORMAL);
        gl_ptr->glBindBuffer(GL_ARRAY_BUFFER, normalArray->bufferId());
        gl_ptr->glVertexAttribPointer(GLBASE_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    // How many texture units
    for(unsigned int i = 0; i < nNumTextureUnits; i++)
    {
        if(texcoordArray[i] != Q_NULLPTR)
        {
            gl_ptr->glEnableVertexAttribArray(GLBASE_ATTRIBUTE_TEXTURE0 + i);
            gl_ptr->glBindBuffer(GL_ARRAY_BUFFER, texcoordArray[i]->bufferId());
            gl_ptr->glVertexAttribPointer(GLBASE_ATTRIBUTE_TEXTURE0 + i, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
    }
#endif

    gl_ptr->glDrawArrays(primitiveType, 0, nNumVerts);

#ifndef PLATFORM_GL_ES
    VAO->release();
#else
    gl_ptr->glDisableVertexAttribArray(GLBASE_ATTRIBUTE_VERTEX);
    gl_ptr->glDisableVertexAttribArray(GLBASE_ATTRIBUTE_NORMAL);
    for(unsigned int i = 0; i < nNumTextureUnits; i++)
    {
        if(texcoordArray[i] != Q_NULLPTR)
        {
            gl_ptr->glDisableVertexAttribArray(GLBASE_ATTRIBUTE_TEXTURE0 + i);
        }
    }
#endif
}


//********************************************************************************************************************
//Quick functions to load predefined shapes,such as Spheres,cubes,etc

void GLBatch::MakeCube(GLfloat fRadius )
{
    if(this->nNumVerts != 0)
    {
        qWarning("GLBatch already begined once.The call will be ignored.");
        return;
    }

    QVector<GLfloat> vert;
    QVector<GLfloat> norm;
    QVector<GLfloat> texc;

    //up
    this->add3fv(norm,0.0f, 0.0f, fRadius);
    this->add2fv(texc, 0.0f, 1);
    this->add3fv(vert,-fRadius, fRadius, fRadius);

    this->add3fv(norm,0.0f, 0.0f, fRadius);
    this->add2fv(texc,1, 1);
    this->add3fv(vert,fRadius, fRadius, fRadius);

    this->add3fv(norm,0.0f, 0.0f, fRadius);
    this->add2fv(texc,1, 0.0f);
    this->add3fv(vert,fRadius, -fRadius, fRadius);

    this->add3fv(norm,0.0f, 0.0f, fRadius);
    this->add2fv(texc,1, 0.0f);
    this->add3fv(vert,fRadius, -fRadius, fRadius);

    this->add3fv(norm,0.0f, 0.0f, fRadius);
    this->add2fv(texc, 0.0f, 0.0f);
    this->add3fv(vert,-fRadius, -fRadius, fRadius);

    this->add3fv(norm,0.0f, 0.0f, fRadius);
    this->add2fv(texc, 0.0f, 1);
    this->add3fv(vert,-fRadius, fRadius, fRadius);



    // DOWN

    this->add3fv(norm,0.0f, 0.0f, -fRadius);
    this->add2fv(texc, 0.0f, 1);
    this->add3fv(vert,-fRadius, fRadius, -fRadius);

    this->add3fv(norm,0.0f, 0.0f, -fRadius);
    this->add2fv(texc, 0.0f, 0.0f);
    this->add3fv(vert,-fRadius, -fRadius, -fRadius);

    this->add3fv(norm,0.0f, 0.0f, -fRadius);
    this->add2fv(texc,1, 0.0f);
    this->add3fv(vert,fRadius, -fRadius, -fRadius);


    this->add3fv(norm,0.0f, 0.0f, -fRadius);
    this->add2fv(texc,1, 0.0f);
    this->add3fv(vert,fRadius, -fRadius, -fRadius);

    this->add3fv(norm,0.0f, 0.0f, -fRadius);
    this->add2fv(texc,1, 1);
    this->add3fv(vert,fRadius, fRadius, -fRadius);

    this->add3fv(norm,0.0f, 0.0f, -fRadius);
    this->add2fv(texc, 0.0f, 1);
    this->add3fv(vert,-fRadius, fRadius, -fRadius);



    //LEFT AND RIGHT
    // LEFT
    this->add3fv(norm,-fRadius, 0.0f, 0.0f);
    this->add2fv(texc, 0.0f, 0.0f);
    this->add3fv(vert,-fRadius, -fRadius, -fRadius);

    this->add3fv(norm,-fRadius, 0.0f, 0.0f);
    this->add2fv(texc,1, 0.0f);
    this->add3fv(vert,-fRadius, fRadius, -fRadius);

    this->add3fv(norm,-fRadius, 0.0f, 0.0f);
    this->add2fv(texc,1, 1);
    this->add3fv(vert,-fRadius, fRadius, fRadius);

    this->add3fv(norm,-fRadius, 0.0f, 0.0f);
    this->add2fv(texc, 0.0f, 1);
    this->add3fv(vert,-fRadius, -fRadius, fRadius);

    this->add3fv(norm,-fRadius, 0.0f, 0.0f);
    this->add2fv(texc, 0.0f, 0.0f);
    this->add3fv(vert,-fRadius, -fRadius, -fRadius);

    this->add3fv(norm,-fRadius, 0.0f, 0.0f);
    this->add2fv(texc,1, 1);
    this->add3fv(vert,-fRadius, fRadius, fRadius);



    // RIGHT

    this->add3fv(norm,fRadius, 0.0f, 0.0f);
    this->add2fv(texc,1, 1);
    this->add3fv(vert,fRadius, fRadius, fRadius);

    this->add3fv(norm,fRadius, 0.0f, 0.0f);
    this->add2fv(texc,1, 0.0f);
    this->add3fv(vert,fRadius, fRadius, -fRadius);

    this->add3fv(norm,fRadius, 0.0f, 0.0f);
    this->add2fv(texc, 0.0f, 0.0f);
    this->add3fv(vert,fRadius, -fRadius, -fRadius);



    this->add3fv(norm,fRadius, 0.0f, 0.0f);
    this->add2fv(texc, 0.0f, 0.0f);
    this->add3fv(vert,fRadius, -fRadius, -fRadius);


    this->add3fv(norm,fRadius, 0.0f, 0.0f);
    this->add2fv(texc, 0.0f, 1);
    this->add3fv(vert,fRadius, -fRadius, fRadius);


    this->add3fv(norm,fRadius, 0.0f, 0.0f);
    this->add2fv(texc,1,1);
    this->add3fv(vert,fRadius, fRadius, fRadius);




    //FRONT AND BACK
    // FRONT
    this->add3fv(norm,0.0f, fRadius, 0.0f);
    this->add2fv(texc, 0.0f, 0.0f);
    this->add3fv(vert,-fRadius, fRadius, -fRadius);

    this->add3fv(norm,0.0f, fRadius, 0.0f);
    this->add2fv(texc,1, 0.0f);
    this->add3fv(vert,fRadius, fRadius, -fRadius);

    this->add3fv(norm,0.0f, fRadius, 0.0f);
    this->add2fv(texc,1, 1);
    this->add3fv(vert,fRadius, fRadius, fRadius);


    this->add3fv(norm,0.0f, fRadius, 0.0f);
    this->add2fv(texc, 0.0f, 1);
    this->add3fv(vert,-fRadius, fRadius, fRadius);

    this->add3fv(norm,0.0f, fRadius, 0.0f);
    this->add2fv(texc, 0.0f, 0.0f);
    this->add3fv(vert,-fRadius, fRadius, -fRadius);

    this->add3fv(norm,0.0f, fRadius, 0.0f);
    this->add2fv(texc,1, 1);
    this->add3fv(vert,fRadius, fRadius, fRadius);


    // BACK

    this->add3fv(norm,0.0f, -fRadius, 0.0f);
    this->add2fv(texc,1, 1);
    this->add3fv(vert,fRadius, -fRadius, fRadius);

    this->add3fv(norm,0.0f, -fRadius, 0.0f);
    this->add2fv(texc,1, 0.0f);
    this->add3fv(vert,fRadius, -fRadius, -fRadius);

    this->add3fv(norm,0.0f, -fRadius, 0.0f);
    this->add2fv(texc, 0.0f, 0.0f);
    this->add3fv(vert,-fRadius, -fRadius, -fRadius);


    this->add3fv(norm,0.0f, -fRadius, 0.0f);
    this->add2fv(texc,1, 1);
    this->add3fv(vert,fRadius, -fRadius, fRadius);

    this->add3fv(norm,0.0f, -fRadius, 0.0f);
    this->add2fv(texc, 0.0f, 0.0f);
    this->add3fv(vert,-fRadius, -fRadius, -fRadius);

    this->add3fv(norm,0.0f, -fRadius, 0.0f);
    this->add2fv(texc, 0.0f, 1);
    this->add3fv(vert,-fRadius, -fRadius, fRadius);

    this->Begin(GL_TRIANGLES, 36, 1);
    this->CopyVertexDataBlock3f(vert.data());
    this->CopyNormalDataBlock3f(norm.data());
    this->CopyTexCoordDataBlock2f(texc.data(),0);
    this->End();
}

void GLBatch::add2fv(QVector<GLfloat> &vec, QVector2D vec2)
{
    GLfloat x,y;
    x = vec2.x();
    y = vec2.y();
    vec.push_back(x);
    vec.push_back(y);
    return;
}

void GLBatch::add3fv(QVector<GLfloat> &vec,QVector3D vec3)
{
    GLfloat x,y,z;
    x = vec3.x();
    y = vec3.y();
    z = vec3.z();
    vec.push_back(x);
    vec.push_back(y);
    vec.push_back(z);
    return;
}

void GLBatch::add4fv(QVector<GLfloat> &vec,QVector4D vec4)
{
    GLfloat x,y,z,w;
    x = vec4.x();
    y = vec4.y();
    z = vec4.z();
    w = vec4.w();
    vec.push_back(x);
    vec.push_back(y);
    vec.push_back(z);
    vec.push_back(w);
    return;
}

void GLBatch::add2fv(QVector<GLfloat> &vec, float x, float y)
{
    vec.push_back(x);
    vec.push_back(y);
    return;
}

void GLBatch::add3fv(QVector<GLfloat> &vec,float x,float y,float z)
{
    vec.push_back(x);
    vec.push_back(y);
    vec.push_back(z);
    return;
}

void GLBatch::add4fv(QVector<GLfloat> &vec,float x,float y,float z,float w)
{
    vec.push_back(x);
    vec.push_back(y);
    vec.push_back(z);
    vec.push_back(w);
    return;
}
