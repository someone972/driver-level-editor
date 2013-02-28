#include "roads.hpp"

RoadTable::RoadTable()
{
    compressedDataSize = 0;
    compressedData = NULL;
};

RoadTable::~RoadTable()
{
    cleanup();
};

void RoadTable::cleanup()
{
    if(compressedData)
    delete[] compressedData;
    compressedData = NULL;
    compressedDataSize = 0;
};

int RoadTable::load(IOHandle handle, IOCallbacks* callbacks, int size)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;

    compressedDataSize = size;
    compressedData = new unsigned char[compressedDataSize];
    callbacks->read(compressedData,1,size,handle);
    return 0;
};


unsigned int RoadTable::getRequiredSize()
{
    return compressedDataSize;
};

int RoadTable::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(compressedData,1,compressedDataSize,handle);
    return 0;
};

RoadTables::RoadTables()
{
    numRoadTables = 0;
    roadTables = NULL;
    numTilesX = 0;
    numTilesZ = 0;
    numSectorsX = 0;
    numSectorsZ = 0;
};

RoadTables::~RoadTables()
{
    cleanup();
};

void RoadTables::cleanup()
{
    if(roadTables)
    delete[] roadTables;
    roadTables = NULL;
    numRoadTables = 0;
};

int RoadTables::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();
    if(!handle || !callbacks)
    return 1;

    if(size < 16)
    {
        log->Log("ERROR: Size required for road table header exceeds size of block!");
        return 2;
    }

    callbacks->read(&numTilesX,4,1,handle);
    callbacks->read(&numTilesZ,4,1,handle);
    callbacks->read(&numSectorsX,4,1,handle);
    callbacks->read(&numSectorsZ,4,1,handle);
    log->Log(DEBUG_LEVEL_VERBOSE, "numTilesX: %d numTilesZ: %d numSectorsX: %d numSectorsZ: %d", numTilesX, numTilesZ, numSectorsX, numSectorsZ);

    if(size < (numSectorsX*numSectorsZ)*4+16)
    {
        log->Log("ERROR: Size required for road offset table exceeds size of block!");
        cleanup();
        return 2;
    }

    numRoadTables = numSectorsX*numSectorsZ;
    int* offsetTable = new int[numRoadTables+1];
    callbacks->read(offsetTable,4,numRoadTables,handle);
    offsetTable[numRoadTables] = size-8;

    char buffer[4096];
    for(int i = 0; i < numSectorsZ; i++)
    {
        unsigned int length = 0;
        for(int j = 0; j < numSectorsX; j++)
        {
            length += snprintf(buffer+length, 4096, "0x%08x ", offsetTable[j+i*numSectorsX]);
        }
        log->Log(DEBUG_LEVEL_RIDICULOUS,"%s",buffer);
    }

    roadTables = new RoadTable[numRoadTables];

    log->increaseIndent();
    for(int i = 0; i < numRoadTables; i++)
    {
        int tableSize = 0;
        if(offsetTable[i] != -1)
        {
            for(int j = i+1; j < numRoadTables+1; j++)
            {
                if(offsetTable[j] != -1)
                {
                    tableSize = offsetTable[j]-offsetTable[i];
                    size -= tableSize;
                    if(size < 0)
                    {
                        log->Log("ERROR: Size required for road tables exceeds size of block!");
                        cleanup();
                        delete[] offsetTable;
                        return 2;
                    }
                    roadTables[i].load(handle, callbacks, offsetTable[j]-offsetTable[i]);
                    break;
                }
            }
        }
        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: Size: %d", i, tableSize);
    }
    log->decreaseIndent();
    delete[] offsetTable;
    return 0;
};

unsigned int RoadTables::getRequiredSize()
{
    unsigned int size = 0;
    for(int i = 0; i < numRoadTables; i++)
    {
        size += roadTables[i].getRequiredSize();
    }
    return 16+numRoadTables*4+size;
};

int RoadTables::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&numTilesX,4,1,handle);
    callbacks->write(&numTilesZ,4,1,handle);
    callbacks->write(&numSectorsX,4,1,handle);
    callbacks->write(&numSectorsZ,4,1,handle);

    int offset = 8+4*numRoadTables;
    int noTable = -1;

    for(int i = 0; i < numRoadTables; i++)
    {
        if(roadTables[i].getRequiredSize() == 0)
        callbacks->write(&noTable,4,1,handle);
        else callbacks->write(&offset,4,1,handle);
        offset += roadTables[i].getRequiredSize();
    }

    for(int i = 0; i < numRoadTables; i++)
    {
        if(roadTables[i].getRequiredSize() != 0)
        {
            roadTables[i].save(handle,callbacks);
        }
    }
    return 0;
};

