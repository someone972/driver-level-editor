#ifndef HEIGHTMAPS_HPP
#define HEIGHTMAPS_HPP

#include <cstdio>
#include <cstring>
#include "../ioFuncs.hpp"
#include "../../vector.hpp"
#include "../../Log_Routines/debug_logger.hpp"

class HeightmapTileFace
{
    public:
        HeightmapTileFace();
        Vector2i v1,v2,v3,v4;
        Vector4f normal;
        int numVerts;
};

class HeightmapTile
{
    public:
        HeightmapTile();
        ~HeightmapTile();
        void cleanup();
        int load(IOHandle handle, IOCallbacks* callbacks,DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumFaces();
        int getModelIndex();

        void setModelIndex(int index);

        unsigned short unk1,unk2;
    protected:
        unsigned short modelIdx;
        HeightmapTileFace* faces;
        unsigned short numFaces;
};

class HeightmapTiles
{
    public:
        HeightmapTiles();
        ~HeightmapTiles();
        void cleanup();
        int load(IOHandle handle, IOCallbacks* callbacks,int size,DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumTiles();
        HeightmapTile* getTile(int idx);

    protected:
        int numTiles;
        HeightmapTile* tiles;
};

class DriverHeightmap
{
    public:
        DriverHeightmap();
        ~DriverHeightmap();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

    protected:
        int compressedDataSize;
        unsigned char* compressedData;
};

class DriverHeightmaps
{
    public:
        DriverHeightmaps();
        ~DriverHeightmaps();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,int size,DebugLogger* log = NULL);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

    protected:
        int numHeightmaps;
        DriverHeightmap* heightmaps;
        int numTilesX;
        int numTilesZ;
        int numSectorsX;
        int numSectorsZ;
};

#endif
