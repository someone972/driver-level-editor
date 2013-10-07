#include "world.hpp"

SectorTextureList::SectorTextureList()
{
    numTexturesUsed = 0;
    memset(textures,0xff,70);
};

void SectorTextureList::cleanup()
{
    numTexturesUsed = 0;
    memset(textures,0xff,70);
};

int SectorTextureList::getNumTexturesUsed()
{
    return numTexturesUsed;
};

int SectorTextureList::getTexture(int idx)
{
    if(idx >= 0 && idx < numTexturesUsed)
    return textures[idx];
    return 0;
};

void SectorTextureList::setTexture(int idx,int tex)
{
    if(idx >= 0 && idx < numTexturesUsed)
    textures[idx] = tex;
};

void SectorTextureList::removeTexture(int idx)
{
    if(idx >= 0 && idx < numTexturesUsed)
    {
        memmove(&textures[idx],&textures[idx+1],(numTexturesUsed-idx-1)*sizeof(unsigned char));
        numTexturesUsed--;
        textures[numTexturesUsed] = 0xff;
    }
};

void SectorTextureList::addTexture(int tex)
{
    if(numTexturesUsed < 64)
    {
        textures[numTexturesUsed] = tex;
        numTexturesUsed++;
        textures[numTexturesUsed] = 0xff;
    }
};

int SectorTextureList::load(IOHandle handle, IOCallbacks* callbacks)
{
    cleanup();

    if(!handle || !callbacks)
    return 1;

    callbacks->read(textures,70,1,handle);
    for(int i = 0; i < 70; i++)
    {
        if(textures[i] == 0xff)
        {
            numTexturesUsed = i;
            break;
        }
    }
    return 0;
};

unsigned int SectorTextureList::getRequiredSize()
{
    return 70;
};

int SectorTextureList::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(textures,70,1,handle);
    return 0;
};

SectorTextureUsage::SectorTextureUsage()
{

};

SectorTextureUsage::~SectorTextureUsage()
{

};

void SectorTextureUsage::cleanup()
{
    for(int i = 0; i < 1024; i++)
    textureLists[i].cleanup();
};

SectorTextureList* SectorTextureUsage::getTextureList(int sectorIdx)
{
    if(sectorIdx >= 0 && sectorIdx < 1024)
    return &textureLists[sectorIdx];
    return NULL;
};

int SectorTextureUsage::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    char buffer[1024];

    cleanup();

    if(!handle || !callbacks)
    return 1;

    if(size < 1024*70)
    {
        log->Log("ERROR: Required size for sector texture usage exceeds block size!");
        return 2;
    }

    log->increaseIndent();
    for(int i = 0; i < 1024; i++)
    {
        textureLists[i].load(handle,callbacks);

        if(log->getLogPriority() >= DEBUG_LEVEL_RIDICULOUS)
        {
            memset(buffer, 0, 1024);
            for(int j = 0; j < textureLists[i].getNumTexturesUsed(); j++)
            {
                sprintf(buffer+strlen(buffer), " %d", textureLists[i].getTexture(j));
            }
            log->Log(DEBUG_LEVEL_RIDICULOUS, "%d:%s", i, buffer);
        }
    }
    log->decreaseIndent();

    return 0;
};

unsigned int SectorTextureUsage::getRequiredSize()
{
    unsigned int size = 0;

    for(int i = 0; i < 1024; i++)
    size += textureLists[i].getRequiredSize();

    return size;
};

int SectorTextureUsage::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    for(int i = 0; i < 1024; i++)
    textureLists[i].save(handle,callbacks);

    return 0;
};

WorldModelDef::WorldModelDef()
{
    modelNum = 0;
    rot = 0.0f;
};

int WorldModelDef::getModelIndex()
{
    return modelNum;
};

void WorldModelDef::setModelIndex(int idx)
{
    if(idx >= 0)
    modelNum = idx;
    else modelNum = 0;
};

