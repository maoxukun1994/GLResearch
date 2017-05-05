#ifndef BLOCKINSTANCE_H
#define BLOCKINSTANCE_H

#include "BlockBase.h"

class BlockInstance
{
public:

    unsigned short baseBlockId;

    //decide if this instance can be culled using optimiztion methods
    bool isCullEnabled;

    //block facing 0:x+ 1:x- 2:y+ 3:y- 4:z+ 5:z-
    unsigned char facing;

    //bit 0~5 up down left right front back
    unsigned char nvisible;

public:

    BlockInstance()
    {
        BlockInstance::initDefaultValues(this);
    }

    static void initDefaultValues(BlockInstance *ins)
    {
        ins->baseBlockId = 0;
        ins->facing = 0;
        ins->nvisible = 0;
        ins->isCullEnabled = false;
    }

};

#endif // BLOCKINSTANCE_H
