#include "heightmaps.hpp"

HeightmapTileFace::HeightmapTileFace()
{
    numVerts = 0;
};

HeightmapTile::HeightmapTile()
{
    modelIdx = 0;
    unk1 = 0;
    unk2 = 0;
    numFaces = 0;
    faces = NULL;
};

HeightmapTile::~HeightmapTile()
{
    cleanup();
};

void HeightmapTile::cleanup()
{
    if(faces)
    delete[] faces;
    faces = NULL;
    numFaces = 0;
};

int HeightmapTile::getNumFaces()
{
    return numFaces;
};

int HeightmapTile::getModelIndex()
{
    return modelIdx;
};

void HeightmapTile::setModelIndex(int index)
{
    modelIdx = index;
};

int HeightmapTile::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
        log = &dummy;

    cleanup();
    if(!handle || !callbacks)
    {
        if(log)
        log->Log("ERROR: I/O handle is invalid!");
        return -1;
    };

    if(size < 8)
        return -2;

    callbacks->read(&modelIdx,2,1,handle);
    callbacks->read(&unk1,2,1,handle);
    callbacks->read(&unk2,2,1,handle);
    callbacks->read(&numFaces,2,1,handle);

    if(log)
    log->Log(DEBUG_LEVEL_DEBUG,"Model index: %d, Unk1: %d, Unk2: %d, Number of faces: %d",modelIdx,unk1,unk2,numFaces);

    if(size < 8+numFaces*0x34)
        return -2;

    faces = new HeightmapTileFace[numFaces];
    for(int i = 0; i < numFaces; i++)
    {
        callbacks->read(&faces[i].v1.x,4,1,handle);
        callbacks->read(&faces[i].v1.y,4,1,handle);
        callbacks->read(&faces[i].v2.x,4,1,handle);
        callbacks->read(&faces[i].v2.y,4,1,handle);
        callbacks->read(&faces[i].v3.x,4,1,handle);
        callbacks->read(&faces[i].v3.y,4,1,handle);
        callbacks->read(&faces[i].v4.x,4,1,handle);
        callbacks->read(&faces[i].v4.y,4,1,handle);
        callbacks->read(&faces[i].normal.x,4,1,handle);
        callbacks->read(&faces[i].normal.y,4,1,handle);
        callbacks->read(&faces[i].normal.z,4,1,handle);
        callbacks->read(&faces[i].normal.w,4,1,handle);
        callbacks->read(&faces[i].numVerts,4,1,handle);
        if(log)
        log->Log(DEBUG_LEVEL_RIDICULOUS,"V1: (%d,%d), V2: (%d,%d), V3: (%d,%d), V4: (%d,%d), Normal: (%f,%f,%f,%f), Num Verts: %d", faces[i].v1.x,faces[i].v1.y,faces[i].v2.x,
                 faces[i].v2.y,faces[i].v3.x,faces[i].v3.y,faces[i].v4.x,faces[i].v4.y,faces[i].normal.x,faces[i].normal.y,faces[i].normal.z,faces[i].normal.w,faces[i].numVerts);
    }
    return 8+numFaces*0x34;
};

unsigned int HeightmapTile::getRequiredSize()
{
    return 8+numFaces*0x34;
};

int HeightmapTile::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return -1;

    callbacks->write(&modelIdx,2,1,handle);
    callbacks->write(&unk1,2,1,handle);
    callbacks->write(&unk2,2,1,handle);
    callbacks->write(&numFaces,2,1,handle);

    for(int i = 0; i < numFaces; i++)
    {
        callbacks->write(&faces[i].v1.x,4,1,handle);
        callbacks->write(&faces[i].v1.y,4,1,handle);
        callbacks->write(&faces[i].v2.x,4,1,handle);
        callbacks->write(&faces[i].v2.y,4,1,handle);
        callbacks->write(&faces[i].v3.x,4,1,handle);
        callbacks->write(&faces[i].v3.y,4,1,handle);
        callbacks->write(&faces[i].v4.x,4,1,handle);
        callbacks->write(&faces[i].v4.y,4,1,handle);
        callbacks->write(&faces[i].normal.x,4,1,handle);
        callbacks->write(&faces[i].normal.y,4,1,handle);
        callbacks->write(&faces[i].normal.z,4,1,handle);
        callbacks->write(&faces[i].normal.w,4,1,handle);
        callbacks->write(&faces[i].numVerts,4,1,handle);
    }
    return getRequiredSize();
};

HeightmapTiles::HeightmapTiles()
{
    numTiles = 0;
    tiles = NULL;
};

HeightmapTiles::~HeightmapTiles()
{
    cleanup();
};

void HeightmapTiles::cleanup()
{
    if(tiles)
    delete[] tiles;
    tiles = NULL;
    numTiles = 0;
};

