#ifndef BLOCKDATABATCH_H
#define BLOCKDATABATCH_H

#include <QDebug>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

#define BLOCK_SIZE (float)1.0

enum FACE {FACE_UP=0,FACE_DOWN,FACE_LEFT,FACE_RIGHT,FACE_FRONT,FACE_BACK};

typedef struct
{
    unsigned short textureId;

    //start form left bottom corner
    QVector2D startPosition;

    float deltaLength;

    QString textureName;
}
texMap;

//stores data needed by a base block

class BlockDataBatch
{

public:

    int nNumVerts;
    int nTexCoordsUnits;
    QVector<float> vVerts;
    QVector<float> vNormals;
    QVector<float> * vTexCoords;

public:

    BlockDataBatch()
    {
        vTexCoords = Q_NULLPTR;
    }

    ~BlockDataBatch()
    {
        if(vTexCoords!=Q_NULLPTR)
        {
            //delete vTexCoords;
        }
    }

    void begin(int vertsCount,int texSetCount = 0)
    {
        this->nNumVerts = vertsCount;

        this->nTexCoordsUnits = texSetCount;

        if(texSetCount > 4) texSetCount = 4;

        if(texSetCount == 0) return;

        vTexCoords = new QVector<float>[texSetCount];

        for(int i=0;i<texSetCount;i++)
        {
            vTexCoords[i].clear();
        }
    }

    void setvData(float * vdata)
    {
        for(int i=0;i<nNumVerts*3;i++)
        {
            vVerts.push_back(*vdata);
            vdata++;
        }
    }

    void setnData(float * ndata)
    {
        for(int i=0;i<nNumVerts*3;i++)
        {
            vNormals.push_back(*ndata);
            ndata++;
        }
    }

    void settData(float * tdata,int unit)
    {
        if(unit<0|| unit >= nTexCoordsUnits)
        {
            qFatal("Wrong texture unit number set.");
        }
        for(int i=0;i<nNumVerts*2;i++)
        {
            vTexCoords[unit].push_back(*tdata);
            tdata++;
        }
    }

    void add2fv(QVector<float> &vec, float x, float y)
    {
        vec.push_back(x);
        vec.push_back(y);
        return;
    }

    void add3fv(QVector<float> &vec,float x,float y,float z)
    {
        vec.push_back(x);
        vec.push_back(y);
        vec.push_back(z);
        return;
    }

    //NOTE: thie function should only be called if the cube is a default cube shape
    void getDefaultCubeFaceVNData(FACE face,QVector<float> &vdata,QVector<float> &ndata,QVector3D offset)
    {
        if(this->nNumVerts != 36)
        {
            qDebug("The cube may not be an default cube.Can't get accurate face data");
            return;
        }

        float delta;

        switch (face)
        {
        case FACE_UP:
            //verts and normals
            for(int i=0;i<18;i++)
            {
                if(i%3 == 0) delta = offset.x();
                if(i%3 == 1) delta = offset.y();
                if(i%3 == 2) delta = offset.z();
                vdata.push_back(this->vVerts[i+0] + delta);
                ndata.push_back(this->vNormals[i+0]);
            }
            break;
        case FACE_DOWN:
            //verts and normals
            for(int i=0;i<18;i++)
            {
                if(i%3 == 0) delta = offset.x();
                if(i%3 == 1) delta = offset.y();
                if(i%3 == 2) delta = offset.z();
                vdata.push_back(this->vVerts[i+18]+delta);
                ndata.push_back(this->vNormals[i+18]);
            }
            break;
        case FACE_LEFT:
            //verts and normals
            for(int i=0;i<18;i++)
            {
                if(i%3 == 0) delta = offset.x();
                if(i%3 == 1) delta = offset.y();
                if(i%3 == 2) delta = offset.z();
                vdata.push_back(this->vVerts[i+36]+delta);
                ndata.push_back(this->vNormals[i+36]);
            }
            break;
        case FACE_RIGHT:
            //verts and normals
            for(int i=0;i<18;i++)
            {
                if(i%3 == 0) delta = offset.x();
                if(i%3 == 1) delta = offset.y();
                if(i%3 == 2) delta = offset.z();
                vdata.push_back(this->vVerts[i+54]+delta);
                ndata.push_back(this->vNormals[i+54]);
            }
            break;
        case FACE_FRONT:
            //verts and normals
            for(int i=0;i<18;i++)
            {
                if(i%3 == 0) delta = offset.x();
                if(i%3 == 1) delta = offset.y();
                if(i%3 == 2) delta = offset.z();
                vdata.push_back(this->vVerts[i+72]+delta);
                ndata.push_back(this->vNormals[i+72]);
            }
            break;
        case FACE_BACK:
            //verts and normals
            for(int i=0;i<18;i++)
            {
                if(i%3 == 0) delta = offset.x();
                if(i%3 == 1) delta = offset.y();
                if(i%3 == 2) delta = offset.z();
                vdata.push_back(this->vVerts[i+90]+delta);
                ndata.push_back(this->vNormals[i+90]);
            }
            break;
        }
    }

