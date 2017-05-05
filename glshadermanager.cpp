#include "glshadermanager.h"
#include "shadersourcefile.h"

// Constructor, just zero out everything
GLShaderManager::GLShaderManager():GLBase()
{
    // Set stock shader handles to 0... uninitialized
    for(unsigned int i = 0; i < GLBASE_SHADER_LAST; i++)
    {
        StockShaders[i] = Q_NULLPTR;
    }
    //clear shader table
    shaderTable.clear();

    this->checkAndInit();
}

// Destructor, turn loose of everything
GLShaderManager::~GLShaderManager()
{
    // Stock shaders is the general (are you initialized test)
    if(StockShaders[0] != Q_NULLPTR)
    {
        for(int i = 0; i < GLBASE_SHADER_LAST; i++)
        {
            StockShaders[i]->release();
            StockShaders[i]->removeAllShaders();
            StockShaders[i]->deleteLater();
        }
    }
    // Free shader table too
    for(int i = 0; i < shaderTable.count(); i++)
    {
        shaderTable[i].program->release();
        shaderTable[i].program->removeAllShaders();
        shaderTable[i].program->deleteLater();
    }
}

bool GLShaderManager::gltLoadShaderPairSrc(QOpenGLShaderProgram * p,const char *szVertexSrc, const char *szFragmentSrc)
{
    if(p->addShaderFromSourceCode(QOpenGLShader::Vertex,szVertexSrc) && p->addShaderFromSourceCode(QOpenGLShader::Fragment,szFragmentSrc))
    {
    }
    else
    {
        qFatal("Add shader failed.");
    }
    return p->link();
}

bool GLShaderManager::gltLoadShaderPairSrcWithAttributes(QOpenGLShaderProgram * p,const char *szVertexSrc, const char *szFragmentSrc, ...)
{
    p->addShaderFromSourceCode(QOpenGLShader::Vertex,szVertexSrc);
    p->addShaderFromSourceCode(QOpenGLShader::Fragment,szFragmentSrc);

    // List of attributes
    va_list attributeList;
    va_start(attributeList, szFragmentSrc);
    char *szNextArg;
    // Number of attributes
    int iArgCount = va_arg(attributeList, int);
    for(int i = 0; i < iArgCount; i++)
    {
        int index = va_arg(attributeList, int);
        szNextArg = va_arg(attributeList, char*);
        p->bindAttributeLocation(szNextArg,index);
    }
    va_end(attributeList);

    return p->link();
}

bool GLShaderManager::gltLoadShaderPair(QOpenGLShaderProgram * p, const char *szVertexCode, const char *szFragmentCode)
{
    p->addShaderFromSourceFile(QOpenGLShader::Vertex,szVertexCode);
    p->addShaderFromSourceFile(QOpenGLShader::Fragment,szFragmentCode);
    return p->link();
}

bool GLShaderManager::gltLoadShaderPairWithAttributes(QOpenGLShaderProgram * p,const char *szVertexCode, const char *szFragmentCode, ...)
{
    p->addShaderFromSourceFile(QOpenGLShader::Vertex,szVertexCode);
    p->addShaderFromSourceFile(QOpenGLShader::Fragment,szFragmentCode);

    // List of attributes
    va_list attributeList;
    va_start(attributeList, szFragmentCode);
    char *szNextArg;
    // Number of attributes
    int iArgCount = va_arg(attributeList, int);
    for(int i = 0; i < iArgCount; i++)
    {
        int index = va_arg(attributeList, int);
        szNextArg = va_arg(attributeList, char*);
        p->bindAttributeLocation(szNextArg,index);
    }
    va_end(attributeList);

    return p->link();
}

