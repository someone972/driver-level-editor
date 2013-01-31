#ifndef RANDOM_MODEL_PLACEMENT
#define RANDOM_MODEL_PLACEMENT

#include <cstdio>
#include <cstring>
#include "../ioFuncs.hpp"
#include "../../vector.hpp"

class RandomModelPlacement
{
    public:
        RandomModelPlacement();
        Vector3f position;
        Vector3s rotation;
        Vector3f position2;
        Vector3s rotation2;
        short modelNumber;
        short unk;
};

class RandomModelPlacements
{
    public:
        RandomModelPlacements();
        ~RandomModelPlacements();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumPlacements();
        RandomModelPlacement getPlacement(int idx);
        void setPlacement(int idx,RandomModelPlacement placement);
        void removePlacement(int idx);
        void addPlacement(RandomModelPlacement placement);
    protected:
        int numPlacements;
        RandomModelPlacement* placements;
};

#endif