    void getDefaultCubeFaceTData(FACE face,QVector<float> &tdata,int texunit)
    {
        if(this->nNumVerts != 36)
        {
            qDebug("The cube may not be an default cube.Can't get accurate face data");
            return;
        }

        if(texunit < 0 || texunit >= nTexCoordsUnits) return;

        switch (face)
        {
        case FACE_UP:
            //texture coordinates
            for(int i=0;i<12;i++)
            {
                tdata.push_back(this->vTexCoords[texunit].at(i+0) );
            }
            break;
        case FACE_DOWN:
            //texture coordinates
            for(int i=0;i<12;i++)
            {
                tdata.push_back(this->vTexCoords[texunit].at(i+12) );
            }
            break;
        case FACE_LEFT:
            //texture coordinates
            for(int i=0;i<12;i++)
            {
                tdata.push_back(this->vTexCoords[texunit].at(i+24) );
            }
            break;
        case FACE_RIGHT:
            //texture coordinates
            for(int i=0;i<12;i++)
            {
                tdata.push_back(this->vTexCoords[texunit].at(i+36) );
            }
            break;
        case FACE_FRONT:
            //texture coordinates
            for(int i=0;i<12;i++)
            {
                tdata.push_back(this->vTexCoords[texunit].at(i+48) );
            }
            break;
        case FACE_BACK:
            //texture coordinates
            for(int i=0;i<12;i++)
            {
                tdata.push_back(this->vTexCoords[texunit].at(i+60) );
            }
            break;
        }
    }

    void getAllVNData(QVector<float> &vdata,QVector<float> &ndata,QVector3D offset)
    {
        float delta = 0.0f;
        for(int i=0;i<vVerts.count();i++)
        {
            if(i%3 == 0) delta = offset.x();
            if(i%3 == 1) delta = offset.y();
            if(i%3 == 2) delta = offset.z();
            vdata.push_back(vVerts[i]+delta);
        }
        for(int i=0;i<vNormals.count();i++)
        {
            ndata.push_back(vNormals[i]);
        }
    }

    void getTData(QVector<float> &tdata,int texunit)
    {
        if(texunit<0 || texunit >=nTexCoordsUnits) return;
        for(int i=0;i<vTexCoords[texunit].count();i++)
        {
            tdata.push_back(vTexCoords[texunit].at(i));
        }
    }

    void loadDefaultCube(float fRadius)
       {
           this->begin(36,1);
           // UP AND DOWN
           // UP
           this->add3fv(vNormals,0.0f, 0.0f, fRadius);
           this->add2fv(vTexCoords[0],1, 0.0f);
           this->add3fv(vVerts,fRadius, -fRadius, fRadius);

           this->add3fv(vNormals,0.0f, 0.0f, fRadius);
           this->add2fv(vTexCoords[0],1, 1);
           this->add3fv(vVerts,fRadius, fRadius, fRadius);

           this->add3fv(vNormals,0.0f, 0.0f, fRadius);
           this->add2fv(vTexCoords[0], 0.0f, 1);
           this->add3fv(vVerts,-fRadius, fRadius, fRadius);

           this->add3fv(vNormals,0.0f, 0.0f, fRadius);
           this->add2fv(vTexCoords[0], 0.0f, 1);
           this->add3fv(vVerts,-fRadius, fRadius, fRadius);

           this->add3fv(vNormals,0.0f, 0.0f, fRadius);
           this->add2fv(vTexCoords[0], 0.0f, 0.0f);
           this->add3fv(vVerts,-fRadius, -fRadius, fRadius);

           this->add3fv(vNormals,0.0f, 0.0f, fRadius);
           this->add2fv(vTexCoords[0],1, 0.0f);
           this->add3fv(vVerts,fRadius, -fRadius, fRadius);

           // DOWN
           this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
           this->add2fv(vTexCoords[0],1, 0.0f);
           this->add3fv(vVerts,fRadius, -fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
           this->add2fv(vTexCoords[0], 0.0f, 0.0f);
           this->add3fv(vVerts,-fRadius, -fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
           this->add2fv(vTexCoords[0], 0.0f, 1);
           this->add3fv(vVerts,-fRadius, fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
           this->add2fv(vTexCoords[0], 0.0f, 1);
           this->add3fv(vVerts,-fRadius, fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
           this->add2fv(vTexCoords[0],1, 1);
           this->add3fv(vVerts,fRadius, fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
           this->add2fv(vTexCoords[0],1, 0.0f);
           this->add3fv(vVerts,fRadius, -fRadius, -fRadius);



           //LEFT AND RIGHT
           // LEFT
           this->add3fv(vNormals,-fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0],1, 1);
           this->add3fv(vVerts,-fRadius, fRadius, fRadius);

           this->add3fv(vNormals,-fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0],1, 0.0f);
           this->add3fv(vVerts,-fRadius, fRadius, -fRadius);

           this->add3fv(vNormals,-fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 0.0f);
           this->add3fv(vVerts,-fRadius, -fRadius, -fRadius);

           this->add3fv(vNormals,-fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0],1, 1);
           this->add3fv(vVerts,-fRadius, fRadius, fRadius);

           this->add3fv(vNormals,-fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 0.0f);
           this->add3fv(vVerts,-fRadius, -fRadius, -fRadius);

           this->add3fv(vNormals,-fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 1);
           this->add3fv(vVerts,-fRadius, -fRadius, fRadius);

           // RIGHT
           this->add3fv(vNormals,fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 0.0f);
           this->add3fv(vVerts,fRadius, -fRadius, -fRadius);

           this->add3fv(vNormals,fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0],1, 0.0f);
           this->add3fv(vVerts,fRadius, fRadius, -fRadius);

           this->add3fv(vNormals,fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0],1, 1);
           this->add3fv(vVerts,fRadius, fRadius, fRadius);

           this->add3fv(vNormals,fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0],1,1);
           this->add3fv(vVerts,fRadius, fRadius, fRadius);

