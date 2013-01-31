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

int RoadTable::load(IOHandle handle, IOCallbacks* callbacks,int size)
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

int RoadTables::load(IOHandle handle, IOCallbacks* callbacks,int size)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numTilesX,4,1,handle);
    callbacks->read(&numTilesZ,4,1,handle);
    callbacks->read(&numSectorsX,4,1,handle);
    callbacks->read(&numSectorsZ,4,1,handle);

    numRoadTables = numSectorsX*numSectorsZ;
    int* offsetTable = new int[numRoadTables+1];
    callbacks->read(offsetTable,4,numRoadTables,handle);
    offsetTable[numRoadTables] = size-8;

    roadTables = new RoadTable[numRoadTables];

    for(int i = 0; i < numRoadTables; i++)
    {
        if(offsetTable[i] != -1)
        {
            for(int j = i+1; j < numRoadTables+1; j++)
            {
                if(offsetTable[j] != -1)
                {
                    roadTables[i].load(handle, callbacks,offsetTable[j]-offsetTable[i]);
                    break;
                }
            }
        }
    }
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

int RoadConnections::load(IOHandle handle, IOCallbacks* callbacks,int size)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numRoads,4,1,handle);
    roadConnections = new RoadConnection[numRoads];

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
    }
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

int RoadSections::load(IOHandle handle, IOCallbacks* callbacks,int size)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numRoadSections,4,1,handle);
    roadSections = new RoadSection[numRoadSections];

    for(int i = 0; i < numRoadSections; i++)
    {
        callbacks->read(&roadSections[i].tileStartX,4,1,handle);
        callbacks->read(&roadSections[i].tileStartZ,4,1,handle);
        callbacks->read(&roadSections[i].tileEndX,4,1,handle);
        callbacks->read(&roadSections[i].tileEndZ,4,1,handle);
        callbacks->read(&roadSections[i].direction,4,1,handle);
    }
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

int Intersections::load(IOHandle handle, IOCallbacks* callbacks,int size)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numIntersections,4,1,handle);
    intersections = new Intersection[numIntersections];

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
    }
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

int IntersectionPositions::load(IOHandle handle, IOCallbacks* callbacks,int size)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;

    callbacks->read(&numIntersections,4,1,handle);
    positions = new Vector2f[numIntersections];

    for(int i = 0; i < numIntersections; i++)
    {
        callbacks->read(&positions[i].x,4,1,handle);
        callbacks->read(&positions[i].y,4,1,handle);
    }
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