WorldSector::WorldSector()
{
    numModelDefs = 0;
    modelDefs = NULL;
    numContentsEntries = 0;
    contentsEntries = NULL;
    contentsTableDataSize = 0;
    contentsTableData = NULL;
};

WorldSector::~WorldSector()
{
    cleanup();
};

void WorldSector::cleanup()
{
    if(modelDefs)
    delete[] modelDefs;
    modelDefs = NULL;
    numModelDefs = 0;

    if(contentsEntries)
    delete[] contentsEntries;
    contentsEntries = NULL;
    numContentsEntries = 0;

    if(contentsTableData)
    delete[] contentsTableData;
    contentsTableData = NULL;
    contentsTableDataSize = 0;
};

int WorldSector::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();
    if(!handle || !callbacks)
    return -1;

    if(size < 12)
    {
        log->Log("ERROR: Size required for world sector header exceeds size of block.");
        return -2;
    }
    size -= 12;

    callbacks->read(&numContentsEntries,4,1,handle);
    log->Log(DEBUG_LEVEL_DEBUG, "numContentsEntries: %d", numContentsEntries);
    if(size < numContentsEntries*4)
    {
        log->Log("ERROR: Size required for contentsEntries exceeds size of block.");
        cleanup();
        return -2;
    }
    size -= numContentsEntries*4;
    contentsEntries = new unsigned int[numContentsEntries];
    callbacks->read(contentsEntries,4,numContentsEntries,handle);

    callbacks->read(&contentsTableDataSize,4,1,handle);
    log->Log(DEBUG_LEVEL_DEBUG, "contentsTableDataSize: %d",contentsTableDataSize);
    if(size < contentsTableDataSize)
    {
        log->Log("ERROR: Size required for contentsTableData exceeds size of block.");
        cleanup();
        return -2;
    }
    size -= contentsTableDataSize;
    contentsTableData = new unsigned char[contentsTableDataSize];
    callbacks->read(contentsTableData,1,contentsTableDataSize,handle);

    callbacks->read(&numModelDefs,4,1,handle);
    log->Log(DEBUG_LEVEL_DEBUG, "numModelDefs: %d", numModelDefs);
    if(size < numModelDefs*20)
    {
        log->Log("ERROR: Size required for modelDefs exceeds size of block.");
        cleanup();
        return -2;
    }
    size -= numModelDefs*20;
    modelDefs = new WorldModelDef[numModelDefs];

    log->increaseIndent();
    for(int i = 0; i < numModelDefs; i++)
    {
        unsigned short temp;
        callbacks->read(&modelDefs[i].modelNum,2,1,handle);
        callbacks->read(&temp,2,1,handle);
        callbacks->read(&modelDefs[i].position.x,4,1,handle);
        callbacks->read(&modelDefs[i].position.y,4,1,handle);
        callbacks->read(&modelDefs[i].position.z,4,1,handle);
        callbacks->read(&modelDefs[i].rot,2,1,handle);
        callbacks->seek(handle,2,SEEK_CUR);//padding
        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: modelNum: %hd padding?: %hu position: (%f, %f, %f) rotation: %hd",
                 i, modelDefs[i].modelNum, temp, modelDefs[i].position.x, modelDefs[i].position.y, modelDefs[i].position.z, modelDefs[i].rot);
    }
    log->decreaseIndent();
    return 12+4*numContentsEntries+contentsTableDataSize+numModelDefs*20;
};

unsigned int WorldSector::getRequiredSize()
{
    return 12+4*numContentsEntries+contentsTableDataSize+numModelDefs*20;
};

int WorldSector::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&numContentsEntries,4,1,handle);
    callbacks->write(contentsEntries,4,numContentsEntries,handle);

    callbacks->write(&contentsTableDataSize,4,1,handle);
    callbacks->write(contentsTableData,1,contentsTableDataSize,handle);

    callbacks->write(&numModelDefs,4,1,handle);

    short padding = 0;

    for(int i = 0; i < numModelDefs; i++)
    {
        callbacks->write(&modelDefs[i].modelNum,2,1,handle);
        callbacks->write(&padding,2,1,handle);
        callbacks->write(&modelDefs[i].position.x,4,1,handle);
        callbacks->write(&modelDefs[i].position.y,4,1,handle);
        callbacks->write(&modelDefs[i].position.z,4,1,handle);
        callbacks->write(&modelDefs[i].rot,2,1,handle);
        callbacks->write(&padding,2,1,handle);
    }
    return 0;
};