RoadConnection::RoadConnection()
{
    roadIndex = 0;
    startIntersectionIdx = 0;
    endIntersectionIdx = 0;
    unk1 = 0;
    unk2 = 0;
    unk3 = 0;
    unk4 = 0;

    for(int i = 0; i < 3*4; i++)
    unk5[i] = 0;

    unk6 = 0;
    unk7 = 0;
    originTileX = 0;
    originTileZ = 0;
};

RoadConnections::RoadConnections()
{
    numRoads = 0;
    roadConnections = NULL;
};

RoadConnections::~RoadConnections()
{
    cleanup();
};

void RoadConnections::cleanup()
{
    if(roadConnections)
    delete[] roadConnections;
    roadConnections = NULL;
    numRoads = 0;
};

int RoadConnections::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();
    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numRoads,4,1,handle);
    log->Log(DEBUG_LEVEL_DEBUG, "Loading %d road connections.", numRoads);

    if(size < 4+numRoads*66)
    {
        numRoads = 0;
        log->Log("ERROR: Required size to load road connections exceeds size of block!");
        return 2;
    }

    roadConnections = new RoadConnection[numRoads];

    log->increaseIndent();
    for(int i = 0; i < numRoads; i++)
    {
        callbacks->read(&roadConnections[i].roadIndex,2,1,handle);
        callbacks->read(&roadConnections[i].startIntersectionIdx,2,1,handle);
        callbacks->read(&roadConnections[i].endIntersectionIdx,2,1,handle);
        callbacks->read(&roadConnections[i].unk1,1,1,handle);
        callbacks->read(&roadConnections[i].unk2,1,1,handle);
        callbacks->read(&roadConnections[i].unk3,2,1,handle);
        callbacks->read(&roadConnections[i].unk4,2,1,handle);

        for(int j = 0; j < 3*4; j++)
        callbacks->read(&roadConnections[i].unk5[j],4,1,handle);

        callbacks->read(&roadConnections[i].unk6,1,1,handle);
        callbacks->read(&roadConnections[i].unk7,1,1,handle);
        callbacks->read(&roadConnections[i].originTileX,2,1,handle);
        callbacks->read(&roadConnections[i].originTileZ,2,1,handle);

        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: roadIndex: %hd startIntersectionIdx: %hd endIntersectionIdx: %hd unk1: %hhd unk2: %hhd unk3: %hd unk4: %hd unk6: %hhd unk7: %hhd originTileX: %hd originTileZ: %hu",
                 i, roadConnections[i].roadIndex, roadConnections[i].startIntersectionIdx, roadConnections[i].endIntersectionIdx, roadConnections[i].unk1, roadConnections[i].unk2, roadConnections[i].unk3,
                 roadConnections[i].unk4, roadConnections[i].unk6, roadConnections[i].unk7, roadConnections[i].originTileX, roadConnections[i].originTileZ);
        log->Log(DEBUG_LEVEL_RIDICULOUS, "   unk5: %d %d %d %d %d %d %d %d %d %d %d %d", roadConnections[i].unk5[0], roadConnections[i].unk5[1], roadConnections[i].unk5[2],
                 roadConnections[i].unk5[3], roadConnections[i].unk5[4], roadConnections[i].unk5[5], roadConnections[i].unk5[6], roadConnections[i].unk5[7],
                 roadConnections[i].unk5[8], roadConnections[i].unk5[9], roadConnections[i].unk5[10], roadConnections[i].unk5[11]);

    }
    log->decreaseIndent();
    return 0;
};

unsigned int RoadConnections::getRequiredSize()
{
    return 4+numRoads*66;
};

int RoadConnections::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&numRoads,4,1,handle);

    for(int i = 0; i < numRoads; i++)
    {
        callbacks->write(&roadConnections[i].roadIndex,2,1,handle);
        callbacks->write(&roadConnections[i].startIntersectionIdx,2,1,handle);
        callbacks->write(&roadConnections[i].endIntersectionIdx,2,1,handle);
        callbacks->write(&roadConnections[i].unk1,1,1,handle);
        callbacks->write(&roadConnections[i].unk2,1,1,handle);
        callbacks->write(&roadConnections[i].unk3,2,1,handle);
        callbacks->write(&roadConnections[i].unk4,2,1,handle);

        for(int j = 0; j < 3*4; j++)
        callbacks->write(&roadConnections[i].unk5[j],4,1,handle);

        callbacks->write(&roadConnections[i].unk6,1,1,handle);
        callbacks->write(&roadConnections[i].unk7,1,1,handle);
        callbacks->write(&roadConnections[i].originTileX,2,1,handle);
        callbacks->write(&roadConnections[i].originTileZ,2,1,handle);
    }
    return 0;
};

