using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DriverLevelEditor.Driver.FileIO
{
    class BinaryStream : IDisposable
    {
        public FileStream Stream { get; private set; }
        public BinaryReader Reader { get; private set; }

        public string Filename { get; private set; }

        private bool CanDisposeStream = false;

        public void Dispose()
        {
            if (Reader != null)
                Reader.Dispose();

            if (CanDisposeStream)
                Stream.Dispose();
            else
                Stream = null;
        }

        public long Position
        {
            get { return Stream.Position; }
            set { Stream.Position = value; }
        }

        private void CreateStream(FileStream stream)
        {
            Stream = stream;
            Reader = new BinaryReader(Stream, Encoding.UTF8);
        }

        public BinaryStream(string filename) : this(filename, FileMode.Open, FileAccess.Read) { }
        public BinaryStream(string filename, FileMode mode) : this(filename, mode, FileAccess.Read) { }
        public BinaryStream(string filename, FileMode mode, FileAccess access)
        {
            Filename = filename;

            CreateStream(File.Open(Filename, mode, access));

            CanDisposeStream = true;
        }

        public BinaryStream(string filename, FileStream stream)
        {
            Filename = filename;

            CreateStream(stream);
        }
    }
}
