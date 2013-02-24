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

int RandomModelPlacements::load(IOHandle handle, IOCallbacks* callbacks,int size)
{
    cleanup();
    if(!handle || !callbacks)
    return 1;

    numPlacements = size/44;
    placements = new RandomModelPlacement[numPlacements];

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
    }
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
