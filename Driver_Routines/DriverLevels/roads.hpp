#ifndef INTERSECTIONS_HPP
#define INTERSECTIONS_HPP

#include <iostream>
#include <cstdio>
#include <cstring>
#include "../../vector.hpp"
#include "../ioFuncs.hpp"

class RoadTable
{
    public:
        RoadTable();
        ~RoadTable();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

    protected:
        int compressedDataSize;
        unsigned char* compressedData;
};

class RoadTables
{
    public:
        RoadTables();
        ~RoadTables();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

    protected:
        int numRoadTables;
        RoadTable* roadTables;
        int numTilesX;
        int numTilesZ;
        int numSectorsX;
        int numSectorsZ;
};

class RoadConnection
{
    public:
        RoadConnection();
        short roadIndex;
        short startIntersectionIdx;
        short endIntersectionIdx;
        char unk1;
        char unk2;
        short unk3;
        short unk4; //padding?
        int unk5[3*4];
        char unk6;
        char unk7;
        short originTileX;//maybe
        short originTileZ;//maybe
};

class RoadConnections
{
    public:
        RoadConnections();
        ~RoadConnections();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

    protected:
        int numRoads;
        RoadConnection* roadConnections;
};

class RoadSection
{
    public:
        RoadSection();
        int tileStartX;
        int tileStartZ;
        int tileEndX;
        int tileEndZ;
        int direction;
};

class RoadSections
{
    public:
        RoadSections();
        ~RoadSections();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumRoadSections();
    protected:
        int numRoadSections;
        RoadSection* roadSections;
};

class IntersectionConnection
{
    public:
        IntersectionConnection();
        short intersectionIndex; //intersection that is on other side of road
        short roadIndex;
        short unk1;
        short unk2;
};

class Intersection
{
    public:
        Intersection();
        unsigned short intersectionIndex;
        unsigned char unk1;
        unsigned char unk2;
        unsigned int unk3;
        IntersectionConnection connections[4];
        short originTileX;
        short originTileZ;
};

class Intersections
{
    public:
        Intersections();
        ~Intersections();
        void cleanup();

        int load(IOHandle handle, IOCallbacks* callbacks,int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        int getNumIntersections();
    protected:
        int numIntersections;
        Intersection* intersections;
};

class IntersectionPositions
{
    public:
        IntersectionPositions();
        ~IntersectionPositions();
        void cleanup();
        int load(IOHandle handle, IOCallbacks* callbacks,int size);

        unsigned int getRequiredSize();
        int save(IOHandle handle, IOCallbacks* callbacks);

        Vector2f getPosition(int idx);
        void setPosition(int idx,Vector2f pos);
        void insertPosition(int idx,Vector2f pos,bool before = true);
        int addPosition(Vector2f pos);
        int getNumIntersections();

    protected:
        int numIntersections;
        Vector2f* positions;
};

#endif
