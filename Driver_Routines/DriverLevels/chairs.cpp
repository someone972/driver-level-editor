#include "chairs.hpp"

DriverChair::DriverChair()
{
    x = 0;
    z = 0;
    inUse = 0;
};

DriverChairList::DriverChairList()
{
    visTileIndex = -1;
    numChairs = 0;
    chairs = NULL;
};

DriverChairList::~DriverChairList()
{
    cleanup();
};

void DriverChairList::cleanup()
{
    if(chairs)
    delete[] chairs;
    chairs = NULL;
    numChairs = 0;
};

int DriverChairList::getVisibilityTileIndex()
{
    return visTileIndex;
};

unsigned int DriverChairList::getRequiredSize()
{
    return 12+20*numChairs;
};

int DriverChairList::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle)
    return 1;

    int temp = 0;

    callbacks->write(&visTileIndex,4,1,handle);
    callbacks->write(&numChairs,4,1,handle);
    callbacks->write(&temp,4,1,handle); //This is where a worthless memory location was stored from when it was originally written

    for(int i = 0; i < numChairs; i++)
    {
        callbacks->write(&chairs[i].x,4,1,handle);
        callbacks->write(&chairs[i].z,4,1,handle);
        callbacks->write(&chairs[i].inUse,4,1,handle);//Also useless
        callbacks->write(&chairs[i].rotation.x,2,1,handle);
        callbacks->write(&chairs[i].rotation.y,2,1,handle);
        callbacks->write(&chairs[i].rotation.z,2,1,handle);
        callbacks->write(&chairs[i].rotation.padding,2,1,handle);
    }
    return 0;
};

int DriverChairList::load(IOHandle handle, IOCallbacks* callbacks,DebugLogger* log)
{
    cleanup();

    if(!handle)
    {
        if(log)
        log->Log("ERROR: File pointer is NULL!");
        return 1;
    }

    callbacks->read(&visTileIndex,4,1,handle);
    callbacks->read(&numChairs,4,1,handle);
    callbacks->seek(handle,4,SEEK_CUR); //This is where a worthless memory location was stored from when it was originally written

    if(log)
    log->Log(DEBUG_LEVEL_DEBUG,"Vis tile index: %d; Number of chairs: %d");

    chairs = new DriverChair[numChairs];

    for(int i = 0; i < numChairs; i++)
    {
        callbacks->read(&chairs[i].x,4,1,handle);
        callbacks->read(&chairs[i].z,4,1,handle);
        callbacks->read(&chairs[i].inUse,4,1,handle);//Also useless
        callbacks->read(&chairs[i].rotation.x,2,1,handle);
        callbacks->read(&chairs[i].rotation.y,2,1,handle);
        callbacks->read(&chairs[i].rotation.z,2,1,handle);
        callbacks->read(&chairs[i].rotation.padding,2,1,handle);
        if(log)
        log->Log(DEBUG_LEVEL_RIDICULOUS,"%d: Position (%d,%d), inUse %d, Rotation (%d,%d,%d)",i,chairs[i].x,chairs[i].z,
                 chairs[i].inUse,chairs[i].rotation.x,chairs[i].rotation.y,chairs[i].rotation.z);
    }
    return 0;
};

DriverChairs::DriverChairs()
{
    numChairLists = 0;
    lists = NULL;
    listLookupLength = 0;
    listLookup = NULL;
    version = 0;
    mapWidth = 0;
    mapHeight = 0;
};

DriverChairs::~DriverChairs()
{
    cleanup();
};

void DriverChairs::cleanup()
{
    if(lists)
    delete[] lists;
    lists = NULL;
    numChairLists = 0;

    if(listLookup)
    delete[] listLookup;
    listLookup = NULL;
    listLookupLength = 0;
};

int DriverChairs::load(IOHandle handle, IOCallbacks* callbacks,DebugLogger* log)
{
    cleanup();

    if(!handle)
    return 1;

    unsigned long int start = callbacks->tell(handle);

    callbacks->read(id,4,1,handle);
    if(strncmp(id,"GCHR",4) != 0)
    {
        if(log)
        log->Log("ERROR: GCHR magic number not found in header.");
        callbacks->seek(handle,start,SEEK_SET);
        return 2;
    }

    callbacks->read(&version,4,1,handle);
    callbacks->read(&mapWidth,4,1,handle);
    callbacks->read(&mapHeight,4,1,handle);
    callbacks->read(&numChairLists,4,1,handle);
    if(log)
    {
        log->Log(DEBUG_LEVEL_VERBOSE,"Version: %d",version);
        log->Log(DEBUG_LEVEL_NORMAL,"Chair table is %d by %d cells.");
        log->Log(DEBUG_LEVEL_NORMAL,"Loading %d chair lists.",numChairLists);
    }

    listLookupLength = mapWidth*mapHeight;
    listLookup = new DriverChairList*[listLookupLength];
    memset(listLookup,0,listLookupLength*sizeof(DriverChairList*));

    lists = new DriverChairList[numChairLists];

    for(int i = 0; i < numChairLists; i++)
    {
        lists[i].load(handle,callbacks);
        if(lists[i].getVisibilityTileIndex() < listLookupLength)
        listLookup[lists[i].getVisibilityTileIndex()] = &lists[i];
    }

    return 0;
};

unsigned int DriverChairs::getRequiredSize()
{
    unsigned int size = 0;
    for(int i = 0; i < numChairLists; i++)
    {
        size += lists[i].getRequiredSize();
    }
    return 20+size;
};

int DriverChairs::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle)
    return 1;

    callbacks->write(id,4,1,handle);

    callbacks->write(&version,4,1,handle);
    callbacks->write(&mapWidth,4,1,handle);
    callbacks->write(&mapHeight,4,1,handle);
    callbacks->write(&numChairLists,4,1,handle);

    for(int i = 0; i < numChairLists; i++)
    {
        lists[i].save(handle,callbacks);
    }

    return 0;
};
