#include "driver_wdf.hpp"

DriverWheelDefinitions::DriverWheelDefinitions()
{
    reset();
};

void DriverWheelDefinitions::reset()
{
    memset(playerData,0,0x900);
    memset(civilianData,0,0x480);
};

int DriverWheelDefinitions::loadFromFile(const char* filename)
{
    FILE* file = fopen(filename,"rb");
    if(!file)
    return 1;
    int ret = loadFromFile(file);
    fclose(file);
    return ret;
};

int DriverWheelDefinitions::loadFromFile(FILE* file)
{
    if(!file)
        return 1;

    return load(&fileCallbacks, (IOHandle)file);
};

int DriverWheelDefinitions::load(IOCallbacks* callbacks, IOHandle handle)
{
    if(!callbacks || !handle)
        return 1;

    callbacks->read(playerData,0x900,1,handle);
    callbacks->read(civilianData,0x480,1,handle);
    return 0;
};

int DriverWheelDefinitions::saveToFile(const char* filename)
{
    FILE* file = fopen(filename,"wb");
    if(!file)
    return 1;
    int ret = saveToFile(file);
    fclose(file);
    return ret;
};

int DriverWheelDefinitions::saveToFile(FILE* file)
{
    if(!file)
        return 1;

    return save(&fileCallbacks, (IOHandle)file);
};

int DriverWheelDefinitions::save(IOCallbacks* callbacks, IOHandle handle)
{
    if(!callbacks || !handle)
        return 1;

    callbacks->write(playerData,0x900,1,handle);
    callbacks->write(civilianData,0x480,1,handle);
    return 0;
};

const char* DriverWheelDefinitions::getCleanWheelName(int carnum,bool civcar)
{
    if(civcar)
    {
        if(carnum >= 0 && carnum < 12)
        return civilianData+carnum*96;
    }
    else
    {
        if(carnum >= 0 && carnum < 18)
        return playerData+carnum*128;
    }
    return NULL;
};

const char* DriverWheelDefinitions::getFastWheelName(int carnum,bool civcar)
{
    if(civcar)
    {
        if(carnum >= 0 && carnum < 12)
        return civilianData+carnum*96+32;
    }
    else
    {
        if(carnum >= 0 && carnum < 18)
        return playerData+carnum*128+32;
    }
    return NULL;
};

const char* DriverWheelDefinitions::getDamagedWheelName(int carnum,bool civcar)
{
    if(civcar)
    {
        if(carnum >= 0 && carnum < 12)
        return civilianData+carnum*96+64;
    }
    else
    {
        if(carnum >= 0 && carnum < 18)
        return playerData+carnum*128+64;
    }
    return NULL;
};

const char* DriverWheelDefinitions::getHubcapName(int carnum)
{
    if(carnum >= 0 && carnum < 18)
    return playerData+carnum*128+96;
    return NULL;
};

void DriverWheelDefinitions::setCleanWheelName(int carnum,bool civcar,const char* name)
{
    if(name)
    {
        if(civcar)
        {
            if(carnum >= 0 && carnum < 12)
            strncpy(civilianData+carnum*96,name,31);
        }
        else
        {
            if(carnum >= 0 && carnum < 18)
            strncpy(playerData+carnum*128,name,31);
        }
    }
};

void DriverWheelDefinitions::setFastWheelName(int carnum,bool civcar,const char* name)
{
    if(name)
    {
        if(civcar)
        {
            if(carnum >= 0 && carnum < 12)
            strncpy(civilianData+carnum*96+32,name,31);
        }
        else
        {
            if(carnum >= 0 && carnum < 18)
            strncpy(playerData+carnum*128+32,name,31);
        }
    }
};

void DriverWheelDefinitions::setDamagedWheelName(int carnum,bool civcar,const char* name)
{
    if(name)
    {
        if(civcar)
        {
            if(carnum >= 0 && carnum < 12)
            strncpy(civilianData+carnum*96+64,name,31);
        }
        else
        {
            if(carnum >= 0 && carnum < 18)
            strncpy(playerData+carnum*128+64,name,31);
        }
    }
};

void DriverWheelDefinitions::setHubcapName(int carnum,const char* name)
{
    if(name)
    {
        if(carnum >= 0 && carnum < 18)
        strncpy(playerData+carnum*128+96,name,31);
    }
};
