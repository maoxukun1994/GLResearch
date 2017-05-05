#ifndef BLOCK_H
#define BLOCK_H

#include "BlockDataBatch.h"

#include <QPair>
#include <QString>

class BlockBase
{
public:

    QString blockName;

    unsigned short blockId;

    QString typeName;

    unsigned short typeId;

    //universial texture id
    unsigned short texId;
    //texture id for each faces
    unsigned short texIdUp;
    unsigned short texIdDown;
    unsigned short texIdFront;
    unsigned short texIdBack;
    unsigned short texIdLeft;
    unsigned short texIdRight;

    //block vertex databatch
    BlockDataBatch * bdata;

    bool isDefaultCubeMesh;


public:

    BlockBase()
    {
        blockId = 0;
        typeId =0;

        texId=0;
        texIdUp=0;
        texIdDown=0;
        texIdFront=0;
        texIdBack=0;
        texIdLeft=0;
        texIdRight=0;

        bdata = Q_NULLPTR;

        isDefaultCubeMesh = false;

    }

    ~BlockBase()
    {
        if(bdata != Q_NULLPTR)
        {
            delete bdata;
        }
    }

    //for now,it juse fill the batch with a default cube data
    bool initBatchDataFromDefaultCube(QVector<texMap> &tmap)
    {
        this->bdata = new BlockDataBatch();

        this->isDefaultCubeMesh = true;

        if(this->texId != 0)
        {
            if(this->texIdBack == 0) this->texIdBack = texId;
            if(this->texIdFront == 0) this->texIdFront = texId;
            if(this->texIdUp == 0) this->texIdUp = texId;
            if(this->texIdDown == 0) this->texIdDown = texId;
            if(this->texIdLeft== 0) this->texIdLeft = texId;
            if(this->texIdRight == 0) this->texIdRight = texId;
        }

        bdata->loadDefaultCube_Gen_TextureUV(BLOCK_SIZE/2,texIdUp,texIdDown,texIdLeft,texIdRight,texIdFront,texIdBack,tmap);

        return true;
    }

    //maybe we can add import model functionality in this later
    bool initBatchData()
    {
        return true;
    }
};




#endif // BLOCK_H