int WorldSector::getNumModelDefs()
{
    return numModelDefs;
};

WorldModelDef* WorldSector::getModelDef(int idx)
{
    if(idx >= 0 && idx < numModelDefs)
    return &modelDefs[idx];
    return NULL;
};

WorldHeader::WorldHeader()
{
    mapWidth = 0;
    mapHeight = 0;
    tileSize = 0;
    numSectors = 0;
    visTableWidth = 0;
    unk1 = 0;
    unk2 = 0;
    ambientLight = 0.0f;
};

DriverWorld::DriverWorld()
{
    numBridgedDefs = 0;
    bridgedDefs = NULL;
    sectors = NULL;
};

DriverWorld::~DriverWorld()
{
    cleanup();
};

void DriverWorld::cleanup()
{
    if(bridgedDefs)
    delete[] bridgedDefs;
    bridgedDefs = NULL;
    numBridgedDefs = 0;

    if(sectors)
    delete[] sectors;
    sectors = NULL;
    header.numSectors = 0;
};

int DriverWorld::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();
    if(!handle || !callbacks)
    return 1;

    if(size < 48)
    {
        log->Log("ERROR: Size required for world header exceeds size of block!");
        return 2;
    }

    callbacks->read(&header.mapWidth,4,1,handle);
    callbacks->read(&header.mapHeight,4,1,handle);
    callbacks->read(&header.tileSize,4,1,handle);
    callbacks->read(&header.numSectors,4,1,handle);
    callbacks->read(&header.visTableWidth,4,1,handle);
    callbacks->read(&header.unk1,4,1,handle);
    callbacks->read(&header.unk2,4,1,handle);
    callbacks->read(&header.ambientLight,4,1,handle);
    callbacks->read(&header.sunDirection.x,4,1,handle);
    callbacks->read(&header.sunDirection.y,4,1,handle);
    callbacks->read(&header.sunDirection.z,4,1,handle);

    callbacks->read(&numBridgedDefs,4,1,handle);

    log->Log(DEBUG_LEVEL_VERBOSE,"mapWidth: %d mapHeight: %d tileSize: %d numSectors: %d visTableWidth: %d unk1: %d unk2: %d ambientLight: %d sunDirection: (%f, %f, %f)",
             header.mapWidth, header.mapHeight, header.tileSize, header.numSectors, header.visTableWidth, header.unk1, header.unk2, header.ambientLight,
             header.sunDirection.x, header.sunDirection.y, header.sunDirection.z);

    size -= 48;

    if(size < numBridgedDefs*20 + 12*header.numSectors)
    {
        log->Log("ERROR: Required size for world exceeds size of block!");
        cleanup();
        return 2;
    }

    sectors = new WorldSector[header.numSectors];
    bridgedDefs = new WorldModelDef[numBridgedDefs];

    log->Log(DEBUG_LEVEL_VERBOSE,"Loading bridged definitions...");
    log->increaseIndent();
    for(int i = 0; i < numBridgedDefs; i++)
    {
        unsigned short temp;
        callbacks->read(&temp,2,1,handle);//messup?
        callbacks->read(&bridgedDefs[i].modelNum,2,1,handle);
        callbacks->read(&bridgedDefs[i].position.x,4,1,handle);
        callbacks->read(&bridgedDefs[i].position.y,4,1,handle);
        callbacks->read(&bridgedDefs[i].position.z,4,1,handle);
        callbacks->read(&bridgedDefs[i].rot,2,1,handle);
        callbacks->seek(handle,2,SEEK_CUR);//padding
        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: messup?: %hu modelNum: %hd position: (%f, %f, %f) rotation: %hd",
                 i, temp, bridgedDefs[i].modelNum, bridgedDefs[i].position.x, bridgedDefs[i].position.y, bridgedDefs[i].position.z, bridgedDefs[i].rot);
    }
    log->decreaseIndent();

    log->Log(DEBUG_LEVEL_VERBOSE,"Loading world sectors...");
    log->increaseIndent();
    for(int i = 0; i < header.numSectors; i++)
    {
        log->Log(DEBUG_LEVEL_DEBUG, "Sector %d:",i);
        log->increaseIndent();
        int ret = sectors[i].load(handle, callbacks, size, log);
        log->decreaseIndent();

        size -= ret;
        if(ret < 0)
        {
            log->Log("ERROR: Failed to load sector, aborting.");
            cleanup();
            return 2;
        }
        if(size < 0)
        {
            log->Log("ERROR: Size required for world sectors exceeds size of block!");
            cleanup();
            return 2;
        }
    }
    log->decreaseIndent();
    return 0;
};

