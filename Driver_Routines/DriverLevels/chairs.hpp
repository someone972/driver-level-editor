#ifndef CHAIRS_HPP
#define CHAIRS_HPP

#include <cstdio>
#include <cstring>
#include "../ioFuncs.hpp"
#include "../../vector.hpp"
#include "../../Log_Routines/debug_logger.hpp"

class DriverChair
{
    public:
        DriverChair();
        int x,z;
        int inUse;
        Vector3s rotation;
};

class DriverChairList
{
    public:
        DriverChairList();
        ~DriverChairList();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumChairs();
        int getVisibilityTileIndex();

    protected:
        int numChairs;
        int visTileIndex;
        DriverChair* chairs;
};

class DriverChairs
{
    public:
        DriverChairs();
        ~DriverChairs();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

    protected:
        char id[4];
        int version;
        int mapWidth;
        int mapHeight;
        int numChairLists;
        DriverChairList* lists;
        int listLookupLength;
        DriverChairList** listLookup;
};

#endif