RoadSection::RoadSection()
{
    tileStartX = 0;
    tileStartZ = 0;
    tileEndX = 0;
    tileEndZ = 0;
    direction = 0;
};

RoadSections::RoadSections()
{
    numRoadSections = 0;
    roadSections = NULL;
};

RoadSections::~RoadSections()
{
    cleanup();
};

void RoadSections::cleanup()
{
    if(roadSections)
    delete[] roadSections;
    roadSections = NULL;
    numRoadSections = 0;
};

int RoadSections::getNumRoadSections()
{
    return numRoadSections;
};

int RoadSections::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();
    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numRoadSections,4,1,handle);
    log->Log(DEBUG_LEVEL_DEBUG, "Loading %d road sections.", numRoadSections);

    if(size < 4+numRoadSections*20)
    {
        log->Log("ERROR: Size of road section data exceeds size of block!");
        numRoadSections = 0;
        return 2;
    }

    roadSections = new RoadSection[numRoadSections];

    log->increaseIndent();
    for(int i = 0; i < numRoadSections; i++)
    {
        callbacks->read(&roadSections[i].tileStartX,4,1,handle);
        callbacks->read(&roadSections[i].tileStartZ,4,1,handle);
        callbacks->read(&roadSections[i].tileEndX,4,1,handle);
        callbacks->read(&roadSections[i].tileEndZ,4,1,handle);
        callbacks->read(&roadSections[i].direction,4,1,handle);
        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: tileStartX: %d tileStartZ: %d tileEndX: %d tileEndZ: %d direction: %d", i,
                 roadSections[i].tileStartX,roadSections[i].tileStartZ,roadSections[i].tileEndX,roadSections[i].tileEndZ,roadSections[i].direction);
    }
    log->decreaseIndent();
    return 0;
};

unsigned int RoadSections::getRequiredSize()
{
    return 4+numRoadSections*20;
};

int RoadSections::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&numRoadSections,4,1,handle);

    for(int i = 0; i < numRoadSections; i++)
    {
        callbacks->write(&roadSections[i].tileStartX,4,1,handle);
        callbacks->write(&roadSections[i].tileStartZ,4,1,handle);
        callbacks->write(&roadSections[i].tileEndX,4,1,handle);
        callbacks->write(&roadSections[i].tileEndZ,4,1,handle);
        callbacks->write(&roadSections[i].direction,4,1,handle);
    }
    return 0;
};

IntersectionConnection::IntersectionConnection()
{
    intersectionIndex = 0;
    roadIndex = 0;
    unk1 = 0;
    unk2 = 0;
};

Intersection::Intersection()
{
    intersectionIndex = 0;
    unk1 = 0;
    unk2 = 0;
    unk3 = 0;
    originTileX = 0;
    originTileZ = 0;
};

Intersections::Intersections()
{
    numIntersections = 0;
    intersections = NULL;
};

Intersections::~Intersections()
{
    cleanup();
};

void Intersections::cleanup()
{
    if(intersections)
    delete[] intersections;
    intersections = NULL;
    numIntersections = 0;
};

int Intersections::getNumIntersections()
{
    return numIntersections;
};

int Intersections::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();
    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numIntersections,4,1,handle);
    log->Log(DEBUG_LEVEL_DEBUG, "Loading %d intersections.", numIntersections);

    if(size < 4+numIntersections*44)
    {
        log->Log("ERROR: Required size for intersections exceeds block size!");
        numIntersections = 0;
        return 2;
    }

    intersections = new Intersection[numIntersections];

    log->increaseIndent();
    for(int i = 0; i < numIntersections; i++)
    {
        callbacks->read(&intersections[i].intersectionIndex,2,1,handle);
        callbacks->read(&intersections[i].unk1,1,1,handle);
        callbacks->read(&intersections[i].unk2,1,1,handle);
        callbacks->read(&intersections[i].unk3,4,1,handle);
        for(int j = 0; j < 4; j++)
        {
            callbacks->read(&intersections[i].connections[j].intersectionIndex,2,1,handle);
            callbacks->read(&intersections[i].connections[j].roadIndex,2,1,handle);
            callbacks->read(&intersections[i].connections[j].unk1,2,1,handle);
            callbacks->read(&intersections[i].connections[j].unk2,2,1,handle);
        }
        callbacks->read(&intersections[i].originTileX,2,1,handle);
        callbacks->read(&intersections[i].originTileZ,2,1,handle);
        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: intersectionIndex: %hu unk1: %hhu unk2: %hhu, unk3: %u originTileX: %hd originTileZ: %hd",
                 i, intersections[i].intersectionIndex, intersections[i].unk1, intersections[i].unk2, intersections[i].unk3,
                 intersections[i].originTileX, intersections[i].originTileZ);
        log->increaseIndent();
        for(int j = 0; j < 4; j++)
        {
            log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: intersectionIndex: %hd roadIndex: %hd unk1: %hd unk2: %hd", j, intersections[i].connections[j].intersectionIndex,
                     intersections[i].connections[j].roadIndex, intersections[i].connections[j].unk1, intersections[i].connections[j].unk2);
        }
        log->decreaseIndent();
    }
    log->decreaseIndent();
    return 0;
};

