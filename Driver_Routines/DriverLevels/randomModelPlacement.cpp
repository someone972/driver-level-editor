#include "randomModelPlacement.hpp"

RandomModelPlacement::RandomModelPlacement()
{
    modelNumber = 0;
    unk = 0;
};

RandomModelPlacements::RandomModelPlacements()
{
    numPlacements = 0;
    placements = NULL;
};

RandomModelPlacements::~RandomModelPlacements()
{
    cleanup();
};

void RandomModelPlacements::cleanup()
{
    if(placements)
    delete[] placements;
    placements = NULL;
    numPlacements = 0;
};

int RandomModelPlacements::getNumPlacements()
{
    return numPlacements;
};

int RandomModelPlacements::load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log)
{
    DebugLogger dummy;
    if(log == NULL)
    log = &dummy;

    cleanup();
    if(!handle || !callbacks)
    return 1;

    if(size%44 != 0)
    {
        log->Log("ERROR: Block size is not multiple of model placement size.");
        return 2;
    }

    numPlacements = size/44;
    log->Log(DEBUG_LEVEL_DEBUG, "Loading %d random placements.", numPlacements);

    placements = new RandomModelPlacement[numPlacements];

    log->increaseIndent();
    for(int i = 0; i < numPlacements; i++)
    {
        callbacks->read(&placements[i].position.x,4,1,handle);
        callbacks->read(&placements[i].position.y,4,1,handle);
        callbacks->read(&placements[i].position.z,4,1,handle);

        callbacks->read(&placements[i].rotation.x,2,1,handle);
        callbacks->read(&placements[i].rotation.y,2,1,handle);
        callbacks->read(&placements[i].rotation.z,2,1,handle);
        callbacks->read(&placements[i].rotation.padding,2,1,handle);

        callbacks->read(&placements[i].position2.x,4,1,handle);
        callbacks->read(&placements[i].position2.y,4,1,handle);
        callbacks->read(&placements[i].position2.z,4,1,handle);

        callbacks->read(&placements[i].rotation2.x,2,1,handle);
        callbacks->read(&placements[i].rotation2.y,2,1,handle);
        callbacks->read(&placements[i].rotation2.z,2,1,handle);
        callbacks->read(&placements[i].rotation2.padding,2,1,handle);

        callbacks->read(&placements[i].modelNumber,2,1,handle);
        callbacks->read(&placements[i].unk,2,1,handle);

        log->Log(DEBUG_LEVEL_RIDICULOUS, "%d: position: (%f, %f, %f) rotation: (%hd, %hd, %hd) position2: (%f, %f, %f) rotation2: (%hd, %hd, %hd) modelNumber: %hd unk: %hd",
                 i, placements[i].position.x, placements[i].position.y, placements[i].position.z, placements[i].rotation.x, placements[i].rotation.y, placements[i].rotation.z,
                 placements[i].position2.x, placements[i].position2.y, placements[i].position2.z, placements[i].rotation2.x, placements[i].rotation2.y, placements[i].rotation2.z,
                 placements[i].modelNumber, placements[i].unk);
    }
    log->decreaseIndent();
    return 0;
};

unsigned int RandomModelPlacements::getRequiredSize()
{
    return 44*numPlacements;
};

int RandomModelPlacements::save(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks)
    return 1;

    for(int i = 0; i < numPlacements; i++)
    {
        callbacks->write(&placements[i].position.x,4,1,handle);
        callbacks->write(&placements[i].position.y,4,1,handle);
        callbacks->write(&placements[i].position.z,4,1,handle);

        callbacks->write(&placements[i].rotation.x,2,1,handle);
        callbacks->write(&placements[i].rotation.y,2,1,handle);
        callbacks->write(&placements[i].rotation.z,2,1,handle);
        callbacks->write(&placements[i].rotation.padding,2,1,handle);

        callbacks->write(&placements[i].position2.x,4,1,handle);
        callbacks->write(&placements[i].position2.y,4,1,handle);
        callbacks->write(&placements[i].position2.z,4,1,handle);

        callbacks->write(&placements[i].rotation2.x,2,1,handle);
        callbacks->write(&placements[i].rotation2.y,2,1,handle);
        callbacks->write(&placements[i].rotation2.z,2,1,handle);
        callbacks->write(&placements[i].rotation2.padding,2,1,handle);

        callbacks->write(&placements[i].modelNumber,2,1,handle);
        callbacks->write(&placements[i].unk,2,1,handle);
    }
    return 0;
};

RandomModelPlacement RandomModelPlacements::getPlacement(int idx)
{
    if(idx >= 0 && idx < numPlacements)
    return placements[idx];
    return RandomModelPlacement();
};

void RandomModelPlacements::setPlacement(int idx,RandomModelPlacement placement)
{
    if(idx >= 0 && idx < numPlacements)
    placements[idx] = placement;
};

void RandomModelPlacements::removePlacement(int idx)
{
    if(idx >= 0 && idx < numPlacements)
    {
        RandomModelPlacement* temp = NULL;
        if(numPlacements > 1)
        {
            temp = new RandomModelPlacement[numPlacements-1];
            memcpy(temp,placements,idx*sizeof(RandomModelPlacement));
            if(idx+1 < numPlacements)
            memcpy(&temp[idx],&placements[idx+1],(numPlacements-idx-1)*sizeof(RandomModelPlacement));
            delete[] placements;
        }
        placements = temp;
        numPlacements--;
    }
};

void RandomModelPlacements::addPlacement(RandomModelPlacement placement)
{
    RandomModelPlacement* temp = new RandomModelPlacement[numPlacements+1];
    if(numPlacements > 0)
    {
        memcpy(temp,placements,numPlacements*sizeof(RandomModelPlacement));
        delete[] placements;
    }
    placements = temp;
    placements[numPlacements] = placement;
    numPlacements++;
};
