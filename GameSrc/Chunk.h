#ifndef CHUNK_H
#define CHUNK_H

#include "glbatch.h"


#include "BlockInstance.h"
#include "BlockManager.h"
#include "WorldGenerator.h"

#include <QTime>
#include <QPoint>
#include <QColor>
#include <QImage>
#include <QVector2D>
#include <QVector3D>

#define CHUNK_SIDE_SIZE 16
#define CHUNK_HEIGHT_SIZE 256

//decide how many glbatch should a chunk have to render.this number must can devide the CHUNK_HEIGHT_SIZE
#define CHUNK_BATCH_DEVIDE_SIZE 2


class Chunk
{
public:

    BlockInstance chunkblocks[CHUNK_SIDE_SIZE][CHUNK_SIDE_SIZE][CHUNK_HEIGHT_SIZE];

    //chunk's left-down-bottom posotion in world
    QVector2D pos;

    //chunk position in the world
    int sectorX,sectorY;

    BlocksManager * blkmgr;

private:

    bool isChunkLoaded;

    WorldGenerator world_gen;

public:

    Chunk(BlocksManager * mgr)
    {
        this->blkmgr = mgr;

        sectorX = sectorY = 0;

        this->pos = QVector2D(0,0);

        isChunkLoaded = false;
    }

    ~Chunk()
    {
    }

    void setChunkSector(QVector2D worldpos)
    {
        float x = worldpos.x();
        float y = worldpos.y();
        sectorX =(int)( x / (CHUNK_SIDE_SIZE * BLOCK_SIZE) );
        sectorY =(int)( y / (CHUNK_SIDE_SIZE * BLOCK_SIZE) );
        this->pos.setX( CHUNK_SIDE_SIZE * BLOCK_SIZE * sectorX);
        this->pos.setY( CHUNK_SIDE_SIZE * BLOCK_SIZE * sectorY);
    }

    void setChunkSector(int x,int y)
    {
        this->sectorX = x;
        this->sectorY = y;
        this->pos.setX( CHUNK_SIDE_SIZE * BLOCK_SIZE * sectorX);
        this->pos.setY( CHUNK_SIDE_SIZE * BLOCK_SIZE * sectorY);
    }

    QPoint getChunkSector()
    {
        return QPoint(sectorX,sectorY);
    }

    static QPoint getChunkSector(QVector2D worldpos)
    {
        QPoint secpos;
        float x = worldpos.x();
        float y = worldpos.y();
        secpos.setX( (int)( x / (CHUNK_SIDE_SIZE * BLOCK_SIZE) ) );
        secpos.setY( (int)( y / (CHUNK_SIDE_SIZE * BLOCK_SIZE) ) );
        return secpos;
    }

    //load data from saved block files or generate (by current sector value)
    bool loadChunkData()
    {
        if(isChunkLoaded)
        {
            qDebug("Chunk data already loaded.Call unloadChunkData before load another.");
            return false;
        }


        /*
        //try a bitmap loader
        QImage bitmap(":/Textures/Bitmap.bmp");
        if(bitmap.isNull())
        {
            qDebug("Error,No bitmap image file loaded.");
            return false;
        }
        //bitmap loader
        for(unsigned short i=0;i<CHUNK_SIDE_SIZE;++i)
        {
            for(unsigned short j=0;j<CHUNK_SIDE_SIZE;++j)
            {
                for(unsigned short k=0;k<CHUNK_HEIGHT_SIZE;++k)
                {
                    BlockInstance::initDefaultValues(&(chunkblocks[i][j][k]));
                }

                //try a bitmap loader
                int bw = bitmap.width(),bh = bitmap.height();
                int ix = this->sectorX * CHUNK_SIDE_SIZE + i;
                int iy = this->sectorY * CHUNK_SIDE_SIZE + j;
                if(ix < 0) ix = -ix;
                if(iy < 0) iy = -iy;
                ix %= bw;
                iy %= bh;
                int height = (qGray(bitmap.pixel(ix,iy)));
                float scale = 0.6;
                scale = scale * height;
                height = (int)(scale);
                if(height < 4) height = 4;
                if(height > 144) height = 144;
                for(unsigned short k = 0;k<height;k++)
                {
                    if(k<7)
                    {
                        chunkblocks[i][j][k].baseBlockId = 6;
                        chunkblocks[i][j][k].isCullEnabled = true;
                        continue;
                    }
                    if(k<32)
                    {
                        chunkblocks[i][j][k].baseBlockId = 4+qrand()%2;
                        chunkblocks[i][j][k].isCullEnabled = true;
                        continue;
                    }
                    if(k<64)
                    {
                        chunkblocks[i][j][k].baseBlockId = 2;
                        chunkblocks[i][j][k].isCullEnabled = true;
                        continue;
                    }
                    if(k<128)
                    {
                        chunkblocks[i][j][k].baseBlockId = qrand()%8+1;
                        chunkblocks[i][j][k].isCullEnabled = true;
                        continue;
                    }
                    if(k<=144)
                    {
                        chunkblocks[i][j][k].baseBlockId = 8;
                        chunkblocks[i][j][k].isCullEnabled = true;
                        continue;
                    }
                }
            }
        }
        */

        //try using a perlin noise
        for(unsigned short i=0;i<CHUNK_SIDE_SIZE;++i)
        {
            for(unsigned short j=0;j<CHUNK_SIDE_SIZE;++j)
            {
                for(unsigned short k=0;k<CHUNK_HEIGHT_SIZE;++k)
                {
                    BlockInstance::initDefaultValues(&(chunkblocks[i][j][k]));
                }

                float worldPos_x = (float)(this->sectorX  * CHUNK_SIDE_SIZE + j)/32.0f;
                float worldPos_y = (float)(this->sectorY * CHUNK_SIDE_SIZE + i)/32.0f;
                if(worldPos_x < 0) worldPos_x = -worldPos_x;
                if(worldPos_y < 0) worldPos_y = -worldPos_y;

                unsigned short height =(unsigned short)(world_gen.perlinNoise_2d_1(worldPos_x,worldPos_y) );

                if(height > CHUNK_HEIGHT_SIZE-1) height = CHUNK_HEIGHT_SIZE-1;

                for(unsigned short k = 0;k<height;k++)
                {
                    if(k<48)
                    {
                        chunkblocks[i][j][k].baseBlockId = 5;
                        chunkblocks[i][j][k].isCullEnabled = true;
                        continue;
                    }
                    if(k<128)
                    {
                        if(k == height-1)
                        {
                            //top grass
                            chunkblocks[i][j][k].baseBlockId = 2;
                            chunkblocks[i][j][k].isCullEnabled = true;
                            continue;
                        }
                        chunkblocks[i][j][k].baseBlockId = 1;
                        chunkblocks[i][j][k].isCullEnabled = true;
                        continue;
                    }
                    if(k<=CHUNK_HEIGHT_SIZE)
                    {
                        //default
                        chunkblocks[i][j][k].baseBlockId = qrand()%2+1;
                        chunkblocks[i][j][k].isCullEnabled = true;
                        continue;
                    }
                }
            }
        }


        this->isChunkLoaded = true;
        return true;
    }

    bool unloadChunkData()
    {
        //save data here...
        //
        //

        //after data is saved,the flag set to false to allow reload data of different chunks
        this->isChunkLoaded = false;
        return true;
    }

};

#endif // CHUNK_H