unsigned int Intersections::getRequiredSize()
{
    return 4+numIntersections*44;
};

int Intersections::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&numIntersections,4,1,handle);

    for(int i = 0; i < numIntersections; i++)
    {
        callbacks->write(&intersections[i].intersectionIndex,2,1,handle);
        callbacks->write(&intersections[i].unk1,1,1,handle);
        callbacks->write(&intersections[i].unk2,1,1,handle);
        callbacks->write(&intersections[i].unk3,4,1,handle);
        for(int j = 0; j < 4; j++)
        {
            callbacks->write(&intersections[i].connections[j].intersectionIndex,2,1,handle);
            callbacks->write(&intersections[i].connections[j].roadIndex,2,1,handle);
            callbacks->write(&intersections[i].connections[j].unk1,2,1,handle);
            callbacks->write(&intersections[i].connections[j].unk2,2,1,handle);
        }
        callbacks->write(&intersections[i].originTileX,2,1,handle);
        callbacks->write(&intersections[i].originTileZ,2,1,handle);
    }
    return 0;
};

IntersectionPositions::IntersectionPositions()
{
    numIntersections = 0;
    positions = NULL;
};

IntersectionPositions::~IntersectionPositions()
{
    cleanup();
};

void IntersectionPositions::cleanup()
{
    if(positions)
    delete[] positions;
    positions = NULL;
    numIntersections = 0;
};

int IntersectionPositions::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();
    if(!handle || !callbacks)
    return 1;

    if(size < 4)
    {
        log->Log("ERROR: Required size for intersection positions greater than block size!");
        return 2;
    }

    callbacks->read(&numIntersections,4,1,handle);
    log->Log(DEBUG_LEVEL_DEBUG, "Loading %d intersection positions.", numIntersections);
    if(size < 4+numIntersections*8)
    {
        numIntersections = 0;
        log->Log("ERROR: Required size for intersection positions greater than block size!");
        return 2;
    }

    positions = new Vector2f[numIntersections];

    log->increaseIndent();
    for(int i = 0; i < numIntersections; i++)
    {
        callbacks->read(&positions[i].x,4,1,handle);
        callbacks->read(&positions[i].y,4,1,handle);
        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: x: %f y: %f", i, positions[i].x, positions[i].y);
    }
    log->decreaseIndent();
    return 0;
};

unsigned int IntersectionPositions::getRequiredSize()
{
    return 4+numIntersections*8;
};

int IntersectionPositions::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&numIntersections,4,1,handle);

    for(int i = 0; i < numIntersections; i++)
    {
        callbacks->write(&positions[i].x,4,1,handle);
        callbacks->write(&positions[i].y,4,1,handle);
    }
    return 0;
};

Vector2f IntersectionPositions::getPosition(int idx)
{
    if(idx >= 0 && idx < numIntersections)
    return positions[idx];
    return Vector2f(0,0);
};

void IntersectionPositions::setPosition(int idx, Vector2f pos)
{
    if(idx >= 0 && idx < numIntersections)
    positions[idx] = pos;
};

void IntersectionPositions::insertPosition(int idx,Vector2f pos,bool before)
{
    if((idx >= 0 && idx < numIntersections) || (idx == -1 && numIntersections == 0))
    {
        int realIdx = idx+(before == true ? 0 : 1);
        if(idx == -1)
        realIdx = 0;

        Vector2f* tempData = new Vector2f[numIntersections+1];
        if(positions)
        memcpy(tempData,positions,realIdx*sizeof(Vector2f));

        tempData[realIdx] = pos;

        if(positions)
        {
            memcpy(&tempData[realIdx+1],positions,numIntersections-realIdx);
            delete[] positions;
        }
        positions = tempData;
        numIntersections++;
    }
};

int IntersectionPositions::addPosition(Vector2f pos)
{
    insertPosition(numIntersections-1,pos,false);
    return numIntersections-1;
};

int IntersectionPositions::getNumIntersections()
{
    return numIntersections;
};
