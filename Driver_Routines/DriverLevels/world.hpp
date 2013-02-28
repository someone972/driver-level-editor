#ifndef WORLD_HPP
#define WORLD_HPP

#include <cstdio>
#include <cstring>
#include "../ioFuncs.hpp"
#include "../../vector.hpp"
#include "../../Log_Routines/debug_logger.hpp"

class SectorTextureList
{
    public:
        SectorTextureList();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumTexturesUsed();
        int getTexture(int idx);
        void setTexture(int idx,int tex);
        void removeTexture(int idx);
        void addTexture(int tex);
    protected:
        int numTexturesUsed;
        unsigned char textures[70];
};

class SectorTextureUsage
{
    public:
        SectorTextureUsage();
        ~SectorTextureUsage();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        SectorTextureList* getTextureList(int sectorIdx);
    protected:
        SectorTextureList textureLists[1024];
};

class WorldModelDef
{
    public:
        WorldModelDef();
        int getModelIndex();
        void setModelIndex(int idx);

        short modelNum;
        Vector3f position;
        float rot;
};

class WorldSector
{
    public:
        WorldSector();
        ~WorldSector();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumModelDefs();
        WorldModelDef* getModelDef(int idx);

    protected:
        int numModelDefs;
        WorldModelDef* modelDefs;
        int numContentsEntries;
        unsigned int* contentsEntries;
        int contentsTableDataSize;
        unsigned char* contentsTableData;
};

class WorldHeader
{
    public:
        WorldHeader();
        int mapWidth;
        int mapHeight;
        int tileSize;
        int numSectors;
        int visTableWidth;
        int unk1;
        int unk2;
        float ambientLight;
        Vector3f sunDirection;
};

class DriverWorld
{
    public:
        DriverWorld();
        ~DriverWorld();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumBridgedDefs();
        WorldModelDef* getBridgedDef(int idx);
        int getNumSectors();
        WorldSector* getSector(int idx);

    protected:
        int numBridgedDefs;
        WorldModelDef* bridgedDefs;
        WorldHeader header;
        WorldSector* sectors;
};

class SectorVisibility
{
    public:
        SectorVisibility();
        ~SectorVisibility();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        void createEmptyData(); //needed to repair broken level part

    protected:
        int visibilityDataSize;
        unsigned char* visibilityData;
};

class LevelVisibility
{
    public:
        LevelVisibility();
        ~LevelVisibility();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks, int size, DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

    protected:
        int unk1;
        short numSectorsX;
        short numSectorsZ;
        short unk2;
        short unk3;
        int unk4;

        int numSectors;
        SectorVisibility* sectors;
};

#endif