// Initialize and load the stock shaders
bool GLShaderManager::InitializeStockShaders()
{

    //create shaderprogram on client side
    for(int i=0;i<GLBASE_SHADER_LAST;i++)
    {
        StockShaders[i] = new QOpenGLShaderProgram();
    }
    if(this->gltLoadShaderPairSrcWithAttributes(StockShaders[GLBASE_SHADER_IDENTITY],szIdentityShaderVP,szIdentityShaderFP,1,GLBASE_ATTRIBUTE_VERTEX,"vVertex"))
    {
        qDebug("Identity Shader Loaded.");
    }
    else
    {
        qWarning("Identity Shader load failed.");
        return false;
    }

    if(this->gltLoadShaderPairSrcWithAttributes(StockShaders[GLBASE_SHADER_FLAT],szFlatShaderVP,szFlatShaderFP,1,GLBASE_ATTRIBUTE_VERTEX,"vVertex"))
    {
        qDebug("Flat Shader Loaded.");
    }
    else
    {
        qWarning("Flat Shader load failed.");
        return false;
    }

    if(this->gltLoadShaderPairSrcWithAttributes(StockShaders[GLBASE_SHADER_SHADED],szShadedVP,szShadedFP,2,GLBASE_ATTRIBUTE_VERTEX,"vVertex",GLBASE_ATTRIBUTE_COLOR,"vColor"))
    {
        qDebug("Shaded Shader Loaded.");
    }
    else
    {
        qWarning("Shaded Shader load failed.");
        return false;
    }

    if(this->gltLoadShaderPairSrcWithAttributes(StockShaders[GLBASE_SHADER_DEFAULT_LIGHT],szDefaultLightVP,szDefaultLightFP,2,GLBASE_ATTRIBUTE_VERTEX,"vVertex",GLBASE_ATTRIBUTE_NORMAL,"vNormal"))
    {
        qDebug("Default light Shader Loaded.");
    }
    else
    {
        qWarning("Default light shader load failed.");
        return false;
    }

    if(this->gltLoadShaderPairSrcWithAttributes(StockShaders[GLBASE_SHADER_POINT_LIGHT_DIFF],szPointLightDiffVP, szPointLightDiffFP, 2,GLBASE_ATTRIBUTE_VERTEX, "vVertex", GLBASE_ATTRIBUTE_NORMAL, "vNormal"))
    {
        qDebug("Point Light Diff Shader Loaded.");
    }
    else
    {
        qWarning("Point Light Diff Shader load failed.");
        return false;
    }

    if(this->gltLoadShaderPairSrcWithAttributes(StockShaders[GLBASE_SHADER_TEXTURE_REPLACE],szTextureReplaceVP, szTextureReplaceFP, 2, GLBASE_ATTRIBUTE_VERTEX, "vVertex", GLBASE_ATTRIBUTE_TEXTURE0, "vTexCoord0"))
    {
        qDebug("Texture Replace Shader Loaded.");
    }
    else
    {
        qWarning("Texture Replace Shader load failed.");
        return false;
    }

    if(this->gltLoadShaderPairSrcWithAttributes(StockShaders[GLBASE_SHADER_TEXTURE_MODULATE],szTextureModulateVP, szTextureModulateFP, 2,GLBASE_ATTRIBUTE_VERTEX, "vVertex", GLBASE_ATTRIBUTE_TEXTURE0, "vTexCoord0"))
    {
        qDebug("Texture modulate Shader Loaded.");
    }
    else
    {
        qWarning("Texture modulate Shader load failed.");
        return false;
    }

    if(this->gltLoadShaderPairSrcWithAttributes(StockShaders[GLBASE_SHADER_TEXTURE_POINT_LIGHT_DIFF],szTexturePointLightDiffVP, szTexturePointLightDiffFP, 3,GLBASE_ATTRIBUTE_VERTEX, "vVertex", GLBASE_ATTRIBUTE_NORMAL, "vNormal", GLBASE_ATTRIBUTE_TEXTURE0, "vTexCoord0"))
    {
        qDebug("Texture Pointlight diff Shader Loaded.");
    }
    else
    {
        qWarning("Texture Pointlight diff Shader load failed.");
        return false;
    }

    if(this->gltLoadShaderPairSrcWithAttributes(StockShaders[GLBASE_SHADER_TEXTURE_RECT_REPLACE],szTextureRectReplaceVP, szTextureRectReplaceFP, 2, GLBASE_ATTRIBUTE_VERTEX, "vVertex", GLBASE_ATTRIBUTE_TEXTURE0, "vTexCoord0"))
    {
        qDebug("Texture rect replace Shader Loaded.");
    }
    else
    {
        qWarning("Texture rect replace Shader load failed.");
        return false;
    }

    return true;
}

