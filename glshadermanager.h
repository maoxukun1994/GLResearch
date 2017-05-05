#ifndef GLSHADERMANAGER_H
#define GLSHADERMANAGER_H

#include "glbase.h"

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

struct EXTERNPROGRAM
{
    QOpenGLShaderProgram * program;
    QString name;
};


class GLShaderManager:protected GLBase
{
protected:
    //protected virables

    QOpenGLShaderProgram * StockShaders[GLBASE_SHADER_LAST];

    QVector <EXTERNPROGRAM>	shaderTable;

public:
    //public functions

    GLShaderManager();

    ~GLShaderManager();

    // Call before using
    bool InitializeStockShaders();

    // Find one of the standard stock shaders and return it's shader handle.
    GLuint GetStockShader(GLBASE_STOCK_SHADER nShaderID);

    // Use a stock shader, and pass in the parameters needed
    QOpenGLShaderProgram * UseStockShader(GLBASE_STOCK_SHADER nShaderID, ...);

    QOpenGLShaderProgram * UseExtShader(int index);

    QOpenGLShaderProgram * UseExtShader(const char * name);

    // Load a shader pair from file, return NULL or shader handle.
    // Vertex program name (minus file extension)
    // is saved in the lookup table
    GLuint LoadShaderPair(const char * shadername,const char *szVertexProgFileName, const char *szFragProgFileName);
    // Load shaders from source text.
    GLuint LoadShaderPairSrc(const char * shadername, const char *szVertexSrc, const char *szFragSrc);

    // Ditto above, but pop in the attributes
    GLuint LoadShaderPairWithAttributes(const char * shadername,const char *szVertexProgFileName, const char *szFragmentProgFileName, ...);
    GLuint LoadShaderPairSrcWithAttributes(const char *shadername, const char *szVertexCode, const char *szFragmentCode, ...);

    // Lookup a previously loaded shader
    GLuint LookupShader(const char *name);

protected:
    //protected functions

    bool gltLoadShaderPairSrc(QOpenGLShaderProgram * p,const char *szVertexSrc, const char *szFragmentSrc);
    bool gltLoadShaderPairSrcWithAttributes(QOpenGLShaderProgram * p,const char *szVertexSrc, const char *szFragmentSrc, ...);
    bool gltLoadShaderPair(QOpenGLShaderProgram * p,const char *szVertexProg, const char *szFragmentProg);
    bool gltLoadShaderPairWithAttributes(QOpenGLShaderProgram * p,const char *szVertexProg, const char *szFragmentProg, ...);

};

#endif // GLSHADERMANAGER_H