           this->add3fv(vNormals,fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 1);
           this->add3fv(vVerts,fRadius, -fRadius, fRadius);

           this->add3fv(vNormals,fRadius, 0.0f, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 0.0f);
           this->add3fv(vVerts,fRadius, -fRadius, -fRadius);



           //FRONT AND BACK
           // FRONT
           this->add3fv(vNormals,0.0f, fRadius, 0.0f);
           this->add2fv(vTexCoords[0],1, 1);
           this->add3fv(vVerts,fRadius, fRadius, fRadius);

           this->add3fv(vNormals,0.0f, fRadius, 0.0f);
           this->add2fv(vTexCoords[0],1, 0.0f);
           this->add3fv(vVerts,fRadius, fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, fRadius, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 0.0f);
           this->add3fv(vVerts,-fRadius, fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, fRadius, 0.0f);
           this->add2fv(vTexCoords[0],1, 1);
           this->add3fv(vVerts,fRadius, fRadius, fRadius);

           this->add3fv(vNormals,0.0f, fRadius, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 0.0f);
           this->add3fv(vVerts,-fRadius, fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, fRadius, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 1);
           this->add3fv(vVerts,-fRadius, fRadius, fRadius);


           // BACK
           this->add3fv(vNormals,0.0f, -fRadius, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 0.0f);
           this->add3fv(vVerts,-fRadius, -fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, -fRadius, 0.0f);
           this->add2fv(vTexCoords[0],1, 0.0f);
           this->add3fv(vVerts,fRadius, -fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, -fRadius, 0.0f);
           this->add2fv(vTexCoords[0],1, 1);
           this->add3fv(vVerts,fRadius, -fRadius, fRadius);

           this->add3fv(vNormals,0.0f, -fRadius, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 1);
           this->add3fv(vVerts,-fRadius, -fRadius, fRadius);

           this->add3fv(vNormals,0.0f, -fRadius, 0.0f);
           this->add2fv(vTexCoords[0], 0.0f, 0.0f);
           this->add3fv(vVerts,-fRadius, -fRadius, -fRadius);

