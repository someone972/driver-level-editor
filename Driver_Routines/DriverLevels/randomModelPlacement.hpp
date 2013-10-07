#ifndef RANDOM_MODEL_PLACEMENT
#define RANDOM_MODEL_PLACEMENT

#include <cstdio>
#include <cstring>
#include "../ioFuncs.hpp"
#include "../../vector.hpp"
#include "../../Log_Routines/debug_logger.hpp"

//Important note: Random model placements aren't used anywhere in the current executable, but are still included in the level.
//  Perhaps they were used to create data included in the executable?

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

        int load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log = NULL);

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
