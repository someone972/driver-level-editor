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
            Textures                = 0x1,
            Models                  = 0x2,
            World                   = 0x4,
            RandomModelPlacement    = 0x10,
            TextureDefinitions      = 0x20,
            RoadTable               = 0x80,
            RoadConnections         = 0x100,
            Intersections           = 0x200,
            HeightmapTiles          = 0x400,
            Heightmap               = 0x800,
            ModelNames              = 0x1000,
            EventModels             = 0x2000,
            Visibility              = 0x4000,
            SectorTextureUsage      = 0x8000,
            RoadSections            = 0x10000,
            IntersectionPositions   = 0x20000,
            Lamps                   = 0x80000,
            ChairPlacement          = 0x100000,
            All                     = 0xFFFFFFFF,

            Traffic                 = RoadConnections | RoadSections | Intersections | IntersectionPositions
        }

        public enum Blocks
        {
            Textures                = 0,
            Models                  = 1,
            World                   = 2,
            RandomModelPlacement    = 4,
            TextureDefinitions      = 5,
            RoadTable               = 7,
            RoadConnections         = 8,
            Intersections           = 9,
            HeightmapTiles          = 10,
            Heightmap               = 11,
            ModelNames              = 12,
            EventModels             = 13,
            Visibility              = 14,
            SectorTextureUsage      = 15,
            RoadSections            = 16,
            IntersectionPositions   = 17,
            Lamps                   = 19,
            ChairPlacement          = 20,
            NumberOfBlocks          = 21
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