unsigned int DriverWorld::getRequiredSize()
{
    unsigned int size = 0;
    for(int i = 0; i < header.numSectors; i++)
    {
        size += sectors[i].getRequiredSize();
    }
    return 48+numBridgedDefs*20+size;
};

int DriverWorld::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&header.mapWidth,4,1,handle);
    callbacks->write(&header.mapHeight,4,1,handle);
    callbacks->write(&header.tileSize,4,1,handle);
    callbacks->write(&header.numSectors,4,1,handle);
    callbacks->write(&header.visTableWidth,4,1,handle);
    callbacks->write(&header.unk1,4,1,handle);
    callbacks->write(&header.unk2,4,1,handle);
    callbacks->write(&header.ambientLight,4,1,handle);
    callbacks->write(&header.sunDirection.x,4,1,handle);
    callbacks->write(&header.sunDirection.y,4,1,handle);
    callbacks->write(&header.sunDirection.z,4,1,handle);

    callbacks->write(&numBridgedDefs,4,1,handle);

    short padding = 0;

    for(int i = 0; i < numBridgedDefs; i++)
    {
        callbacks->write(&padding,2,1,handle);
        callbacks->write(&bridgedDefs[i].modelNum,2,1,handle);
        callbacks->write(&bridgedDefs[i].position.x,4,1,handle);
        callbacks->write(&bridgedDefs[i].position.y,4,1,handle);
        callbacks->write(&bridgedDefs[i].position.z,4,1,handle);
        callbacks->write(&bridgedDefs[i].rot,2,1,handle);
        callbacks->write(&padding,2,1,handle);
    }

    for(int i = 0; i < header.numSectors; i++)
    {
        sectors[i].save(handle,callbacks);
    }
    return 0;
};

int DriverWorld::getNumBridgedDefs()
{
    return numBridgedDefs;
};

WorldModelDef* DriverWorld::getBridgedDef(int idx)
{
    if(idx >= 0 && idx < numBridgedDefs)
    return &bridgedDefs[idx];
    return NULL;
};

int DriverWorld::getNumSectors()
{
    return header.numSectors;
};

WorldSector* DriverWorld::getSector(int idx)
{
    if(idx >= 0 && idx < header.numSectors)
    return &sectors[idx];
    return NULL;
};

SectorVisibility::SectorVisibility()
{
    visibilityDataSize = 0;
    visibilityData = NULL;
};

SectorVisibility::~SectorVisibility()
{
    cleanup();
};

void SectorVisibility::cleanup()
{
    if(visibilityData)
    delete[] visibilityData;
    visibilityData = NULL;
    visibilityDataSize = 0;
};

int SectorVisibility::load(IOHandle handle, IOCallbacks* callbacks,int size)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;

    visibilityDataSize = size;
    visibilityData = new unsigned char[visibilityDataSize];
    callbacks->read(visibilityData,1,size,handle);
    return 0;
};

