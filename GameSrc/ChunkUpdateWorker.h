#ifndef CHUNKUPDATEWORKER_H
#define CHUNKUPDATEWORKER_H

#include "glbatch.h"
#include "Chunk.h"

#include <cmath>
#include <QPoint>
#include <QObject>
#include <QDebug>
#include "QQueue"


#define MAX_CHUNK_LOADNUM_LIMIT 2048
#define BATCH_TRANS_BUF_SIZE 16

typedef struct
{
    Chunk * chunk;
    volatile bool loaded;
}
CHUNKNODE;

typedef struct
{
    GLBatch * batch;
    volatile bool batchDone;
}
BATCHNODE;

typedef struct tempstr
{
    QVector<float> tvdata;
    QVector<float> tndata;
    QVector<float> * ttdata;
    volatile bool readyforfetch;
}
TPSTR;



class ChunkUpdateWorker : public QObject
{
    Q_OBJECT

public:

    QVector2D position;

    int chunkLoadRad,chunkUnloadRad;

    BlocksManager * blkmgr;

    QVector<CHUNKNODE> * chunks;

    int nTexUnits;

    volatile bool terminateflag;

    volatile bool updating;

public:

    TPSTR buf[BATCH_TRANS_BUF_SIZE];

private:

    QQueue<int> changeList;

public:

    ChunkUpdateWorker() : QObject()
    {
        this->blkmgr = Q_NULLPTR;

        this->chunks = Q_NULLPTR;

        nTexUnits = 0;

        terminateflag = false;

        for(int i=0;i<BATCH_TRANS_BUF_SIZE;i++)
        {
            buf[i].ttdata = Q_NULLPTR;
            buf[i].readyforfetch = false;
        }

        updating =false;
    }


    ~ChunkUpdateWorker()
    {
        for(int i=0;i<BATCH_TRANS_BUF_SIZE;i++)
        {
            if(buf[i].ttdata != Q_NULLPTR)
            {
                //delete buf[i].ttdata;
            }
        }
    }


    void setTexUnitsCount(int count)
    {
        this->nTexUnits = count;

        if(count == 0 )
        {
            return;
        }

        for(int i=0;i<BATCH_TRANS_BUF_SIZE;i++)
        {
            if (buf[i].ttdata != Q_NULLPTR)
            {
                delete buf[i].ttdata;
            }
            buf[i].ttdata = new QVector<float>[count];
        }

    }

public slots:

    void chunkLoadAndUnloadUpdate()
    {
        this->updating = true;

        bool firstload = false;
        if(chunks->count() == 0) firstload = true;

        QPoint curPos = Chunk::getChunkSector(this->position);
        QPoint ru,ld;

        //unload old chunk
        if(!firstload)
        {
            ru = curPos + QPoint(chunkUnloadRad,chunkUnloadRad);
            ld = curPos + QPoint(-chunkUnloadRad,-chunkUnloadRad);
            for(int k=0;k<chunks->count();k++)
            {
                if(chunks->at(k).loaded && (chunks->at(k).chunk->sectorX > ru.x() || chunks->at(k).chunk->sectorX < ld.x() || chunks->at(k).chunk->sectorY > ru.y() || chunks->at(k).chunk->sectorY < ld.y()))
                {
                    this->unloadChunk(k);
                }
            }
        }

        //load new chunk
        ru = curPos + QPoint(chunkLoadRad,chunkLoadRad);
        ld = curPos + QPoint(-chunkLoadRad,-chunkLoadRad);
        int i,j,xx,yy,hold = chunkLoadRad*chunkLoadRad,index;

        //load self
        index = this->loadChunkBySector(curPos.x(),curPos.y());
        if(!firstload && index!=-1)
        {
            changeList.enqueue(index);
        }

        for(int kpass = 1;kpass <= chunkLoadRad;kpass++)
        {
            i = curPos.x()-kpass;
            j = curPos.y()-kpass;
            for(;j<curPos.y()+kpass;j++)
            {
                //if too far ( out of a circle area) then don't load
                xx = abs(curPos.x()-i);
                yy =  abs(curPos.y()-j);
                if(xx*xx + yy*yy > hold) continue;
                index = this->loadChunkBySector(i,j);
                if(!firstload && index!=-1)
                {
                    changeList.enqueue(index);
                }
            }
            for(;i<curPos.x()+kpass;i++)
            {
                //if too far ( out of a circle area) then don't load
                xx = abs(curPos.x()-i);
                yy =  abs(curPos.y()-j);
                if(xx*xx + yy*yy > hold) continue;
                index = this->loadChunkBySector(i,j);
                if(!firstload && index!=-1)
                {
                    changeList.enqueue(index);
                }
            }
            for(;j>curPos.y()-kpass;j--)
            {
                //if too far ( out of a circle area) then don't load
                xx = abs(curPos.x()-i);
                yy =  abs(curPos.y()-j);
                if(xx*xx + yy*yy > hold) continue;
                index = this->loadChunkBySector(i,j);
                if(!firstload && index!=-1)
                {
                    changeList.enqueue(index);
                }
            }
            for(;i>curPos.x()-kpass;i--)
            {
                //if too far ( out of a circle area) then don't load
                xx = abs(curPos.x()-i);
                yy =  abs(curPos.y()-j);
                if(xx*xx + yy*yy > hold) continue;
                index = this->loadChunkBySector(i,j);
                if(!firstload && index!=-1)
                {
                    changeList.enqueue(index);
                }
            }
        }

        //if first load,build all chunks batch,no need to unload
        if(firstload)
        {
            for(int i=0;i<chunks->count();i++)
            {
                for(int j=0;j<CHUNK_BATCH_DEVIDE_SIZE;j++)
                {
                    this->buildChunkBatch(i,j);
                }
            }
        }
        else
        {
            //deal with the change list
            this->dealLoadList();
        }

        this->updating = false;
    }