// Use a specific stock shader, and set the appropriate uniforms
QOpenGLShaderProgram * GLShaderManager::UseStockShader(GLBASE_STOCK_SHADER nShaderID, ...)
{
    // Check for out of bounds
    if(nShaderID >= GLBASE_SHADER_LAST)
    {
        return Q_NULLPTR;
    }

    // List of uniforms
    va_list uniformList;
    va_start(uniformList, nShaderID);

    // Bind to the correct shader
    StockShaders[nShaderID]->bind();

    // Set up the uniforms
    int iInteger;
    QMatrix4x4 mvpMatrix;
    QMatrix4x4 pMatrix;
    QMatrix4x4 mvMatrix;
    QVector4D  vColor;
    QVector3D  vLightPos;

    switch(nShaderID)
    {
    case GLBASE_SHADER_FLAT:
        // Just the modelview projection matrix and the color
        mvpMatrix = va_arg(uniformList, QMatrix4x4);
        StockShaders[nShaderID]->setUniformValue("mvpMatrix",mvpMatrix);
        vColor = va_arg(uniformList, QVector4D);
        StockShaders[nShaderID]->setUniformValue("vColor",vColor);
        break;

    case GLBASE_SHADER_TEXTURE_RECT_REPLACE:
    case GLBASE_SHADER_TEXTURE_REPLACE:
        // Just the texture place
        mvpMatrix = va_arg(uniformList, QMatrix4x4);
        StockShaders[nShaderID]->setUniformValue("mvpMatrix",mvpMatrix);
        iInteger = va_arg(uniformList, int);
        StockShaders[nShaderID]->setUniformValue("textureUnit0",iInteger);
        break;

    case GLBASE_SHADER_TEXTURE_MODULATE: // Multiply the texture by the geometry color
        mvpMatrix = va_arg(uniformList, QMatrix4x4);
        StockShaders[nShaderID]->setUniformValue("mvpMatrix",mvpMatrix);
        vColor = va_arg(uniformList, QVector4D);
        StockShaders[nShaderID]->setUniformValue("vColor",vColor);
        iInteger = va_arg(uniformList, int);
        StockShaders[nShaderID]->setUniformValue("textureUnit0",iInteger);
        break;


    case GLBASE_SHADER_DEFAULT_LIGHT:
        mvMatrix = va_arg(uniformList, QMatrix4x4);
        StockShaders[nShaderID]->setUniformValue("mvpMatrix",mvpMatrix);
        pMatrix = va_arg(uniformList, QMatrix4x4);
        StockShaders[nShaderID]->setUniformValue("pMatrix",pMatrix);
        vColor = va_arg(uniformList, QVector4D);
        StockShaders[nShaderID]->setUniformValue("vColor",vColor);
        break;

    case GLBASE_SHADER_POINT_LIGHT_DIFF:
        mvMatrix = va_arg(uniformList, QMatrix4x4);
        StockShaders[nShaderID]->setUniformValue("mvMatrix",mvMatrix);
        pMatrix = va_arg(uniformList, QMatrix4x4);
        StockShaders[nShaderID]->setUniformValue("pMatrix",pMatrix);
        vLightPos = va_arg(uniformList, QVector3D);
        StockShaders[nShaderID]->setUniformValue("vLightPos",vLightPos);
        vColor = va_arg(uniformList, QVector4D);
        StockShaders[nShaderID]->setUniformValue("vColor",vColor);
        break;

    case GLBASE_SHADER_TEXTURE_POINT_LIGHT_DIFF:
        mvMatrix = va_arg(uniformList, QMatrix4x4);
        StockShaders[nShaderID]->setUniformValue("mvMatrix",mvMatrix);
        pMatrix = va_arg(uniformList, QMatrix4x4);
        StockShaders[nShaderID]->setUniformValue("pMatrix",pMatrix);
        vLightPos = va_arg(uniformList, QVector3D);
        StockShaders[nShaderID]->setUniformValue("vLightPos",vLightPos);
        vColor = va_arg(uniformList, QVector4D);
        StockShaders[nShaderID]->setUniformValue("vColor",vColor);
        iInteger = va_arg(uniformList, int);
        StockShaders[nShaderID]->setUniformValue("textureUnit0",iInteger);
        break;


    case GLBASE_SHADER_SHADED:
        // Just the modelview projection matrix. Color is an attribute
        mvpMatrix = va_arg(uniformList, QMatrix4x4);
        StockShaders[nShaderID]->setUniformValue("mvpMatrix",mvpMatrix);
        break;

    case GLBASE_SHADER_IDENTITY:	// Just the Color
        vColor = va_arg(uniformList, QVector4D);
        StockShaders[nShaderID]->setUniformValue("vColor",vColor);
    default:
        break;
    }
    va_end(uniformList);

    return StockShaders[nShaderID];
}

QOpenGLShaderProgram *GLShaderManager::UseExtShader(int index)
{
    if(index < shaderTable.count())
    {
        if(shaderTable[index].program->isLinked())
        {
            shaderTable[index].program->bind();
            return shaderTable[index].program;
        }
        else
        {
            qFatal("Ext shader not valid.");
        }
    }
    else
    {
        qWarning("Index out of bound.");
        return Q_NULLPTR;
    }
}

QOpenGLShaderProgram *GLShaderManager::UseExtShader(const char * name)
{
    QString sname(name);
    for(int i=0;i<shaderTable.count();i++)
    {
        if(shaderTable[i].name == sname)
        {
            shaderTable[i].program->bind();
            return shaderTable[i].program;
        }
    }
    qDebug("No such name extshader found.");
    return Q_NULLPTR;
}

// Lookup a stock shader
GLuint GLShaderManager::GetStockShader(GLBASE_STOCK_SHADER nShaderID)
{
    if(nShaderID >= GLBASE_SHADER_LAST)
    {
        return 0;
    }

    return StockShaders[nShaderID]->programId();
}

