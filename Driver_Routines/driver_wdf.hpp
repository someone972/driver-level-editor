#ifndef DRIVER_WDF_HPP
#define DRIVER_WDF_HPP

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "ioFuncs.hpp"

class DriverWheelDefinitions
{
    public:
        DriverWheelDefinitions();
        void reset();

        int loadFromFile(const char* filename);
        int loadFromFile(FILE* file);
        int load(IOCallbacks* callbacks, IOHandle handle);

        int saveToFile(const char* filename);
        int saveToFile(FILE* file);
        int save(IOCallbacks* callbacks, IOHandle handle);

        const char* getCleanWheelName(int carnum,bool civcar);
        const char* getFastWheelName(int carnum,bool civcar);
        const char* getDamagedWheelName(int carnum,bool civcar);
        const char* getHubcapName(int carnum);

        void setCleanWheelName(int carnum,bool civcar,const char* name);
        void setFastWheelName(int carnum,bool civcar,const char* name);
        void setDamagedWheelName(int carnum,bool civcar,const char* name);
        void setHubcapName(int carnum,const char* name);

    protected:
        char playerData[0x900];
        char civilianData[0x480];
};

#endif
