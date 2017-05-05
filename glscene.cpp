#include "glscene.h"

GLScene::GLScene() : GLBase()
{
    this->checkAndInit();

    cameramovingF = cameramovingB = false;
    cameramovingL = cameramovingR = false;
    cameramovingU = cameramovingD = false;

    this->shaderManager = Q_NULLPTR;

    this->chunkmgr = Q_NULLPTR;

    this->sky = Q_NULLPTR;

    drawWidth = drawHeight = 0;
    portWidth = portHeight = 0;
    marginH = marginV = 0;
}

GLScene::~GLScene()
{
    if(shaderManager != Q_NULLPTR)
    {
        delete shaderManager;
    }
    if(this->chunkmgr != Q_NULLPTR)
    {
        delete chunkmgr;
    }
    if(this->sky != Q_NULLPTR)
    {
        delete sky;
    }
}

void GLScene::changeDrawProperty(int drawWidth, int drawHeight)
{
    this->drawWidth = drawWidth;

    this->drawHeight = drawHeight;

    if((float) drawWidth/(float)drawHeight/(float)2 > DRAW_ASPECT_RATIO)
    {
        portHeight = drawHeight;
        portWidth = (int)( (float)portHeight * DRAW_ASPECT_RATIO );

        marginH = (drawWidth - portWidth * 2)/2;
        marginV = 0;
    }
    else
    {
        portWidth = drawWidth/2;
        portHeight =(int)( (float)portWidth / DRAW_ASPECT_RATIO );

        marginH = 0;
        marginV = (drawHeight - portHeight)/2;
    }
}


void GLScene::sceneInit()
{

    this->shaderManager = new GLShaderManager();

    /*
    if( !shaderManager->InitializeStockShaders() )
    {
        qFatal("Stock shaders init failed.");
        return;
    }
    */
#ifndef PLATFORM_GL_ES
    shaderManager->LoadShaderPairWithAttributes("ADSPhong",":/Shaders/DESKTOP_3_3/adsphong.vert",":/Shaders/DESKTOP_3_3/adsphong.frag",3,GLBASE_ATTRIBUTE_VERTEX,"vVertex",GLBASE_ATTRIBUTE_NORMAL,"vNormal",GLBASE_ATTRIBUTE_TEXTURE0,"vTexCoords");
    shaderManager->LoadShaderPairWithAttributes("SkyBox",":/Shaders/DESKTOP_3_3/sky_box.vert",":/Shaders/DESKTOP_3_3/sky_box.frag",1,GLBASE_ATTRIBUTE_VERTEX,"vVertex");
#else
    shaderManager->LoadShaderPairWithAttributes("ADSPhong",":/Shaders//ES_2_0/adsphong.vert",":/Shaders//ES_2_0/adsphong.frag",3,GLBASE_ATTRIBUTE_VERTEX,"vVertex",GLBASE_ATTRIBUTE_NORMAL,"vNormal",GLBASE_ATTRIBUTE_TEXTURE0,"vTexCoords");
    shaderManager->LoadShaderPairWithAttributes("SkyBox",":/Shaders/ES_2_0/sky_box.vert",":/Shaders/ES_2_0/sky_box.frag",1,GLBASE_ATTRIBUTE_VERTEX,"vVertex");
#endif


    QOpenGLShaderProgram * tmp = shaderManager->UseExtShader(0);
    tmp->setUniformValue("vLightPosition",QVector3D(0,0,100));
    tmp->setUniformValue("ambientColor",QVector4D(0.2,0.2,0.2,1.0));
    tmp->setUniformValue("diffuseColor",QVector4D(0.6,0.6,0.6,1.0));
    tmp->setUniformValue("specularColor",QVector4D(0.3,0.3,0.3,1.0));
    tmp->setUniformValue("colorMap",0);

    /*
    QOpenGLShaderProgram * tmp = shaderManager->UseStockShader(GLBASE_SHADER_TEXTURE_POINT_LIGHT_DIFF);
    tmp->setUniformValue("vLightPos",QVector3D(0,0,1));
    tmp->setUniformValue("vColor",QVector4D(1,1,1,1));
    tmp->setUniformValue("textureUnit0",0);
    */

    camera.setMode(GL_FRAME_FPSCHARACTER);
    camera.setPosition(QVector3D(64,64,128));
    camera.setvForward(QVector3D(0,1,-1));
    camera.setPerspective(60.0f,DRAW_ASPECT_RATIO,0.5,300.0);


    this->chunkmgr = new ChunkManager();
    chunkmgr->initBlockMgrAndUpdateWorker();
    chunkmgr->setPosition(QVector2D(64,64));
    chunkmgr->perpareForDraw(tmp,&camera);

    //for skybox
    tmp = shaderManager->UseExtShader(1);
    tmp->setUniformValue("cubeMap",0);
    this->sky = new SkyBox();
    sky->loadDefaultSky(tmp,&camera);
}

void GLScene::sceneUpdate()
{
    //draw chunks
    chunkmgr->drawAllChunks();

    //update chunk update position
    chunkmgr->setPosition(camera.getPosition().toVector2D());

    //draw skybox
    sky->drawSkyBox();

    //movecamera
    if(cameramovingF)
    {
        camera.moveForward(0.2);
    }
    if(cameramovingB)
    {
        camera.moveForward(-0.2);
    }
    if(cameramovingR)
    {
        camera.moveRight(0.2);
    }
    if(cameramovingL)
    {
        camera.moveRight(-0.2);
    }
    if(cameramovingU)
    {
        camera.moveUp(0.5);
    }
    if(cameramovingD)
    {
        camera.moveUp(-0.5);
    }
}

