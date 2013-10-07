using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DriverLevelEditor.Driver.FileIO
{
    #region Extension Methods
    static class BinaryStreamExtensions
    {
        public static void Seek(this BinaryStream b, long offset, SeekOrigin origin)
        {
            if (b.Stream.CanSeek)
                b.Stream.Seek(offset, origin);
            else
                throw new Exception("Stream invalid, cannot perform seek operation.");
        }

        public static void Read(this BinaryStream b, byte[] buffer, int index, int count)
        {
            b.Reader.Read(buffer, index, count);
        }

        public static void Read(this BinaryStream b, char[] buffer, int index, int count)
        {
            b.Reader.Read(buffer, index, count);
        }

        public static byte ReadByte(this BinaryStream b)
        {
            return b.Reader.ReadByte();
        }

        public static byte[] ReadBytes(this BinaryStream b, int count)
        {
            return b.Reader.ReadBytes(count);
        }

        public static char ReadChar(this BinaryStream b)
        {
            return b.Reader.ReadChar();
        }

        public static char[] ReadChars(this BinaryStream b, int count)
        {
            return b.Reader.ReadChars(count);
        }

        public static short ReadInt16(this BinaryStream b)
        {
            return b.Reader.ReadInt16();
        }

        public static int ReadInt32(this BinaryStream b)
        {
            return b.Reader.ReadInt32();
        }

        public static long ReadInt64(this BinaryStream b)
        {
            return b.Reader.ReadInt64();
        }

        public static float ReadSingle(this BinaryStream b)
        {
            return b.Reader.ReadSingle();
        }

        public static double ReadDouble(this BinaryStream b)
        {
            return b.Reader.ReadDouble();
        }

        public static double ReadFloat(this BinaryStream b)
        {
            return Convert.ToDouble(b.Reader.ReadSingle());
        }

        /* --- Custom string reading functions --- */

        public static string ReadString(this BinaryStream b, int length)
        {
            return Encoding.UTF8.GetString(b.Reader.ReadBytes(length));
        }

        public static string ReadUnicodeString(this BinaryStream b, int length)
        {
            byte[] str = new byte[length];

            for (int s = 0; s < length; s++)
            {
                str[s] = b.Reader.ReadByte();

                if (b.Reader.ReadByte() != 0)
                    --b.Position;
            }

            return Encoding.UTF8.GetString(str);
        }
    }
    #endregion

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