int HeightmapTiles::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    cleanup();
    if(!handle || !callbacks)
    {
        if(log)
        log->Log("ERROR: Invalid I/O handle!");
        return 1;
    }

    callbacks->read(&numTiles,4,1,handle);
    if(log)
    log->Log(DEBUG_LEVEL_NORMAL,"Loading %d tiles.",numTiles);

    size -= 4;
    if(size < (int)numTiles*(int)sizeof(HeightmapTile))
    {
        cleanup();
        log->Log("ERROR: Size mismatch (data corrupt).");
        return 2;
    }

    tiles = new HeightmapTile[numTiles];

    log->increaseIndent();
    for(int i = 0; i < numTiles; i++)
    {
        int tileSize = tiles[i].load(handle,callbacks,size,log);
        size -= tileSize;
        if(tileSize < 0)
        {
            cleanup();
            log->Log("ERROR: Size mismatch (data corrupt).");
            return 2;
        }
    }
    log->decreaseIndent();
    return 0;
};

unsigned int HeightmapTiles::getRequiredSize()
{
    unsigned int size = 0;
    for(int i = 0; i < numTiles; i++)
    {
        size += tiles[i].getRequiredSize();
    }
    return size+4;
};

int HeightmapTiles::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&numTiles,4,1,handle);

    for(int i = 0; i < numTiles; i++)
    {
        tiles[i].save(handle,callbacks);
    }
    return 0;
};

int HeightmapTiles::getNumTiles()
{
    return numTiles;
};

HeightmapTile* HeightmapTiles::getTile(int idx)
{
    if(idx >= 0 && idx < numTiles)
    {
        return &tiles[idx];
    }
    return NULL;
};

DriverHeightmap::DriverHeightmap()
{
    compressedDataSize = 0;
    compressedData = NULL;
};

DriverHeightmap::~DriverHeightmap()
{
    cleanup();
};

void DriverHeightmap::cleanup()
{
    if(compressedData)
    delete[] compressedData;
    compressedData = NULL;
    compressedDataSize = 0;
};

int DriverHeightmap::load(IOHandle handle, IOCallbacks* callbacks,int size)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;

    compressedDataSize = size;
    compressedData = new unsigned char[compressedDataSize];
    callbacks->read(compressedData,1,size,handle);
    return 0;
};

unsigned int DriverHeightmap::getRequiredSize()
{
    return compressedDataSize;
};

int DriverHeightmap::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(compressedData,1,compressedDataSize,handle);
    return 0;
};

DriverHeightmaps::DriverHeightmaps()
{
    numHeightmaps = 0;
    heightmaps = NULL;
    numTilesX = 0;
    numTilesZ = 0;
    numSectorsX = 0;
    numSectorsZ = 0;
};

DriverHeightmaps::~DriverHeightmaps()
{
    cleanup();
};

void DriverHeightmaps::cleanup()
{
    if(heightmaps)
    delete[] heightmaps;
    heightmaps = NULL;
    numHeightmaps = 0;
};

int DriverHeightmaps::load(IOHandle handle, IOCallbacks* callbacks,int size,DebugLogger* log)
{
    cleanup();
    if(!handle || !callbacks)
    {
        if(log)
        log->Log("ERROR: File pointer is NULL!");
        return 1;
    }

    callbacks->read(&numTilesX,4,1,handle);
    callbacks->read(&numTilesZ,4,1,handle);
    callbacks->read(&numSectorsX,4,1,handle);
    callbacks->read(&numSectorsZ,4,1,handle);

    if(log)
    {
        log->Log(DEBUG_LEVEL_VERBOSE,"Dimesions: %dx%d tiles, %dx%d sectors.",numTilesX,numTilesZ,numSectorsX,numSectorsZ);
        log->Log(DEBUG_LEVEL_NORMAL,"Loading %d heightmaps.",numSectorsX*numSectorsZ);
    }

    numHeightmaps = numSectorsX*numSectorsZ;
    int* offsetTable = new int[numHeightmaps+1];
    callbacks->read(offsetTable,4,numHeightmaps,handle);
    offsetTable[numHeightmaps] = size-8;

    heightmaps = new DriverHeightmap[numHeightmaps];

    for(int i = 0; i < numHeightmaps; i++)
    {
        if(offsetTable[i] != -1)
        {
            for(int j = i+1; j < numHeightmaps+1; j++)
            {
                if(offsetTable[j] != -1)
                {
                    heightmaps[i].load(handle,callbacks,offsetTable[j]-offsetTable[i]);
                    break;
                }
            }
        }
    }
    delete[] offsetTable;
    return 0;
};

unsigned int DriverHeightmaps::getRequiredSize()
{
    unsigned int size = 0;
    for(int i = 0; i < numHeightmaps; i++)
    {
        size += heightmaps[i].getRequiredSize();
    }
    return size+16+4*numHeightmaps;
};

int DriverHeightmaps::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&numTilesX,4,1,handle);
    callbacks->write(&numTilesZ,4,1,handle);
    callbacks->write(&numSectorsX,4,1,handle);
    callbacks->write(&numSectorsZ,4,1,handle);

    int offset = 8+4*numHeightmaps;
    int noTable = -1;

    for(int i = 0; i < numHeightmaps; i++)
    {
        if(heightmaps[i].getRequiredSize() == 0)
        callbacks->write(&noTable,4,1,handle);
        else callbacks->write(&offset,4,1,handle);
        offset += heightmaps[i].getRequiredSize();
    }

    for(int i = 0; i < numHeightmaps; i++)
    {
        if(heightmaps[i].getRequiredSize() != 0)
        {
            heightmaps[i].save(handle,callbacks);
        }
    }
    return 0;
};