// Lookup a previously loaded shader. If szFragProg == NULL, it is assumed to be the same name as szVertexCode
GLuint GLShaderManager::LookupShader(const char * name)
{
    QString sname(name);
    // Linear Search... this isn't supposed to be relied on all the time
    for(int i = 0; i < shaderTable.count(); i++)
    {
        if(sname == shaderTable[i].name)
        {
            return shaderTable[i].program->programId();
        }
    }
    //Failed
    return 0;
}

// Load a shader pair from file. The shader pair root is added to the shader
// lookup table and can be found again if necessary with LookupShader.
GLuint GLShaderManager::LoadShaderPair(const char * shadername,const char *szVertexCodeFileName, const char *szFragProgFileName)
{
    EXTERNPROGRAM extshader;

    // Make sure it's not already loaded
    GLuint uiReturn = LookupShader(shadername);
    if(uiReturn != 0)
    {
        return uiReturn;
    }

    // Load shader and test for fail

    extshader.program = new QOpenGLShaderProgram();

    if(!this->gltLoadShaderPair(extshader.program,szVertexCodeFileName,szFragProgFileName))
    {
        return 0;
    }

    // Add to the table

    extshader.name = QString(shadername);
    shaderTable.push_back(extshader);

    return extshader.program->programId();
}

// Load shaders from source text. If the szName is NULL, just make it and return the handle
// (useful for stock shaders). Otherwize, make sure it's not already there, then add to list
GLuint GLShaderManager::LoadShaderPairSrc(const char * shadername, const char *szVertexSrc, const char *szFragSrc)
{
    EXTERNPROGRAM extshader;

    // Make sure it's not already loaded
    GLuint uiReturn = LookupShader(shadername);
    if(uiReturn != 0)
    {
        return uiReturn;
    }

    // Load shader and test for fail

    extshader.program = new QOpenGLShaderProgram();

    if(!this->gltLoadShaderPairSrc(extshader.program,szVertexSrc,szFragSrc))
    {
        return 0;
    }

    // Add to the table

    extshader.name = QString(shadername);
    shaderTable.push_back(extshader);

    return extshader.program->programId();
}

// Load the shader file, with the supplied named attributes
GLuint GLShaderManager::LoadShaderPairWithAttributes(const char * shadername,const char *szVertexCodeFileName, const char *szFragmentCodeFileName, ...)
{
    EXTERNPROGRAM extshader;

    // Check for duplicate
    GLuint uiShader = LookupShader(shadername);
    if(uiShader != 0)
    {
        return uiShader;
    }

    extshader.program = new QOpenGLShaderProgram();


    extshader.program->addShaderFromSourceFile(QOpenGLShader::Vertex,szVertexCodeFileName) ;
    extshader.program->addShaderFromSourceFile(QOpenGLShader::Fragment,szFragmentCodeFileName);

    // List of attributes
    va_list attributeList;
    va_start(attributeList, szFragmentCodeFileName);
    char *szNextArg;
    // Number of attributes
    int iArgCount = va_arg(attributeList, int);
    for(int i = 0; i < iArgCount; i++)
    {
        int index = va_arg(attributeList, int);
        szNextArg = va_arg(attributeList, char*);
        extshader.program->bindAttributeLocation(szNextArg,index);
    }
    va_end(attributeList);

    if(!extshader.program->link())
    {
        qWarning("Shader Program link failed.");
        return 0;
    }

    // Add to the table

    extshader.name = QString(shadername);
    shaderTable.push_back(extshader);

    return extshader.program->programId();
}

// Load the shader from source, with the supplied named attributes
GLuint GLShaderManager::LoadShaderPairSrcWithAttributes(const char * shadername, const char *szVertexCode, const char *szFragmentCode, ...)
{
    EXTERNPROGRAM extshader;

    // Check for duplicate
    GLuint uiShader = LookupShader(shadername);
    if(uiShader != 0)
    {
        return uiShader;
    }

    extshader.program = new QOpenGLShaderProgram();
    extshader.program->addShaderFromSourceCode(QOpenGLShader::Vertex,szVertexCode);
    extshader.program->addShaderFromSourceCode(QOpenGLShader::Fragment,szFragmentCode);

    // List of attributes
    va_list attributeList;
    va_start(attributeList, szFragmentCode);
    char *szNextArg;
    // Number of attributes
    int iArgCount = va_arg(attributeList, int);
    for(int i = 0; i < iArgCount; i++)
    {
        int index = va_arg(attributeList, int);
        szNextArg = va_arg(attributeList, char*);
        extshader.program->bindAttributeLocation(szNextArg,index);
    }
    va_end(attributeList);

    if(!extshader.program->link())
    {
        qWarning("Shader Program link failed.");
        return 0;
    }

    // Add to the table

    extshader.name = QString(shadername);
    shaderTable.push_back(extshader);

    return extshader.program->programId();
}
