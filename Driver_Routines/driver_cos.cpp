#include "driver_cos.hpp"

DriverCosmetics::DriverCosmetics()
{
    speedModifier = 7000;
    lightStyle = 0;
    //TODO: add default values here?
};

int DriverCosmetics::loadFromFile(const char* filename)
{
    FILE* file = fopen(filename,"rb");
    if(!file)
    return -1;
    return loadFromFile(file);
};

int DriverCosmetics::loadFromFile(FILE* file)
{
    return load((IOHandle)file, &fileCallbacks);
};

int DriverCosmetics::load(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle)
    return -1;

    for(int i = 0; i < 2; i++)
    {
        callbacks->read(&headlights[i].x,4,1,handle);
        callbacks->read(&headlights[i].y,4,1,handle);
        callbacks->read(&headlights[i].z,4,1,handle);
        callbacks->read(&headlights[i].w,4,1,handle);
    }

    for(int i = 0; i < 4; i++)
    {
        callbacks->read(&signalLights[i].x,4,1,handle);
        callbacks->read(&signalLights[i].y,4,1,handle);
        callbacks->read(&signalLights[i].z,4,1,handle);
        callbacks->read(&signalLights[i].w,4,1,handle);
    }

    for(int i = 0; i < 2; i++)
    {
        callbacks->read(&tailLights[i].x,4,1,handle);
        callbacks->read(&tailLights[i].y,4,1,handle);
        callbacks->read(&tailLights[i].z,4,1,handle);
        callbacks->read(&tailLights[i].w,4,1,handle);
    }

    callbacks->read(&reverseLights.x,4,1,handle);
    callbacks->read(&reverseLights.y,4,1,handle);
    callbacks->read(&reverseLights.z,4,1,handle);
    callbacks->read(&reverseLights.w,4,1,handle);

    for(int i = 0; i < 2; i++)
    {
        callbacks->read(&sirenLights[i].x,4,1,handle);
        callbacks->read(&sirenLights[i].y,4,1,handle);
        callbacks->read(&sirenLights[i].z,4,1,handle);
        callbacks->read(&sirenLights[i].w,4,1,handle);
    }

    callbacks->read(&exhaustSmoke.x,4,1,handle);
    callbacks->read(&exhaustSmoke.y,4,1,handle);
    callbacks->read(&exhaustSmoke.z,4,1,handle);
    callbacks->read(&exhaustSmoke.w,4,1,handle);

    callbacks->read(&engineSmoke.x,4,1,handle);
    callbacks->read(&engineSmoke.y,4,1,handle);
    callbacks->read(&engineSmoke.z,4,1,handle);
    callbacks->read(&engineSmoke.w,4,1,handle);

    callbacks->read(&engineFire.x,4,1,handle);
    callbacks->read(&engineFire.y,4,1,handle);
    callbacks->read(&engineFire.z,4,1,handle);
    callbacks->read(&engineFire.w,4,1,handle);

    for(int i = 0; i < 4; i++)
    {
        callbacks->read(&wheels[i].x,2,1,handle);
        callbacks->read(&wheels[i].y,2,1,handle);
        callbacks->read(&wheels[i].z,2,1,handle);
        callbacks->read(&wheels[i].padding,2,1,handle);
    }

    callbacks->read(&speedModifier,2,1,handle);
    callbacks->read(&lightStyle,2,1,handle);

    for(int i = 0; i < 12; i++)
    {
        callbacks->read(&collisionPoints[i].x,2,1,handle);
        callbacks->read(&collisionPoints[i].y,2,1,handle);
        callbacks->read(&collisionPoints[i].z,2,1,handle);
        callbacks->read(&collisionPoints[i].padding,2,1,handle);
    }

    callbacks->read(&boundingBoxSize.x,2,1,handle);
    callbacks->read(&boundingBoxSize.y,2,1,handle);
    callbacks->read(&boundingBoxSize.z,2,1,handle);
    callbacks->read(&boundingBoxSize.padding,2,1,handle);

    return getRequiredSize();
};

int DriverCosmetics::writeToFile(const char* filename)
{
    FILE* file = fopen(filename, "wb");
    if(!file)
    return -1;
    return writeToFile(file);
};

int DriverCosmetics::writeToFile(FILE* file)
{
    return write((IOHandle)file, &fileCallbacks);
};

