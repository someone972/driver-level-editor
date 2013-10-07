using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using DriverLevelEditor.Driver.FileIO;

namespace DriverLevelEditor.Driver
{
    interface IWheelDefinitions
    {
        void Reset();

        string GetWheelName(int carnum, bool civcar, Wheels.WheelType wheelType);
        string GetHubcapName(int carnum);

        int SetWheelName(int carnum, bool civcar, Wheels.WheelType wheelType, string name);
        int SetHubcapName(int carnum, string name);

        //Not quite sure what's supposed to be going on here; Fixed arrays? Needs to be implemented
        //-- protected:
        //------ char playerData[0x900];
        //------ char civilianData[0x480];
    }

    class Wheels : IWheelDefinitions
    {
        public enum WheelType
        {
            Clean   = 0,
            Fast    = 1,
            Damaged = 2
        }

        // TODO: Driver::Wheels.Reset
        public void Reset()
        {
            return;
        }

        // TODO: Driver::Wheels.GetWheelName
        public string GetWheelName(int carnum, bool civcar, WheelType wheelType)
        {
            string name = "";

            switch (wheelType)
            {
                case WheelType.Clean:
                    name = "clean";
                    break;
                case WheelType.Fast:
                    name = "fast";
                    break;
                case WheelType.Damaged:
                    name = "damage";
                    break;
                default:
                    name = "error!";
                    break;
            }
            
            return name;
        }

        // TODO: Driver::Wheels.SetWheelName
        public int SetWheelName(int carnum, bool civcar, WheelType wheelType, string name)
        {
            switch (wheelType)
            {
                case WheelType.Clean:
                    return 0;
                case WheelType.Fast:
                    return 0;
                case WheelType.Damaged:
                    return 0;
                default:
                    return -1;
            }
        }

        // TODO: Driver::Wheels.GetHubCapName
        public string GetHubcapName(int carnum)
        {
            return "hubcap";
        }

        // TODO: Driver::Wheels.SetHubcapName
        public int SetHubcapName(int carnum, string name)
        {
            return 0;
        }
    }
}
