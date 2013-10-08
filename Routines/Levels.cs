using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using DriverLevelEditor.Driver.FileIO;

namespace DriverLevelEditor.Driver
{
    interface IDriverLevel
    {
        void Cleanup();
    }

    class DriverEventArgs : EventArgs
    {
        public string Message { get; set; }

        public DriverEventArgs(string s)
        {
            Message = s;
        }
    }

    class Level : IDriverLevel, IDisposable
    {
        #region Level Events
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
#endregion

        public void Cleanup()
        {
            OnLevelReset(new DriverEventArgs("SomeLevel.lev"));
        }

        public void Dispose()
        {
            OnLevelDestroyed(new DriverEventArgs("SomeLevel.lev"));
        }

        public enum BlockLoadFlags
        {
            LEV_TEXTURES                = 0x1,
            LEV_MODELS                  = 0x2,
            LEV_WORLD                   = 0x4,
            LEV_RANDOM_MODEL_PLACEMENT  = 0x10,
            LEV_TEXTURE_DEFINITIONS     = 0x20,
            LEV_ROAD_TABLE              = 0x80,
            LEV_ROAD_CONNECTIONS        = 0x100,
            LEV_INTERSECTIONS           = 0x200,
            LEV_HEIGHTMAP_TILES         = 0x400,
            LEV_HEIGHTMAP               = 0x800,
            LEV_MODEL_NAMES             = 0x1000,
            LEV_EVENT_MODELS            = 0x2000,
            LEV_VISIBILITY              = 0x4000,
            LEV_SECTOR_TEXTURE_USAGE    = 0x8000,
            LEV_ROAD_SECTIONS           = 0x10000,
            LEV_INTERSECTION_POSITIONS  = 0x20000,
            LEV_LAMPS                   = 0x80000,
            LEV_CHAIR_PLACEMENT         = 0x100000,
            LEV_ALL                     = 0xFFFFFFFF,

            LEV_TRAFFIC = LEV_ROAD_CONNECTIONS | LEV_ROAD_SECTIONS | LEV_INTERSECTIONS | LEV_INTERSECTION_POSITIONS
        }

        public enum Blocks
        {
            Textures = 0,
            Models = 1,
            World = 2,
            RandomModelPlacement = 4,
            TextureDefinitions = 5,
            RoadTable = 7,
            RoadConnections = 8,
            Intersections = 9,
            HeightmapTiles = 10,
            Heightmap = 11,
            ModelNames = 12,
            EventModels = 13,
            Visibility = 14,
            SectorTextureUsage = 15,
            RoadSections = 16,
            IntersectionPositions = 17,
            Lamps = 19,
            ChairPlacement = 20,
            NumberOfBlocks = 21
        }

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