           this->add3fv(vNormals,0.0f, -fRadius, 0.0f);
           this->add2fv(vTexCoords[0],1, 1);
           this->add3fv(vVerts,fRadius, -fRadius, fRadius);
       }

    void loadDefaultCube_Gen_TextureUV(float fRadius,int idUp,int idDown,int idLeft,int idRight,int idFront,int idBack,QVector<texMap> &tmap)
    {
        this->begin(36,1 );

        QVector2D startPosition;

        float deltaLength;

        // UP AND DOWN
        // UP
        startPosition = tmap.at(idUp).startPosition;
        deltaLength = tmap.at(idUp).deltaLength;
        this->add3fv(vNormals,0.0f, 0.0f, fRadius);
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y() );
        this->add3fv(vVerts,fRadius, -fRadius, fRadius);

        this->add3fv(vNormals,0.0f, 0.0f, fRadius);
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y()+deltaLength );
        this->add3fv(vVerts,fRadius, fRadius, fRadius);

        this->add3fv(vNormals,0.0f, 0.0f, fRadius);
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y()+deltaLength );
        this->add3fv(vVerts,-fRadius, fRadius, fRadius);

        this->add3fv(vNormals,0.0f, 0.0f, fRadius);
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y()+deltaLength );
        this->add3fv(vVerts,-fRadius, fRadius, fRadius);

        this->add3fv(vNormals,0.0f, 0.0f, fRadius);
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y() );
        this->add3fv(vVerts,-fRadius, -fRadius, fRadius);

        this->add3fv(vNormals,0.0f, 0.0f, fRadius);
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y() );
        this->add3fv(vVerts,fRadius, -fRadius, fRadius);

        // DOWN
        startPosition = tmap.at(idDown).startPosition;
        deltaLength = tmap.at(idDown).deltaLength;
        this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y() );
        this->add3fv(vVerts,fRadius, -fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y() );
        this->add3fv(vVerts,-fRadius, -fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y()+deltaLength );
        this->add3fv(vVerts,-fRadius, fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y()+deltaLength );
        this->add3fv(vVerts,-fRadius, fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y()+deltaLength );
        this->add3fv(vVerts,fRadius, fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, 0.0f, -fRadius);
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y() );
        this->add3fv(vVerts,fRadius, -fRadius, -fRadius);



        //LEFT AND RIGHT
        // LEFT
        startPosition = tmap.at(idLeft).startPosition;
        deltaLength = tmap.at(idLeft).deltaLength;
        this->add3fv(vNormals,-fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y()+deltaLength );
        this->add3fv(vVerts,-fRadius, fRadius, fRadius);

        this->add3fv(vNormals,-fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y() );
        this->add3fv(vVerts,-fRadius, fRadius, -fRadius);

        this->add3fv(vNormals,-fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y() );
        this->add3fv(vVerts,-fRadius, -fRadius, -fRadius);

        this->add3fv(vNormals,-fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y()+deltaLength );
        this->add3fv(vVerts,-fRadius, fRadius, fRadius);

        this->add3fv(vNormals,-fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y() );
        this->add3fv(vVerts,-fRadius, -fRadius, -fRadius);

        this->add3fv(vNormals,-fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y()+deltaLength );
        this->add3fv(vVerts,-fRadius, -fRadius, fRadius);

        // RIGHT
        startPosition = tmap.at(idRight).startPosition;
        deltaLength = tmap.at(idRight).deltaLength;
        this->add3fv(vNormals,fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y() );
        this->add3fv(vVerts,fRadius, -fRadius, -fRadius);

        this->add3fv(vNormals,fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y() );
        this->add3fv(vVerts,fRadius, fRadius, -fRadius);

        this->add3fv(vNormals,fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y()+deltaLength );
        this->add3fv(vVerts,fRadius, fRadius, fRadius);

        this->add3fv(vNormals,fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength,startPosition.y()+deltaLength );
        this->add3fv(vVerts,fRadius, fRadius, fRadius);

        this->add3fv(vNormals,fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y()+deltaLength );
        this->add3fv(vVerts,fRadius, -fRadius, fRadius);

        this->add3fv(vNormals,fRadius, 0.0f, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y() );
        this->add3fv(vVerts,fRadius, -fRadius, -fRadius);



        //FRONT AND BACK
        // FRONT
        startPosition = tmap.at(idFront).startPosition;
        deltaLength = tmap.at(idFront).deltaLength;
        this->add3fv(vNormals,0.0f, fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y()+deltaLength );
        this->add3fv(vVerts,fRadius, fRadius, fRadius);

        this->add3fv(vNormals,0.0f, fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y() );
        this->add3fv(vVerts,fRadius, fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y() );
        this->add3fv(vVerts,-fRadius, fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y()+deltaLength );
        this->add3fv(vVerts,fRadius, fRadius, fRadius);

        this->add3fv(vNormals,0.0f, fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y() );
        this->add3fv(vVerts,-fRadius, fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y()+deltaLength );
        this->add3fv(vVerts,-fRadius, fRadius, fRadius);


        // BACK
        startPosition = tmap.at(idBack).startPosition;
        deltaLength = tmap.at(idBack).deltaLength;
        this->add3fv(vNormals,0.0f, -fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y() );
        this->add3fv(vVerts,-fRadius, -fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, -fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y() );
        this->add3fv(vVerts,fRadius, -fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, -fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y()+deltaLength );
        this->add3fv(vVerts,fRadius, -fRadius, fRadius);

        this->add3fv(vNormals,0.0f, -fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y()+deltaLength );
        this->add3fv(vVerts,-fRadius, -fRadius, fRadius);

        this->add3fv(vNormals,0.0f, -fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x(), startPosition.y() );
        this->add3fv(vVerts,-fRadius, -fRadius, -fRadius);

        this->add3fv(vNormals,0.0f, -fRadius, 0.0f );
        this->add2fv(vTexCoords[0],startPosition.x()+deltaLength, startPosition.y()+deltaLength );
        this->add3fv(vVerts,fRadius, -fRadius, fRadius);
    }

};


#endif // BLOCKDATABATCH_H
