using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DriverLevelEditor.Driver
{
    interface IWheelDefinitions
    {
        void reset();

        int LoadFromFile(string filename);
        int LoadFromFile(File file);
        // int Load(IOCallbacks callbacks, IOHandle handle); //--Not sure how to implement this (from C version)

        int SaveToFile(string filename);
        int SaveToFile(File file);
        // int Save(IOCallbacks callbacks, IOHandle handle); //-- Not sure how to implement this (from C version)

        string GetCleanWheelName(int carnum, bool civcar);
        string GetFastWheelName(int carnum, bool civcar);
        string GetDamagedWheelName(int carnum, bool civcar);
        string GetHubcapName(int carnum);

        void SetCleanWheelName(int carnum, bool civcar, string name);
        void SetFastWheelName(int carnum, bool civcar, string name);
        void SetDamagedWheelName(int carnum, bool civcar, string name);
        void SetHubcapName(int carnum, string name);

        //Not quite sure what's supposed to be going on here; Fixed arrays? Needs to be implemented
        //-- protected:
        //------ char playerData[0x900];
        //------ char civilianData[0x480];
    }

    sealed class Wheels
    {
        // Needs to implement IWheelDefinitions
    }
}