    void buildChunkBatch(int index,int count)
    {

        int idel = 0;
        for(int i=0;i<BATCH_TRANS_BUF_SIZE && !terminateflag;)
        {
            if(!buf[i].readyforfetch)
            {
                idel = i;
                break;
            }
            else
            {
                i++;
                i %= BATCH_TRANS_BUF_SIZE;
            }
        }

        QVector<float> * tvdata;
        QVector<float> * tndata;
        QVector<float> * ttdata;
        tvdata = &(buf[idel].tvdata);
        tndata = &(buf[idel].tndata);
        ttdata = buf[idel].ttdata;

        //clear the arrays
        tvdata->clear();
        tndata->clear();
        for(int i=0;i<nTexUnits;i++)
        {
            ttdata[i].clear();
        }


        CHUNKNODE * chunkbatch = &((*chunks)[index]);

        //check chunks neighborhood
        short int neigh[4] = {-1,-1,-1,-1};//north,south,west,east
        for(int i=0;i<chunks->count();++i)
        {
            if(chunks->at(i).chunk->sectorX == chunkbatch->chunk->sectorX && chunks->at(i).chunk->sectorY == chunkbatch->chunk->sectorY+1) neigh[0] = i;
            if(chunks->at(i).chunk->sectorX == chunkbatch->chunk->sectorX && chunks->at(i).chunk->sectorY == chunkbatch->chunk->sectorY-1) neigh[1] = i;
            if(chunks->at(i).chunk->sectorX == chunkbatch->chunk->sectorX-1 && chunks->at(i).chunk->sectorY == chunkbatch->chunk->sectorY) neigh[2] = i;
            if(chunks->at(i).chunk->sectorX == chunkbatch->chunk->sectorX+1 && chunks->at(i).chunk->sectorY == chunkbatch->chunk->sectorY) neigh[3] = i;
        }


        unsigned char vis = 0;
        int heightstart = count * (CHUNK_HEIGHT_SIZE / CHUNK_BATCH_DEVIDE_SIZE);
        int heightend = (count+1) * (CHUNK_HEIGHT_SIZE / CHUNK_BATCH_DEVIDE_SIZE);
        //prase all blockinstance
        for(unsigned short i=0;i<CHUNK_SIDE_SIZE;++i)
        {
            for(unsigned short j=0;j<CHUNK_SIDE_SIZE;++j)
            {
                for(unsigned short k=heightstart ;k<heightend;++k)
                {
                    Chunk * chk = chunkbatch->chunk;
                    BlockInstance * ins = &(chunkbatch->chunk->chunkblocks[i][j][k]);
                    BlockDataBatch * ba = blkmgr->blks.at( ins->baseBlockId )->bdata;
                    QVector3D offset = QVector3D(BLOCK_SIZE * j,BLOCK_SIZE * i,BLOCK_SIZE * k)+QVector3D(chunks->at(index).chunk->pos,0);

                    //if is air or not visiable ,continue
                    if(ins->baseBlockId == 0) continue;

                    //calculate block face visiability
                    if(!ins->isCullEnabled)
                    {
                        //get vertex and normal data
                        ba->getAllVNData(*tvdata,*tndata,offset);
                        //get texture coords data of each tex channal
                        for(int tt=0;tt<nTexUnits;tt++)
                        {
                            ba->getTData(ttdata[tt],tt);
                        }
                    }
                    else
                    {

                        vis=0;
                        if(k==CHUNK_HEIGHT_SIZE-1 || chk->chunkblocks[i][j][k+1].baseBlockId ==0)
                        {
                            vis = vis | (1<<0);
                            ba->getDefaultCubeFaceVNData(FACE_UP,*tvdata,*tndata,offset);
                            for(int ttu = 0;ttu < ba->nTexCoordsUnits;ttu++)
                            {
                                ba->getDefaultCubeFaceTData(FACE_UP,ttdata[ttu],ttu);
                            }
                        }
                        if(k!=0 && chk->chunkblocks[i][j][k-1].baseBlockId ==0)
                        {
                            vis = vis | (1<<1);
                            ba->getDefaultCubeFaceVNData(FACE_DOWN,*tvdata,*tndata,offset);
                            for(int ttu = 0;ttu < ba->nTexCoordsUnits;ttu++)
                            {
                                ba->getDefaultCubeFaceTData(FACE_DOWN,ttdata[ttu],ttu);
                            }
                        }
                        if( (j != 0 && chk->chunkblocks[i][j-1][k].baseBlockId ==0) || (j==0 && neigh[2]!=-1 && chunks->at(neigh[2]).chunk->chunkblocks[i][CHUNK_SIDE_SIZE-1][k].baseBlockId == 0) )
                        {
                            vis = vis | (1<<2);
                            ba->getDefaultCubeFaceVNData(FACE_LEFT,*tvdata,*tndata,offset);
                            for(int ttu = 0;ttu < ba->nTexCoordsUnits;ttu++)
                            {
                                ba->getDefaultCubeFaceTData(FACE_LEFT,ttdata[ttu],ttu);
                            }
                        }
                        if( (j != CHUNK_SIDE_SIZE-1 && chk->chunkblocks[i][j+1][k].baseBlockId ==0) || (j == CHUNK_SIDE_SIZE-1 && neigh[3]!=-1 && chunks->at(neigh[3]).chunk->chunkblocks[i][0][k].baseBlockId == 0))
                        {
                            vis = vis | (1<<3);
                            ba->getDefaultCubeFaceVNData(FACE_RIGHT,*tvdata,*tndata,offset);
                            for(int ttu = 0;ttu < ba->nTexCoordsUnits;ttu++)
                            {
                                ba->getDefaultCubeFaceTData(FACE_RIGHT,ttdata[ttu],ttu);
                            }
                        }
                        if((i != CHUNK_SIDE_SIZE-1 && chk->chunkblocks[i+1][j][k].baseBlockId ==0) || (i == CHUNK_SIDE_SIZE-1 && neigh[0]!=-1 && chunks->at(neigh[0]).chunk->chunkblocks[0][j][k].baseBlockId == 0))
                        {
                            vis = vis | (1<<4);
                            ba->getDefaultCubeFaceVNData(FACE_FRONT,*tvdata,*tndata,offset);
                            for(int ttu = 0;ttu < ba->nTexCoordsUnits;ttu++)
                            {
                                ba->getDefaultCubeFaceTData(FACE_FRONT,ttdata[ttu],ttu);
                            }
                        }
                        if( (i != 0 && chk->chunkblocks[i-1][j][k].baseBlockId ==0) || (i==0 && neigh[1]!=-1&& chunks->at(neigh[1]).chunk->chunkblocks[CHUNK_SIDE_SIZE-1][j][k].baseBlockId == 0))
                        {
                            vis = vis | (1<<5);
                            ba->getDefaultCubeFaceVNData(FACE_BACK,*tvdata,*tndata,offset);
                            for(int ttu = 0;ttu < ba->nTexCoordsUnits;ttu++)
                            {
                                ba->getDefaultCubeFaceTData(FACE_BACK,ttdata[ttu],ttu);
                            }
                        }
                        ins->nvisible = vis;
                    }
                }
            }
        }

        if(tvdata->count() == 0)
        {
            return;
        }

        buf[idel].readyforfetch = true;

        emit chunkBatchBuiltOneDone(index,count,idel);

        return;
    }

signals:

