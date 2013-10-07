#ifndef DRIVER_LEVELS_FINAL_HPP
#define DRIVER_LEVELS_FINAL_HPP

#include <cstring>
#include <cstdio>
#include "../Log_Routines/debug_logger.hpp"
#include "../EventMgr.hpp"

#include "DriverLevels/textures.hpp"
#include "DriverLevels/roads.hpp"
#include "DriverLevels/models.hpp"
#include "DriverLevels/lamps.hpp"
#include "DriverLevels/chairs.hpp"
#include "DriverLevels/heightmaps.hpp"
#include "DriverLevels/randomModelPlacement.hpp"
#include "DriverLevels/World.hpp"

const unsigned int LEV_TEXTURES               = 0x00000001;
const unsigned int LEV_MODELS                 = 0x00000002;
const unsigned int LEV_WORLD                  = 0x00000004;
const unsigned int LEV_RANDOM_MODEL_PLACEMENT = 0x00000010;
const unsigned int LEV_TEXTURE_DEFINITIONS    = 0x00000020;
const unsigned int LEV_ROAD_TABLE             = 0x00000080;
const unsigned int LEV_ROAD_CONNECTIONS       = 0x00000100;
const unsigned int LEV_INTERSECTIONS          = 0x00000200;
const unsigned int LEV_HEIGHTMAP_TILES        = 0x00000400;
const unsigned int LEV_HEIGHTMAP              = 0x00000800;
const unsigned int LEV_MODEL_NAMES            = 0x00001000;
const unsigned int LEV_EVENT_MODELS           = 0x00002000;
const unsigned int LEV_VISIBILITY             = 0x00004000;
const unsigned int LEV_SECTOR_TEXTURE_USAGE   = 0x00008000;
const unsigned int LEV_ROAD_SECTIONS          = 0x00010000;
const unsigned int LEV_INTERSECTION_POSITIONS = 0x00020000;
const unsigned int LEV_LAMPS                  = 0x00080000;
const unsigned int LEV_CHAIR_PLACEMENT        = 0x00100000;
const unsigned int LEV_ALL                    = 0xFFFFFFFF;

//convienience loaders
//TODO: finish the convienience bitfields and add more.
const unsigned int LEV_TRAFFIC = LEV_ROAD_CONNECTIONS|LEV_ROAD_SECTIONS|LEV_INTERSECTIONS|LEV_INTERSECTION_POSITIONS;

//Blocks 3,6,18 are unused by the game.                    Loading  Saving
const unsigned int BLOCK_TEXTURES = 0;                  // Yes      Yes
const unsigned int BLOCK_MODELS = 1;                    // Yes      Yes
const unsigned int BLOCK_WORLD = 2;                     // Yes      Yes
const unsigned int BLOCK_RANDOM_MODEL_PLACEMENT = 4;    // Yes      Yes
const unsigned int BLOCK_TEXTURE_DEFINITIONS = 5;       // Yes      Yes
const unsigned int BLOCK_ROAD_TABLE = 7;                // Yes      Yes
const unsigned int BLOCK_ROAD_CONNECTIONS = 8;          // Yes      Yes
const unsigned int BLOCK_INTERSECTIONS = 9;             // Yes      Yes
const unsigned int BLOCK_HEIGHTMAP_TILES = 10;          // Yes      Yes
const unsigned int BLOCK_HEIGHTMAP = 11;                // Yes      Yes
const unsigned int BLOCK_MODEL_NAMES = 12;              // Yes      Yes
const unsigned int BLOCK_EVENT_MODELS = 13;             // Yes      Yes
const unsigned int BLOCK_VISIBILITY = 14;               // Yes      Yes
const unsigned int BLOCK_SECTOR_TEXTURE_USAGE = 15;     // Yes      Yes
const unsigned int BLOCK_ROAD_SECTIONS = 16;            // Yes      Yes
const unsigned int BLOCK_INTERSECTION_POSITIONS = 17;   // Yes      Yes
const unsigned int BLOCK_LAMPS = 19;                    // Yes      Yes
const unsigned int BLOCK_CHAIR_PLACEMENT = 20;          // Yes      Yes.
const unsigned int NUMBER_OF_BLOCKS = 21;

class IDriverLevelEvents
{
        DECLARE_EVENTS(IDriverLevelEvents);
    public:
        DEFINE_EVENT0(IDriverLevelEvents, levelDestroyed);
        DEFINE_EVENT1(IDriverLevelEvents, levelReset, bool /*aboutToBe*/);
        DEFINE_EVENT0(IDriverLevelEvents, levelOpened);
        DEFINE_EVENT1(IDriverLevelEvents, levelSaved, bool /*aboutToBe*/);
};
IMPLEMENT_EVENTS(IDriverLevelEvents);

class DriverLevel
{
    public:
        DriverLevel();
        ~DriverLevel();
        void cleanup();

        void setLogPriorities(int levels[NUMBER_OF_BLOCKS]);

        void registerEventHandler(IDriverLevelEvents* handler);
        void unregisterEventHandler(IDriverLevelEvents* handler);

        int loadFromFile(const char* filename,unsigned int openWhat);
        int loadFromFile(FILE* file,unsigned int openWhat);
        int load(IOHandle handle, IOCallbacks* callbacks, unsigned int openWhat);

        int saveToFile(const char* filename, unsigned int saveWhat);
        int saveToFile(FILE* file, unsigned int saveWhat);
        int save(IOHandle handle, IOCallbacks* callbacks, unsigned int saveWhat);

        void setLogger(DebugLogger* newlog);

        DriverTextures textures;
        TextureDefinitions textureDefinitions;

        RandomModelPlacements randomPlacements;

        ModelNames modelNames;
        ModelContainer models;
        ModelContainer eventModels;

        RoadTables roadTables;
        RoadConnections roadConnections;
        RoadSections roadSections;
        Intersections intersections;
        IntersectionPositions intersectionPositions;

        DriverHeightmaps heightmaps;
        HeightmapTiles heightmapTiles;

        DriverWorld world;
        LevelVisibility visibility;
        SectorTextureUsage sectorTextures;

        DriverLamps lamps;

        DriverChairs chairs;
    protected:
        CEventMgr<IDriverLevelEvents> eventManager;
        unsigned int openBlocks;
        int priorities[NUMBER_OF_BLOCKS];
        DebugLogger dummy;
        DebugLogger* log;
};

#endif
