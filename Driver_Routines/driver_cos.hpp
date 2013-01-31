#ifndef DRIVER_COSMETICS_HPP
#define DRIVER_COSMETICS_HPP
#include "../vector.hpp"
#include "ioFuncs.hpp"
#include <cstdio>

class DriverCosmetics
{
    public:
        enum LeftOrRight : unsigned int {RIGHT = 0, LEFT = 1};
        enum FrontOrBack : unsigned int {FRONT = 0, BACK = 1};

        DriverCosmetics();

        int loadFromFile(FILE* file);
        int loadFromFile(const char* filename);
        int load(IOHandle handle, IOCallbacks* callbacks);

        int writeToFile(FILE* file);
        int writeToFile(const char* filename);
        int write(IOHandle handle, IOCallbacks* callbacks);

        unsigned int getRequiredSize();

        Vector4f getHeadlightPosition(LeftOrRight leftOrRight);
        Vector4f getSignalLightPosition(LeftOrRight leftOrRight,FrontOrBack frontOrBack);
        Vector4f getTailLightPosition(LeftOrRight leftOrRight);
        Vector4f getReverseLightPosition();
        Vector4f getSirenLightPosition(LeftOrRight leftOrRight);
        Vector4f getExhaustSmokePosition();
        Vector4f getEngineSmokePosition();
        Vector4f getEngineFirePosition();
        Vector3s getWheelPosition(LeftOrRight leftOrRight,FrontOrBack frontOrBack);
        short getSpeedModifier();
        unsigned short getLightStyleBitfield();
        bool hasDoubleTailLights();
        bool hasDoubleHeadlights();
        bool hasVerticalTailLights();
        bool hasVerticalHeadlights();
        int getTailLightSeparation();
        int getHeadlightSeparation();
        Vector3s getCollisionPoint(int whatami);
        Vector3s getBoundingBoxSize();

        void setHeadlightPosition(LeftOrRight leftOrRight,Vector4f position);
        void setSignalLightPosition(LeftOrRight leftOrRight,FrontOrBack frontOrBack, Vector4f position);
        void setTailLightPosition(LeftOrRight leftOrRight, Vector4f position);
        void setReverseLightPosition(Vector4f position);
        void setSirenLightPosition(LeftOrRight leftOrRight,Vector4f position);
        void setExhaustSmokePosition(Vector4f position);
        void setEngineSmokePosition(Vector4f position);
        void setEngineFirePosition(Vector4f position);
        void setWheelPosition(LeftOrRight leftOrRight,FrontOrBack frontOrBack, Vector3s position);
        void setSpeedModifier(short speed);
        void setLightStyleBitfield(unsigned short val);
        void useDoubleTailLights(bool yesNo);
        void useDoubleHeadlights(bool yesNo);
        void useVerticalTailLights(bool yesNo);
        void useVerticalHeadlights(bool yesNo);
        void setTailLightSeparation(int sep);
        void setHeadlightSeparation(int sep);
        void setCollisionPoint(int whatami, Vector3s position);
        void setBoundingBoxSize(Vector3s size);

    protected:
        Vector4f headlights[2];
        Vector4f signalLights[4];
        Vector4f tailLights[2];
        Vector4f reverseLights;
        Vector4f sirenLights[2];
        Vector4f exhaustSmoke;
        Vector4f engineSmoke;
        Vector4f engineFire;
        Vector3s wheels[4];
        short speedModifier;
        unsigned short lightStyle;
        Vector3s collisionPoints[12];
        Vector3s boundingBoxSize;
};

class CosmeticsContainer
{
    public:
        CosmeticsContainer(unsigned int size);
        ~CosmeticsContainer();
        int loadCosmeticsFromFile(FILE* file);
        int loadCosmeticsFromFile(const char* filename);
        int loadCosmetics(IOHandle handle, IOCallbacks* callbacks);

        int writeCosmeticsToFile(FILE* file);
        int writeCosmeticsToFile(const char* filename);
        int writeCosmetics(IOHandle handle, IOCallbacks* callbacks);

        unsigned int getRequiredSize();
        DriverCosmetics* getEntry(unsigned int idx);
        DriverCosmetics& operator[](unsigned int index);

    protected:
        unsigned int numEntries;
        DriverCosmetics* cosmetics;
};

#endif