int DriverCosmetics::write(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle || !callbacks->write)
    return -1;

    for(int i = 0; i < 2; i++)
    {
        callbacks->write(&headlights[i].x,4,1,handle);
        callbacks->write(&headlights[i].y,4,1,handle);
        callbacks->write(&headlights[i].z,4,1,handle);
        callbacks->write(&headlights[i].w,4,1,handle);
    }

    for(int i = 0; i < 4; i++)
    {
        callbacks->write(&signalLights[i].x,4,1,handle);
        callbacks->write(&signalLights[i].y,4,1,handle);
        callbacks->write(&signalLights[i].z,4,1,handle);
        callbacks->write(&signalLights[i].w,4,1,handle);
    }

    for(int i = 0; i < 2; i++)
    {
        callbacks->write(&tailLights[i].x,4,1,handle);
        callbacks->write(&tailLights[i].y,4,1,handle);
        callbacks->write(&tailLights[i].z,4,1,handle);
        callbacks->write(&tailLights[i].w,4,1,handle);
    }

    callbacks->write(&reverseLights.x,4,1,handle);
    callbacks->write(&reverseLights.y,4,1,handle);
    callbacks->write(&reverseLights.z,4,1,handle);
    callbacks->write(&reverseLights.w,4,1,handle);

    for(int i = 0; i < 2; i++)
    {
        callbacks->write(&sirenLights[i].x,4,1,handle);
        callbacks->write(&sirenLights[i].y,4,1,handle);
        callbacks->write(&sirenLights[i].z,4,1,handle);
        callbacks->write(&sirenLights[i].w,4,1,handle);
    }

    callbacks->write(&exhaustSmoke.x,4,1,handle);
    callbacks->write(&exhaustSmoke.y,4,1,handle);
    callbacks->write(&exhaustSmoke.z,4,1,handle);
    callbacks->write(&exhaustSmoke.w,4,1,handle);

    callbacks->write(&engineSmoke.x,4,1,handle);
    callbacks->write(&engineSmoke.y,4,1,handle);
    callbacks->write(&engineSmoke.z,4,1,handle);
    callbacks->write(&engineSmoke.w,4,1,handle);

    callbacks->write(&engineFire.x,4,1,handle);
    callbacks->write(&engineFire.y,4,1,handle);
    callbacks->write(&engineFire.z,4,1,handle);
    callbacks->write(&engineFire.w,4,1,handle);

    for(int i = 0; i < 4; i++)
    {
        callbacks->write(&wheels[i].x,2,1,handle);
        callbacks->write(&wheels[i].y,2,1,handle);
        callbacks->write(&wheels[i].z,2,1,handle);
        callbacks->write(&wheels[i].padding,2,1,handle);
    }

    callbacks->write(&speedModifier,2,1,handle);
    callbacks->write(&lightStyle,2,1,handle);

    for(int i = 0; i < 12; i++)
    {
        callbacks->write(&collisionPoints[i].x,2,1,handle);
        callbacks->write(&collisionPoints[i].y,2,1,handle);
        callbacks->write(&collisionPoints[i].z,2,1,handle);
        callbacks->write(&collisionPoints[i].padding,2,1,handle);
    }

    callbacks->write(&boundingBoxSize.x,2,1,handle);
    callbacks->write(&boundingBoxSize.y,2,1,handle);
    callbacks->write(&boundingBoxSize.z,2,1,handle);
    callbacks->write(&boundingBoxSize.padding,2,1,handle);

    return getRequiredSize();
};

unsigned int DriverCosmetics::getRequiredSize()
{
    return 0x16C;
};

int calculateSeparation(int separationVal)
{
    return separationVal*4+10;
};

Vector4f DriverCosmetics::getHeadlightPosition(LeftOrRight leftOrRight)
{
    return headlights[leftOrRight];
};

Vector4f DriverCosmetics::getSignalLightPosition(LeftOrRight leftOrRight,FrontOrBack frontOrBack)
{
    return signalLights[frontOrBack*2+leftOrRight];
};

Vector4f DriverCosmetics::getTailLightPosition(LeftOrRight leftOrRight)
{
    return tailLights[leftOrRight];
};

Vector4f DriverCosmetics::getReverseLightPosition()
{
    return reverseLights;
};

Vector4f DriverCosmetics::getSirenLightPosition(LeftOrRight leftOrRight)
{
    return sirenLights[leftOrRight];
};

Vector4f DriverCosmetics::getExhaustSmokePosition()
{
    return exhaustSmoke;
};

Vector4f DriverCosmetics::getEngineSmokePosition()
{
    return engineSmoke;
};

Vector4f DriverCosmetics::getEngineFirePosition()
{
    return engineFire;
};

Vector3s DriverCosmetics::getWheelPosition(LeftOrRight leftOrRight,FrontOrBack frontOrBack)
{
    return wheels[leftOrRight*2+frontOrBack];
};

short DriverCosmetics::getSpeedModifier()
{
    return speedModifier;
};

unsigned short DriverCosmetics::getLightStyleBitfield()
{
    return lightStyle;
};

bool DriverCosmetics::hasDoubleTailLights()
{
    return lightStyle&0x4000;
};

bool DriverCosmetics::hasDoubleHeadlights()
{
    return lightStyle&0x8000;
};

bool DriverCosmetics::hasVerticalTailLights()
{
    return lightStyle&0x1000;
};

bool DriverCosmetics::hasVerticalHeadlights()
{
    return lightStyle&0x2000;
};

int DriverCosmetics::getTailLightSeparation()
{
    return lightStyle&0x0300>>8;
};

int DriverCosmetics::getHeadlightSeparation()
{
    return lightStyle&0x0C00>>10;
};

Vector3s DriverCosmetics::getCollisionPoint(int whatami)
{
    return collisionPoints[whatami];
};

Vector3s DriverCosmetics::getBoundingBoxSize()
{
    return boundingBoxSize;
};


