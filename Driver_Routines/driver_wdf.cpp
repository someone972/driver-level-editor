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
    fread(playerData,0x900,1,file);
    fread(civilianData,0x480,1,file);
    return 0;
};

int DriverWheelDefinitions::loadFromMemory(const unsigned char* data)
{
    if(!data)
    return 0;
    memcpy(playerData,data,0x900);
    memcpy(civilianData,data+0x900,0x480);
    return 0x900+0x480;
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
    fwrite(playerData,0x900,1,file);
    fwrite(civilianData,0x480,1,file);
    return 0;
};

int DriverWheelDefinitions::saveToMemory(unsigned char* data)
{
    if(!data)
    return 0;
    memcpy(data,playerData,0x900);
    memcpy(data+0x900,civilianData,0x480);
    return 0x900+0x480;
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
