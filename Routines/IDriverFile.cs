using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using DriverLevelEditor.Driver.FileIO;

namespace DriverLevelEditor.Routines
{
    interface IDriverFile
    {
        int LoadFromFile(string filename);
        int LoadFromFile(FileStream file);
        int Load(BinaryStream bstream);

        int SaveToFile(string filename);
        int SaveToFile(FileStream file);
        int Save(BinaryStream bstream);
    }
}
