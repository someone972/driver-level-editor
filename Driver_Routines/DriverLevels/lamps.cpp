#include "lamps.hpp"

DriverLamp::DriverLamp()
{
    type = 0;
    radius = 3000.0;
    r = 255;
    g = 255;
    b = 255;
    unk1 = 0.0;
    unk2 = 0;
};

DriverLampList::DriverLampList()
{
    visTileIndex = -1;
    numLamps = 0;
    lamps = NULL;
};

DriverLampList::~DriverLampList()
{
    cleanup();
};

void DriverLampList::cleanup()
{
    if(lamps)
    delete[] lamps;
    lamps = NULL;
    numLamps = 0;
};

int DriverLampList::load(IOHandle handle, IOCallbacks* callbacks)
{
    cleanup();

    if(!handle || !callbacks)
    return 1;

    callbacks->read(&visTileIndex,4,1,handle);
    callbacks->read(&numLamps,4,1,handle);
    lamps = new DriverLamp[numLamps];

    for(int i = 0; i < numLamps; i++)
    {
        callbacks->read(&lamps[i].type,4,1,handle);
        callbacks->read(&lamps[i].radius,4,1,handle);
        callbacks->read(&lamps[i].position.x,4,1,handle);
        callbacks->read(&lamps[i].position.y,4,1,handle);
        callbacks->read(&lamps[i].position.z,4,1,handle);
        callbacks->read(&lamps[i].unk1,4,1,handle);
        callbacks->read(&lamps[i].r,4,1,handle);
        callbacks->read(&lamps[i].g,4,1,handle);
        callbacks->read(&lamps[i].b,4,1,handle);
        callbacks->read(&lamps[i].unk2,4,1,handle);
    }
    return 0;
};

int DriverLampList::getVisibilityTileIndex()
{
    return visTileIndex;
};

unsigned int DriverLampList::getRequiredSize()
{
    return 8+numLamps*40;
};

int DriverLampList::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(&visTileIndex,4,1,handle);
    callbacks->write(&numLamps,4,1,handle);

    for(int i = 0; i < numLamps; i++)
    {
        callbacks->write(&lamps[i].type,4,1,handle);
        callbacks->write(&lamps[i].radius,4,1,handle);
        callbacks->write(&lamps[i].position.x,4,1,handle);
        callbacks->write(&lamps[i].position.y,4,1,handle);
        callbacks->write(&lamps[i].position.z,4,1,handle);
        callbacks->write(&lamps[i].unk1,4,1,handle);
        callbacks->write(&lamps[i].r,4,1,handle);
        callbacks->write(&lamps[i].g,4,1,handle);
        callbacks->write(&lamps[i].b,4,1,handle);
        callbacks->write(&lamps[i].unk2,4,1,handle);
    }
    return 0;
};

DriverLamps::DriverLamps()
{
    numLampLists = 0;
    lists = NULL;
    listLookupLength = 0;
    listLookup = NULL;
    version = 0;
    mapWidth = 0;
    mapHeight = 0;
};

DriverLamps::~DriverLamps()
{
    cleanup();
};

void DriverLamps::cleanup()
{
    if(lists)
    delete[] lists;
    lists = NULL;
    numLampLists = 0;

    if(listLookup)
    delete[] listLookup;
    listLookup = NULL;
    listLookupLength = 0;
};

int DriverLamps::load(IOHandle handle, IOCallbacks* callbacks)
{
    cleanup();

    if(!handle || !callbacks)
    return 1;

    unsigned long int start = callbacks->tell(handle);

    callbacks->read(id,4,1,handle);
    if(strncmp(id,"GLMP",4) != 0)
    {
        callbacks->seek(handle,start,SEEK_SET);
        return 2;
    }

    callbacks->read(&version,4,1,handle);
    callbacks->read(&mapWidth,4,1,handle);
    callbacks->read(&mapHeight,4,1,handle);
    callbacks->read(&numLampLists,4,1,handle);

    listLookupLength = mapWidth*mapHeight;
    listLookup = new DriverLampList*[listLookupLength];
    memset(listLookup,0,listLookupLength*sizeof(DriverLampList*));

    lists = new DriverLampList[numLampLists];

    for(int i = 0; i < numLampLists; i++)
    {
        lists[i].load(handle,callbacks);
        if(lists[i].getVisibilityTileIndex() < listLookupLength)
        listLookup[lists[i].getVisibilityTileIndex()] = &lists[i];
    }

    return 0;
};

unsigned int DriverLamps::getRequiredSize()
{
    int size = 0;
    for(int i = 0; i < numLampLists; i++)
    {
        size += lists[i].getRequiredSize();
    }
    return 20+size;
};

int DriverLamps::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    callbacks->write(id,4,1,handle);
    callbacks->write(&version,4,1,handle);
    callbacks->write(&mapWidth,4,1,handle);
    callbacks->write(&mapHeight,4,1,handle);
    callbacks->write(&numLampLists,4,1,handle);

    for(int i = 0; i < numLampLists; i++)
    {
        lists[i].save(handle,callbacks);
    }

    return 0;
};
