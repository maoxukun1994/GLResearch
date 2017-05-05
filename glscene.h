#ifndef GLSCENE_H
#define GLSCENE_H

#include "glbase.h"
#include "glbatch.h"
#include "glframe.h"
#include "glshadermanager.h"

#include "GameSrc/Skybox.h"
#include "GameSrc/ChunkManager.h"

#include <QBasicTimer>
#include <QOpenGLTexture>

class GLScene:protected GLBase
{

private:

    GLShaderManager * shaderManager;

    ChunkManager * chunkmgr;

    SkyBox * sky;

    int drawWidth,drawHeight,portWidth,portHeight,marginH,marginV;

public:
    //public virables

    GLFrame camera;

    bool cameramovingF,cameramovingB,cameramovingL,cameramovingR,cameramovingU,cameramovingD;

public:

    GLScene();

    ~GLScene();

    void sceneInit();

    void sceneUpdate();

    void changeDrawProperty(int drawWidth, int drawHeight);

};

#endif // GLSCENE_H
