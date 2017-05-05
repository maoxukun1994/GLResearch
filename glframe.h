#ifndef GLFRAME_H
#define GLFRAME_H

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QQuaternion>

enum GLFRAME_MODE {GL_FRAME_FREECAMERA=0,GL_FRAME_FPSCHARACTER};

class GLFrame
{
private:
    //private virables

    float fov;
    float aspratio;
    float nearp;
    float farp;

    GLFRAME_MODE mode;

protected:
    //protected virables

    QVector3D vPosition;	// Where am I?
    QVector3D vForward;	// Where am I going?
    QVector3D vUp;		// Which way is up?

public:
    //public functions

    GLFrame();

    void setMode(GLFRAME_MODE mode);

    void setPosition(QVector3D pos);

    void setvUp(QVector3D up);

    void setvForward(QVector3D forward);

    void setPerspective(float fov,float aspratio,float nearp,float farp);

    void getVMatrix(QMatrix4x4 &mat);

    void getPMatrix(QMatrix4x4 &mat);

    void getVPMatrix(QMatrix4x4 &mat);

    QVector3D getPosition();

    QMatrix4x4 getVPRotationMatrix();

    QMatrix4x4 getVPMatrix();

    QMatrix4x4 getVMatrix();

    QMatrix4x4 getPMatrix();

    void moveForward(float delta);

    void moveRight(float delta);

    void moveUp(float delta);

    void moveXAxis(float delta);

    void moveYAxis(float delta);

    void moveZAxis(float delta);

    void rotateH(float delta);

    void rotateV(float delta);

    void rotateHV(float deltah,float deltav);
};

#endif // GLFRAME_H