    void chunkBatchBuiltOneDone(int index,int count,int bufId);

    void chunkUnloaded(int index);

private:

    int loadChunkBySector(int secx,int secy)
    {
        if(blkmgr == Q_NULLPTR)
        {
            qFatal("Can not load chunk before block manager initialized.Check call sequence.");
        }

        //find if already loaded and find which chunk is unloaded and can be reused.
        int unloadedChunk = -1;
        for(int k=0;k<chunks->count();k++)
        {
            if(chunks->at(k).chunk->sectorX == secx && chunks->at(k).chunk->sectorY == secy)
            {
                if(chunks->at(k).loaded)
                {
                    //already loaded,return -1
                    return -1;
                }
                else
                {
                    //not loaded,but data yet left
                    (*chunks)[k].loaded = true;
                    return k;
                }

            }
            if( !chunks->at(k).loaded)
            {
                unloadedChunk = k;
            }
        }

        //if there is a chunk than can be reused,then no need to new a new one
        if(unloadedChunk != -1)
        {
            chunks->at(unloadedChunk).chunk->setChunkSector(secx,secy);
            chunks->at(unloadedChunk).chunk->loadChunkData();
            (*chunks)[unloadedChunk].loaded = true;

            return unloadedChunk;
        }
        else
        {
            if(chunks->count() > MAX_CHUNK_LOADNUM_LIMIT)
            {
                qFatal("Max Chunk load limit reached.");
                return -1;
            }

            qDebug("New chunkbatch created !");
            //no current chunks in vector that can be used
            CHUNKNODE newchunk;
            newchunk.chunk = new Chunk(blkmgr);
            newchunk.chunk->setChunkSector(secx,secy);
            newchunk.chunk->loadChunkData();
            newchunk.loaded = true;
            chunks->push_back(newchunk);

            return chunks->count()-1;
        }
    }

