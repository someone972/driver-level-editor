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
        class BlockTypes
        {
            class SectorTextureUsageBlock
            {
                class SectorTextureList
                {
                    public byte ID { get; set; }
                }

                public SectorTextureList TextureList { get; set; }
            }

            class HeightmapBlock
            {
                class HeightmapTableData
                {
                    public uint NumSectorsWide { get; set; }
                    public uint NumSectorsTall { get; set; }

                    public uint[] OffsetTable { get; set; }
                    public uint[] Data { get; set; }
                }

                public uint NumTilesWide { get; set; }
                public uint NumTilesTall { get; set; }

                public HeightmapTableData HeightmapTable { get; set; }
            }

            class HeightmapTilesBlock
            {
                class HeightmapTileFace
                {
                    class FaceVertex
                    {
                        public int X { get; set; }
                        public int Y { get; set; }
                    }

                    class FaceNormal
                    {
                        public float X { get; set; }
                        public float Y { get; set; }
                        public float Z { get; set; }
                        public float W { get; set; }
                    }

                    public List<FaceVertex> Vertices { get; set; }

                    public FaceNormal Normal { get; set; }

                    public uint NumVerts { get; set; }

                }

                class HeightmapTile
                {
                    public ushort ModelIndex { get; set; }
                    public uint UnknownData { get; set; }
                    public ushort NumFaces { get; set; }

                    public List<HeightmapTileFace> Faces { get; set; }

                }

                public uint NumTiles { get; set; }

                public List<HeightmapTile> Tiles { get; set; }

            }

            class IntersectionsBlock
            {
                class IntersectionConnection
                {
                    public short IntersectionIndex { get; set; }
                    public short RoadIndex { get; set; }

                    public ushort TrafficControlBitfield { get; set; }
                }

                class Intersection
                {
                    public ushort Index { get; set; }

                    public byte HasTrafficLights { get; set; }
                    public byte IsJunction { get; set; }

                    public uint UnknownData { get; set; }

                    public List<IntersectionConnection> IntersectionConnections { get; set; }

                    public short OriginTileX { get; set; }
                    public short OriginTileZ { get; set; }
                }

                public uint NumIntersections { get; set; }

                public List<Intersection> Intersections { get; set; }
            }

            class IntersectionPositionsBlock
            {
                class Position
                {
                    public int X { get; set; }
                    public int Y { get; set; }
                }

                public uint NumIntersections { get; set; }

                public List<Position> IntersectionPositions { get; set; }
            }

            class RoadTableBlock
            {
                class RoadTableData
                {
                    public uint NumSectorsWide { get; set; }
                    public uint NumSectorsTall { get; set; }

                    public uint[] OffsetTable { get; set; }
                    public ushort[] Data { get; set; }
                }

                public uint NumTilesWide { get; set; }
                public uint NumTilesTall { get; set; }

                public RoadTableData RoadTable { get; set; }
            }

            class RoadConnectionsBlock
            {
                class RoadConnection
                {
                    public ushort RoadIndex { get; set; }

                    public ushort StartIntersectionIndex { get; set; }
                    public ushort EndIntersectionIndex { get; set; }

                    public byte NumLanes { get; set; }
                    public byte LaneDirectionBitfield { get; set; }

                    public ushort RoadLength { get; set; }

                    public byte RoadFlags { get; set; }
                    public byte RoadDirection { get; set; }

                    public short OriginTileX { get; set; }
                    public short OriginTileZ { get; set; }
                }

                public uint NumRoads { get; set; }

                public List<RoadConnection> RoadConnections { get; set; }
            }

            class RoadSectionsBlock
            {
                class RoadSection
                {
                    public int StartTileX { get; set; }
                    public int StartTileZ { get; set; }

                    public int EndTileX { get; set; }
                    public int EndTileZ { get; set; }

                    public byte Direction { get; set; }
                }

                public uint NumRoads { get; set; }

                List<RoadSection> RoadSections { get; set; }
            }

            class TextureAtlasInfo
            {
                class TextureAtlasDefinition
                {
                    public string Name { get; set; }

                    public byte X { get; set; }
                    public byte Y { get; set; }
                    public byte W { get; set; }
                    public byte H { get; set; }
                }

                public uint NumDefs { get; set; }

                public List<TextureAtlasDefinition> Definitions { get; set; }
            }

            class ChairPlacementBlock
            {
                class ChairList
                {
                    public uint VisTileIndex { get; set; }
                    public uint NumChairs { get; set; }

                    public List<Chair> Chairs { get; set; }
                }

                class Chair
                {
                    public int X { get; set; }
                    public int Z { get; set; }

                    public int InUse { get; set; }

                    public short RotX { get; set; }
                    public short RotY { get; set; }
                    public short RotZ { get; set; }
                }

                public const uint Magic = 0x52484347; //GCHR
                public const uint Version = 0x2;

                public uint NumVisTilesWide { get; set; }
                public uint NumVisTilesTall { get; set; }
                public uint NumChairLists { get; set; }

                public List<ChairList> ChairLists { get; set; }   
            }

            class LampBlock
            {
                class LampList
                {
                    public uint VisTileIndex { get; set; }
                    public uint NumLamps { get; set; }

                    public List<Lamp> Lamps { get; set; }
                }

                class Lamp
                {
                    enum LampTypes
                    {
                        Normal = 0,
                        Flicker = 1,
                        Strobe = 2
                    }

                    public LampTypes Type { get; set; }

                    public float Radius { get; set; }

                    public float X { get; set; }
                    public float Y { get; set; }
                    public float Z { get; set; }

                    public uint R { get; set; }
                    public uint G { get; set; }
                    public uint B { get; set; }
                }

                public const uint Magic = 0x504D4C47; //GLMP

                public float Version { get; set; }

                public uint NumVisTilesWide { get; set; }
                public uint NumVisTilesTall { get; set; }
                public uint NumLampLights { get; set; }

                public List<LampList> LampLists { get; set; }
            }

            class BlockData
            {
                public uint Type { get; set; }
                public uint Size { get; set; }
            }

            public uint NumBlocks { get; set; }
            public List<BlockData> Blocks { get; set; }
        }

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