void SectorVisibility::createEmptyData()
{
    cleanup();

    visibilityDataSize = 14400;
    visibilityData = new unsigned char[14400];
};

unsigned int SectorVisibility::getRequiredSize()
{
    return visibilityDataSize;
};

int SectorVisibility::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(visibilityData,1,visibilityDataSize,handle);
    return 0;
};

LevelVisibility::LevelVisibility()
{
    unk1 = 0;
    unk2 = 0;
    unk3 = 0;
    unk4 = 0;
    numSectors = 0;
    numSectorsX = 0;
    numSectorsZ = 0;
    sectors = NULL;
};

LevelVisibility::~LevelVisibility()
{
    cleanup();
};

void LevelVisibility::cleanup()
{
    if(sectors)
    delete[] sectors;
    sectors = NULL;
    numSectors = 0;
};

int LevelVisibility::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;

    callbacks->read(&unk1,4,1,handle);
    callbacks->read(&numSectorsX,2,1,handle);
    callbacks->read(&numSectorsZ,2,1,handle);
    callbacks->read(&unk2,2,1,handle);
    callbacks->read(&unk3,2,1,handle);
    callbacks->read(&unk4,4,1,handle);

    if(log)
    log->Log(DEBUG_LEVEL_DEBUG,"unk1: %d, Num sectors wide: %d, Num sectors long: %d, unk2: %d, unk3: %d, unk4: %d",unk1,numSectorsX,numSectorsZ,unk2,unk3,unk4);

    numSectors = numSectorsX*numSectorsZ;
    int* offsetTable = new int[numSectors+1];
    callbacks->read(offsetTable,4,numSectors,handle);
    offsetTable[numSectors] = size;

    sectors = new SectorVisibility[numSectors];

    for(int i = 0; i < numSectors; i++)
    {
        if(offsetTable[i] != -1)
        {
            for(int j = i+1; j < numSectors+1; j++)
            {
                if(offsetTable[j] != -1) //This should never be -1, so should it be removed?
                {
                    if(log)
                    log->Log(DEBUG_LEVEL_RIDICULOUS,"Preparing to load visiblity for sector %d (%d bytes long) at location 0x%X.",i,offsetTable[j]-offsetTable[i],callbacks->tell(handle));
                    if(offsetTable[j]-offsetTable[i] >= 14400)
                    {
                        sectors[i].load(handle,callbacks,offsetTable[j]-offsetTable[i]);
                    }
                    else
                    {
                        if(log)
                        log->Log("ALERT: Block 14 (visibility) - sector %d is missing or corrupt! Repair level as soon as possible!",i);

                        if(offsetTable[j]-offsetTable[i] > 0)

                        callbacks->seek(handle,offsetTable[j]-offsetTable[i],SEEK_CUR);

                        sectors[i].createEmptyData();
                    }
                    break;
                }
            }
        }
    }
    delete[] offsetTable;
    if(log)
    log->Log(DEBUG_LEVEL_DEBUG,"All sector visibility tables loaded successfully.");
    return 0;
};

unsigned int LevelVisibility::getRequiredSize()
{
    unsigned int size = 0;
    for(int i = 0; i < numSectors; i++)
    {
        size += sectors[i].getRequiredSize();
    }
    return 16+4*numSectors+size;
};

int LevelVisibility::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&unk1,4,1,handle);
    callbacks->write(&numSectorsX,2,1,handle);
    callbacks->write(&numSectorsZ,2,1,handle);
    callbacks->write(&unk2,2,1,handle);
    callbacks->write(&unk3,2,1,handle);
    callbacks->write(&unk4,4,1,handle);

    int offset = 16+4*numSectors;

    for(int i = 0; i < numSectors; i++)
    {
        callbacks->write(&offset,4,1,handle);
        offset += sectors[i].getRequiredSize();
    }

    for(int i = 0; i < numSectors; i++)
    {
        sectors[i].save(handle,callbacks);
    }
    return 0;
};
