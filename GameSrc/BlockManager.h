#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H

#endif // BLOCKMANAGER_H

#include "glbase.h"
#include "BlockBase.h"

#include <QFile>
#include <QString>
#include <QVector2D>
#include <QStringList>
#include <QOpenGLTexture>

class BlocksManager : protected GLBase
{
public:

    int blockCount;

    QVector<BlockBase *> blks;

    QVector<texMap> blkTexUVMap;

    QOpenGLTexture * blkTex;

public:

    BlocksManager() : GLBase()
    {
        this->checkAndInit();

        blockCount=0;

        blks.clear();

        blkTexUVMap.clear();

        blkTex = Q_NULLPTR;
    }

    ~BlocksManager()
    {
        //delete all blocks (base)
        for(int i=0;i<blks.count();i++)
        {
            if(blks[i] != Q_NULLPTR) delete blks[i];
        }

        if(blkTex != Q_NULLPTR)
        {
            delete blkTex;
        }
    }

    bool initAllBlocks()
    {

        //init texture packs
        if( !this->initBlockTextures() )
        {
            qDebug("Block textures init failed.");
            return false;
        }

        //read blocks definition from file or something
        QFile file(":/Blocks/Blocks.config");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qFatal("Open Block Config File Failed.");
        }
        else
        {
            qDebug("Block config File read success");
            while (!file.atEnd())
            {
                QByteArray linedata = file.readLine();
                QString line(linedata);
                line = line.trimmed();
                if(line.isEmpty()) continue;
                //skip comment lines
                if(line.trimmed().startsWith("#")) continue;
                //qDebug("%s",line.toStdString().c_str());

                //process line data
                QStringList list = line.split("\t");
                if(list.count() != 11) qFatal("BlockConfig file not readable.");

                //adding blocks
                BlockBase * newblk = new BlockBase();
                //void (air) block
                if(list.at(2) == QString("0"))
                {
                    newblk->blockId = list.at(0).toInt();
                    newblk->blockName = list.at(1);
                    newblk->typeId = list.at(2).toInt();
                    newblk->typeName = list.at(3);
                    //out
                    blks.push_back(newblk);
                    qDebug("Block  %s  added.",newblk->blockName.toStdString().c_str());
                    this->blockCount ++;
                }
                else                //normal blocks
                {
                    newblk->blockId = list.at(0).toInt();
                    newblk->blockName = list.at(1);
                    newblk->typeId = list.at(2).toInt();
                    newblk->typeName = list.at(3);
                    newblk->texId = list.at(4).toInt();
                    newblk->texIdUp = list.at(5).toInt();
                    newblk->texIdDown = list.at(6).toInt();
                    newblk->texIdLeft = list.at(7).toInt();
                    newblk->texIdRight = list.at(8).toInt();
                    newblk->texIdFront = list.at(9).toInt();
                    newblk->texIdBack = list.at(10).toInt();

                    //initBatchData
                    newblk->initBatchDataFromDefaultCube(this->blkTexUVMap);

                    //out
                    blks.push_back(newblk);
                    qDebug("Block  %s  added.",newblk->blockName.toStdString().c_str());
                    this->blockCount++;

                }

            }
        }

        //close file.
        file.close();
        return true;
    }

private:

    //initial texture atalas
    //called by initAllblocks
    bool initBlockTextures()
    {
        int smallTextureCount=0;

        int countPerLine = 0;

        blkTexUVMap.clear();

        QFile file(":/Blocks/Pack.config");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qFatal("Open Texture Config File Failed.");
        }
        else
        {
            qDebug("Texture config File read success");
            while (!file.atEnd())
            {
                QByteArray linedata = file.readLine();
                QString line(linedata);
                line = line.trimmed();
                if(line.isEmpty()) continue;
                //skip comment lines
                if(line.startsWith("#")) continue;


                QStringList list = line.split("\t");

                //get small texture count
                if(line.startsWith("@"))
                {
                    if(list.count() != 3) qFatal("TextureConfig file not readable.");
                    smallTextureCount = list.at(1).toInt();
                    countPerLine = list.at(2).toInt();
                    qDebug("Small texture count : %d\nTextures per line : %d",smallTextureCount,countPerLine);
                    continue;
                }

                if(list.count() != 3) qFatal("TextureConfig file not readable.");


                //process line data
                int pos = list.at(1).toInt();
                texMap newtexmap;
                newtexmap.textureId = list.at(0).toInt();
                newtexmap.textureName = list.at(2);
                if(pos != -1)
                {
                    newtexmap.deltaLength = 1.0f / (float)countPerLine;
                    newtexmap.startPosition = QVector2D(  (pos%4)*newtexmap.deltaLength , ((int)(pos/4))*newtexmap.deltaLength);
                }
                blkTexUVMap.push_back(newtexmap);
            }

            if(this->blkTex != NULL)
            {
                qWarning("blkTexture already initialized.");
                return false;
            }

            blkTex = new QOpenGLTexture(QImage(":/Blocks/Pack.png").mirrored());
            //blkTex->setMipLevelRange(0,4);
            //blkTex->generateMipMaps();
            blkTex->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
            blkTex->setMagnificationFilter(QOpenGLTexture::Nearest);
            blkTex->setWrapMode(QOpenGLTexture::ClampToEdge);
        }


        //close file.
        file.close();
        return true;
    }
};