void DriverCosmetics::setHeadlightPosition(LeftOrRight leftOrRight,Vector4f position)
{
    headlights[leftOrRight] = position;
};

void DriverCosmetics::setSignalLightPosition(LeftOrRight leftOrRight,FrontOrBack frontOrBack, Vector4f position)
{
    signalLights[frontOrBack*2+leftOrRight] = position;
};

void DriverCosmetics::setTailLightPosition(LeftOrRight leftOrRight, Vector4f position)
{
    tailLights[leftOrRight] = position;
};

void DriverCosmetics::setReverseLightPosition(Vector4f position)
{
    reverseLights = position;
};

void DriverCosmetics::setSirenLightPosition(LeftOrRight leftOrRight,Vector4f position)
{
    sirenLights[leftOrRight] = position;
};

void DriverCosmetics::setExhaustSmokePosition(Vector4f position)
{
    exhaustSmoke = position;
};

void DriverCosmetics::setEngineSmokePosition(Vector4f position)
{
    engineSmoke = position;
};

void DriverCosmetics::setEngineFirePosition(Vector4f position)
{
    engineFire = position;
};

void DriverCosmetics::setWheelPosition(LeftOrRight leftOrRight,FrontOrBack frontOrBack, Vector3s position)
{
    wheels[leftOrRight*2+frontOrBack] = position;
};

void DriverCosmetics::setSpeedModifier(short speed)
{
    speedModifier = speed;
};

void DriverCosmetics::setLightStyleBitfield(unsigned short val)
{
    lightStyle = val;
};

void DriverCosmetics::useDoubleTailLights(bool yesNo)
{
    if(yesNo)
    lightStyle |= 0x4000;
    else lightStyle &= 0xBFFF;
};

void DriverCosmetics::useDoubleHeadlights(bool yesNo)
{
    if(yesNo)
    lightStyle |= 0x8000;
    else lightStyle &= 0x7FFF;
};

void DriverCosmetics::useVerticalTailLights(bool yesNo)
{
    if(yesNo)
    lightStyle |= 0x1000;
    else lightStyle &= 0xEFFF;
};

void DriverCosmetics::useVerticalHeadlights(bool yesNo)
{
    if(yesNo)
    lightStyle |= 0x2000;
    else lightStyle &= 0xDFFF;
};

void DriverCosmetics::setTailLightSeparation(int sep)
{
    lightStyle &= 0xFCFF;
    lightStyle += (sep&3)<<8;
};

void DriverCosmetics::setHeadlightSeparation(int sep)
{
    lightStyle &= 0xF3FF;
    lightStyle += (sep&3)<<10;
};

void DriverCosmetics::setCollisionPoint(int whatami,Vector3s position)
{
    collisionPoints[whatami] = position;
};

void DriverCosmetics::setBoundingBoxSize(Vector3s size)
{
    boundingBoxSize = size;
};


CosmeticsContainer::CosmeticsContainer(unsigned int size)
{
    numEntries = size;
    cosmetics = 0;
    if(numEntries != 0)
    cosmetics = new DriverCosmetics[numEntries];
};

CosmeticsContainer::~CosmeticsContainer()
{
    if(cosmetics)
    delete[] cosmetics;
};

int CosmeticsContainer::loadCosmeticsFromFile(const char* filename)
{
    FILE* file = fopen(filename,"rb");
    if(!file)
    return -1;
    return loadCosmeticsFromFile(file);
};

int CosmeticsContainer::loadCosmeticsFromFile(FILE* file)
{
    return loadCosmetics((IOHandle)file, &fileCallbacks);
};

int CosmeticsContainer::loadCosmetics(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle)
    return -1;

    int totalRead = 0;

    for(unsigned int i = 0; i < numEntries; i++)
    {
        totalRead += cosmetics[i].load(handle,callbacks);
    }
    return totalRead;
};

int CosmeticsContainer::writeCosmeticsToFile(const char* filename)
{
    FILE* file = fopen(filename, "wb");
    if(!file)
    return -1;
    return writeCosmeticsToFile(file);
};

int CosmeticsContainer::writeCosmeticsToFile(FILE* file)
{
    return writeCosmetics((IOHandle)file, &fileCallbacks);
};

int CosmeticsContainer::writeCosmetics(IOHandle handle, IOCallbacks* callbacks)
{
    if(!handle)
    return -1;

    int totalWritten = 0;

    for(unsigned int i = 0; i < numEntries; i++)
    {
        totalWritten = cosmetics[i].write(handle,callbacks);
    }
    return totalWritten;
};

unsigned int CosmeticsContainer::getRequiredSize()
{
    unsigned int buffersize = 0;
    for(unsigned int i = 0; i < numEntries; i++)
    {
        buffersize += cosmetics[i].getRequiredSize();
    }
    return buffersize;
};

DriverCosmetics* CosmeticsContainer::getEntry(unsigned int idx)
{
    if(cosmetics && idx >= 0 && idx < numEntries)
    return &cosmetics[idx];
    return NULL;
};

DriverCosmetics& CosmeticsContainer::operator[](unsigned int index)
{
    return cosmetics[index];
};

