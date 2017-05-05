#include "glframe.h"

GLFrame::GLFrame()
{
    vPosition = QVector3D(0,0,0);
    vForward = QVector3D(0,1,0);
    vUp = QVector3D(0,0,1);

    this->mode = GL_FRAME_FREECAMERA;
}

void GLFrame::setMode(GLFRAME_MODE mode)
{
    this->mode = mode;
}

void GLFrame::setPosition(QVector3D pos)
{
    this->vPosition = pos;
}

void GLFrame::setvUp(QVector3D up)
{
    QVector3D left = QVector3D::crossProduct(vUp,vForward);
    this->vUp = up.normalized();
    this->vForward = QVector3D::crossProduct(left,vUp).normalized();
}

void GLFrame::setvForward(QVector3D forward)
{
    QVector3D left = QVector3D::crossProduct(vUp,vForward);
    this->vForward = forward.normalized();
    this->vUp =QVector3D::crossProduct(vForward,left).normalized();
}



void GLFrame::setPerspective(float fov,float aspratio,float nearp,float farp)
{
    this->fov = fov;
    this->aspratio = aspratio;
    this->nearp = nearp;
    this->farp = farp;
}

void GLFrame::getVMatrix(QMatrix4x4 &mat)
{
    mat.setToIdentity();
    mat.lookAt(vPosition,vPosition+vForward,vUp);
}

void GLFrame::getPMatrix(QMatrix4x4 &mat)
{
    mat.setToIdentity();
    mat.perspective(fov,aspratio,nearp,farp);
}

void GLFrame::getVPMatrix(QMatrix4x4 &mat)
{
    QMatrix4x4 p = this->getPMatrix();
    this->getVMatrix(mat);
    mat = p * mat;
}

QMatrix4x4 GLFrame::getVPMatrix()
{
    QMatrix4x4 vp = this->getPMatrix();
    vp = vp * this->getVMatrix();
    return vp;
}

QMatrix4x4 GLFrame::getVPRotationMatrix()
{
    QMatrix4x4 mat;
    mat.setToIdentity();
    mat.lookAt(QVector3D(0,0,0),vForward,vUp);

    return this->getPMatrix()*mat;
}

QMatrix4x4 GLFrame::getVMatrix()
{
    QMatrix4x4 mat;
    mat.setToIdentity();
    mat.lookAt(vPosition,vPosition+vForward,vUp);
    return mat;
}

QMatrix4x4 GLFrame::getPMatrix()
{
    QMatrix4x4 mat;
    mat.setToIdentity();
    mat.perspective(fov,aspratio,nearp,farp);
    return mat;
}


QVector3D GLFrame::getPosition()
{
    return this->vPosition;
}

void GLFrame::moveForward(float delta)
{
    switch (mode)
    {
    case GL_FRAME_FREECAMERA:
        this->vPosition += this->vForward * delta;
        break;
    case GL_FRAME_FPSCHARACTER:
        QVector3D nvForward = vForward;
        nvForward.setZ(0.0f);
        nvForward.normalize();
        this->vPosition += nvForward * delta;
        break;
    }
}

void GLFrame::moveRight(float delta)
{
    //if delta > 0,then move "right" else move "left"
    this->vPosition += QVector3D::crossProduct(vForward,vUp).normalized() * delta;
}

void GLFrame::moveUp(float delta)
{
    switch (mode)
    {
    case GL_FRAME_FREECAMERA:
        this->vPosition += this->vUp * delta;
        break;
    case GL_FRAME_FPSCHARACTER:
        this->vPosition += QVector3D(0,0,1) * delta;
        break;
    }
}

void GLFrame::moveXAxis(float delta)
{
    this->vPosition += QVector3D(1,0,0) * delta;
}

void GLFrame::moveYAxis(float delta)
{
    this->vPosition += QVector3D(0,1,0) * delta;
}

void GLFrame::moveZAxis(float delta)
{
    this->vPosition += QVector3D(0,0,1) * delta;
}

void GLFrame::rotateH(float delta)
{
    QMatrix4x4 mat;
    mat.setToIdentity();
    mat.rotate(delta,QVector3D(0,0,1));

    vForward = mat * vForward;
    vUp = mat * vUp;
    vForward.normalize();
    vUp.normalize();

    //correct up in case of float precision lose
    QVector3D left = QVector3D::crossProduct(vUp,vForward);
    vUp = QVector3D::crossProduct(vForward,left).normalized();
}

void GLFrame::rotateV(float delta)
{
    QMatrix4x4 mat;
    QVector3D origLeft = QVector3D::crossProduct(vUp,vForward);
    origLeft.setZ(0.0f);
    mat.setToIdentity();
    mat.rotate(delta,origLeft);

    vForward = mat * vForward;
    vUp = mat * vUp;

    QVector3D left = QVector3D::crossProduct(vUp,vForward);
    vUp = QVector3D::crossProduct(vForward,left).normalized();
}

void GLFrame::rotateHV(float deltah,float deltav)
{
    rotateH(deltah);
    rotateV(deltav);
}
