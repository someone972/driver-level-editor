using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DriverLevelEditor.Driver
{
    class DriverEventArgs : EventArgs
    {
        public string Message { get; set; }

        public DriverEventArgs(string s)
        {
            Message = s;
        }
    }

    interface IDriverLevel
    {
        event EventHandler<DriverEventArgs> LevelDestroyed;
        event EventHandler<DriverEventArgs> LevelReset;
        event EventHandler<DriverEventArgs> LevelOpened;
        event EventHandler<DriverEventArgs> LevelSaved;
    }

    class Level : IDisposable
    {
        public event EventHandler<DriverEventArgs> LevelDestroyed;
        public event EventHandler<DriverEventArgs> LevelReset;
        public event EventHandler<DriverEventArgs> LevelOpened;
        public event EventHandler<DriverEventArgs> LevelSaved;

        protected virtual void OnLevelDestroyed(DriverEventArgs e)
        {
            EventHandler<DriverEventArgs> handler = LevelDestroyed;

            if (handler != null)
            {
                e.Message += String.Format(" destroyed at {0}", DateTime.Now.ToString());
                handler(this, e);
            }
        }

        protected virtual void OnLevelReset(DriverEventArgs e)
        {
            EventHandler<DriverEventArgs> handler = LevelReset;
         
            if (handler != null)
            {
                e.Message += String.Format(" reset at {0}", DateTime.Now.ToString());
                handler(this, e);
            }
        }

        protected virtual void OnLevelOpened(DriverEventArgs e)
        {
            EventHandler<DriverEventArgs> handler = LevelOpened;

            if (handler != null)
            {
                e.Message += String.Format(" opened at {0}", DateTime.Now.ToString());
                handler(this, e);
            }
        }

        protected virtual void OnLevelSaved(DriverEventArgs e)
        {
            EventHandler<DriverEventArgs> handler = LevelSaved;

            if (handler != null)
            {
                e.Message += String.Format(" saved at {0}", DateTime.Now.ToString());
                handler(this, e);
            }
        }

        public void Dispose()
        {
            OnLevelDestroyed(new DriverEventArgs("SomeLevel.lev"));
        }

        public const uint LEV_TEXTURES                = 0x1;
        public const uint LEV_MODELS                  = 0x2;
        public const uint LEV_WORLD                   = 0x4;
        public const uint LEV_RANDOM_MODEL_PLACEMENT  = 0x10;
        public const uint LEV_TEXTURE_DEFINITIONS     = 0x20;
        public const uint LEV_ROAD_TABLE              = 0x80;
        public const uint LEV_ROAD_CONNECTIONS        = 0x100;
        public const uint LEV_INTERSECTIONS           = 0x200;
        public const uint LEV_HEIGHTMAP_TILES         = 0x400;
        public const uint LEV_HEIGHTMAP               = 0x800;
        public const uint LEV_MODEL_NAMES             = 0x1000;
        public const uint LEV_EVENT_MODELS            = 0x2000;
        public const uint LEV_VISIBILITY              = 0x4000;
        public const uint LEV_SECTOR_TEXTURE_USAGE    = 0x8000;
        public const uint LEV_ROAD_SECTIONS           = 0x10000;
        public const uint LEV_INTERSECTION_POSITIONS  = 0x20000;
        public const uint LEV_LAMPS                   = 0x80000;
        public const uint LEV_CHAIR_PLACEMENT         = 0x100000;
        public const uint LEV_ALL                     = 0xFFFFFFFF;

        public const uint LEV_TRAFFIC = LEV_ROAD_CONNECTIONS | LEV_ROAD_SECTIONS | LEV_INTERSECTIONS | LEV_INTERSECTION_POSITIONS;

        public const uint BLOCK_TEXTURES = 0;
        public const uint BLOCK_MODELS = 1;
        public const uint BLOCK_WORLD = 2;
        public const uint BLOCK_RANDOM_MODEL_PLACEMENT = 4;
        public const uint BLOCK_TEXTURE_DEFINITIONS = 5;
        public const uint BLOCK_ROAD_TABLE = 7;
        public const uint BLOCK_ROAD_CONNECTIONS = 8;
        public const uint BLOCK_INTERSECTIONS = 9;
        public const uint BLOCK_HEIGHTMAP_TILES = 10;
        public const uint BLOCK_HEIGHTMAP = 11;
        public const uint BLOCK_MODELS_NAMES = 12;
        public const uint BLOCK_EVENT_MODELS = 13;
        public const uint BLOCK_VISIBILITY = 14;
        public const uint BLOCK_SECTOR_TEXTURE_USAGE = 15;
        public const uint BLOCK_ROAD_SECTIONS = 16;
        public const uint BLOCK_INTERSECTION_POISITIONS = 17;
        public const uint BLOCK_LAMPS = 19;
        public const uint BLOCK_CHAIR_PLACEMENT = 20;
        public const uint NUMBER_OF_BLOCKS = 21;

        protected int openBlocks { get; set; }

        public Level()
        {
            openBlocks = 0;
        }

        public void OpenLevel()
        {
            OnLevelOpened(new DriverEventArgs("SomeLevel.lev"));
        }
    }
}
