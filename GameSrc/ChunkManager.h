#ifndef CHUNKMANAGER_H
#define CHUNKMANAGER_H

#include "glframe.h"
#include "glbase.h"
#include "ChunkUpdateWorker.h"

#include <QObject>
#include <QThread>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>



class ChunkManager : public QObject,protected GLBase
{
    Q_OBJECT

public:

    QVector2D position;

    int chunkLoadRad,chunkUnloadRad;

    BlocksManager * blkmgr;

    QVector<CHUNKNODE> chunks;
    QVector<BATCHNODE> batches;

private:

    QOpenGLShaderProgram * shader;

    GLFrame * camera;

    QThread chunkUpdateThread;

    ChunkUpdateWorker * cworker;

    bool readyToDraw;

public:

    ChunkManager() : QObject(),GLBase()
    {
        this->checkAndInit();

        this->position = QVector2D(0,0);

        //default load and unload radius
        chunkLoadRad = 12;
        chunkUnloadRad = 12;

        this->blkmgr = Q_NULLPTR;

        chunks.clear();
        batches.clear();

        this->shader = Q_NULLPTR;
        this->camera = Q_NULLPTR;
        this->cworker = Q_NULLPTR;
        this->readyToDraw = false;
    }

    ~ChunkManager()
    {
        //stop thread
        cworker->terminateflag = true;
        chunkUpdateThread.quit();
        chunkUpdateThread.wait();

        if(blkmgr != Q_NULLPTR)
        {
            delete blkmgr;
        }

        //delete chunks
        for(int i=0;i<chunks.size();i++)
        {
            if(chunks[i].chunk != Q_NULLPTR)
            {
                chunks[i].chunk->unloadChunkData();
                delete chunks[i].chunk;
            }
        }
        //delete batches
        for(int i=0;i<batches.count();i++)
        {
            if(batches[i].batch != Q_NULLPTR)
            {
                delete batches[i].batch;
            }
        }
        //done
    }

    bool initBlockMgrAndUpdateWorker()
    {
        if(this->blkmgr != Q_NULLPTR)
        {
            qWarning("Block manager already loaded.");
            return false;
        }
        this->blkmgr = new BlocksManager();

        if( blkmgr->initAllBlocks() )
        {
            qDebug("BlockManager : All blocks initialized.");

            //init chunk update worker and move it to another thread

            this->cworker = new ChunkUpdateWorker();

            cworker->position = this->position;
            cworker->blkmgr = this->blkmgr;
            cworker->chunkLoadRad = this->chunkLoadRad;
            cworker->chunkUnloadRad = this->chunkUnloadRad;
            cworker->chunks = &chunks;

            cworker->setTexUnitsCount(1);
            cworker->moveToThread(&chunkUpdateThread);

            connect(&chunkUpdateThread,SIGNAL(finished()), cworker, SLOT(deleteLater()));
            connect(this, SIGNAL(chunksNeedUpdate()), cworker, SLOT(chunkLoadAndUnloadUpdate()));
            connect(cworker,SIGNAL(chunkBatchBuiltOneDone(int,int,int)),this,SLOT(updateChunkBatch(int,int,int)));
            connect(cworker,SIGNAL(chunkUnloaded(int)),this,SLOT(unloadChunkBatch(int)));

            chunkUpdateThread.start();

            return true;
        }
        else
        {
            qWarning("BlockManager : blocks init failed.");
            return false;
        }
    }

    void setPosition(QVector2D ppos)
    {
        if((ppos-position).length() > CHUNK_SIDE_SIZE * BLOCK_SIZE)
        {
            position = ppos;
            if(!cworker->updating)
            {
                this->cworker->position = ppos;
                emit chunksNeedUpdate();
            }
        }
    }

    void perpareForDraw(QOpenGLShaderProgram * s,GLFrame * cam)
    {
        this->shader = s;
        this->camera = cam;

        if(shader != Q_NULLPTR && camera != Q_NULLPTR)
        {
            this->readyToDraw = true;
        }
        else
        {
            this->readyToDraw = false;
        }
    }

    void drawAllChunks()
    {

        if(!this->readyToDraw)
        {
            qFatal("ChunkManager is not ready to draw");
            return;
        }
        QMatrix4x4 v,p;
        camera->getVMatrix(v);
        camera->getPMatrix(p);

        //bind block texture
        blkmgr->blkTex->bind();
        shader->bind();
        shader->setUniformValue("mvpMatrix",p*v);
        shader->setUniformValue("mvMatrix",v);
        shader->setUniformValue("normalMatrix",v.normalMatrix());

        for(int i=0;i<batches.count();i++)
        {
            if(batches.at(i).batch != Q_NULLPTR && batches.at(i).batchDone)
            {
                batches.at(i).batch->draw();
            }
        }
    }

signals:

    void chunksNeedUpdate();

public slots:

    void updateChunkBatch(int index,int count,int bufId)
    {
        //if the chunk is not loaded,return
        if(!chunks[index].loaded)
        {
            qDebug("Can not update batch when chunkbatch is not load.");
            cworker->buf[bufId].readyforfetch = false;
            return;
        }
        //if no data to fetch ,return
        if(!cworker->buf[bufId].readyforfetch)
        {
            qDebug("no chunk batch data to update");
            cworker->buf[bufId].readyforfetch = false;
            return;
        }

        //if batchnode havn't been added to the vector,then add
        int numsp =  (index+1) * CHUNK_BATCH_DEVIDE_SIZE;
        if(batches.count() < numsp)
        {
            unsigned int goal = numsp - batches.count();
            for(unsigned int i=0;i<goal;++i)
            {
                BATCHNODE newbnode;
                newbnode.batchDone = false;
                newbnode.batch = new GLBatch();
                batches.push_back(newbnode);
            }
        }

        //find the batch
        unsigned int dest = index * CHUNK_BATCH_DEVIDE_SIZE + count;

        batches[dest].batch->Begin(GL_TRIANGLES,cworker->buf[bufId].tvdata.count()/3,1);
        batches[dest].batch->CopyVertexDataBlock3f(cworker->buf[bufId].tvdata.data());
        batches[dest].batch->CopyNormalDataBlock3f(cworker->buf[bufId].tndata.data());
        batches[dest].batch->CopyTexCoordDataBlock2f(cworker->buf[bufId].ttdata[0].data(),0);
        batches[dest].batch->End();
        batches[dest].batchDone = true;
        cworker->buf[bufId].readyforfetch = false;
    }

    void unloadChunkBatch(int index)
    {
        int dest = (index+1) * CHUNK_BATCH_DEVIDE_SIZE;
        if(dest<0 || dest > batches.count()) return;
        for(int i=index * CHUNK_BATCH_DEVIDE_SIZE;i<dest;i++)
        {
            batches[i].batchDone = false;
        }
    }
};



#endif // CHUNKMANAGER_H
