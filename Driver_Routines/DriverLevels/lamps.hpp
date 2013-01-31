#ifndef LAMPS_HPP
#define LAMPS_HPP

#include <cstdio>
#include <cstring>
#include "../ioFuncs.hpp"
#include "../../vector.hpp"

const int LAMP_NORMAL = 0;
const int LAMP_FLICKER = 1;
const int LAMP_STROBE = 2;

class DriverLamp
{
    public:
        DriverLamp();
        int type;
        float radius;
        Vector3f position;
        float unk1;
        int r,g,b;
        int unk2;
};

class DriverLampList
{
    public:
        DriverLampList();
        ~DriverLampList();
        void cleanup();
        int load(IOHandle handle, IOCallbacks* callbacks);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumLamps();
        int getVisibilityTileIndex();
    protected:
        int numLamps;
        int visTileIndex;
        DriverLamp* lamps;
};

class DriverLamps
{
    public:
        DriverLamps();
        ~DriverLamps();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

    protected:
        char id[4];
        float version; //This is not used in the exe, we assume it is correct
        int mapWidth;
        int mapHeight;
        int numLampLists;
        DriverLampList* lists;
        int listLookupLength;
        DriverLampList** listLookup;
};

#endif