    void unloadChunk(int index)
    {
        chunks->at(index).chunk->unloadChunkData();
        (*chunks)[index].loaded = false;
        qDebug("Chunk %d %d unloaded.",chunks->at(index).chunk->sectorX,chunks->at(index).chunk->sectorY);
        emit chunkUnloaded(index);
    }

    void dealLoadList()
    {
        int index,secx,secy,cx,cy;
        unsigned char bitmap[MAX_CHUNK_LOADNUM_LIMIT/8+1] = {0};
        while(!this->changeList.isEmpty())
        {
            index = changeList.dequeue();

            //load self
            for(int i=0;i<CHUNK_BATCH_DEVIDE_SIZE;i++)
            {
                this->buildChunkBatch(index,i);
            }
            bitmap[index/8] = bitmap[index/8] | (1<<(index%8));


            //find neighbor
            secx = chunks->at(index).chunk->sectorX;
            secy = chunks->at(index).chunk->sectorY;

            for(int i=0;i<chunks->count();i++)
            {
                if(!chunks->at(i).loaded) continue;
                cx = chunks->at(i).chunk->sectorX;
                cy = chunks->at(i).chunk->sectorY;
                if(cx ==secx+1 && cy == secy && !(bitmap[i/8] & (1<<(i%8))))
                {
                    for(int j=0;j<CHUNK_BATCH_DEVIDE_SIZE;j++)
                    {
                        this->buildChunkBatch(i,j);
                    }
                    bitmap[i/8] = bitmap[i/8] | (1<<(i%8));
                    continue;
                }
                if(cx ==secx-1 && cy == secy && !(bitmap[i/8] & (1<<(i%8))))
                {
                    for(int j=0;j<CHUNK_BATCH_DEVIDE_SIZE;j++)
                    {
                        this->buildChunkBatch(i,j);
                    }
                    bitmap[i/8] = bitmap[i/8] | (1<<(i%8));
                    continue;
                }
                if(cx ==secx && cy == secy+1 && !(bitmap[i/8] & (1<<(i%8))))
                {
                    for(int j=0;j<CHUNK_BATCH_DEVIDE_SIZE;j++)
                    {
                        this->buildChunkBatch(i,j);
                    }
                    bitmap[i/8] = bitmap[i/8] | (1<<(i%8));
                    continue;
                }
                if(cx ==secx && cy == secy-1 && !(bitmap[i/8] & (1<<(i%8))))
                {
                    for(int j=0;j<CHUNK_BATCH_DEVIDE_SIZE;j++)
                    {
                        this->buildChunkBatch(i,j);
                    }
                    bitmap[i/8] = bitmap[i/8] | (1<<(i%8));
                    continue;
                }
            }
        }
    }
};



#endif // CHUNKUPDATEWORKER_H
