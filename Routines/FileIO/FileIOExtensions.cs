using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DriverLevelEditor.Driver.FileIO
{
    static class BinaryStreamExtensions
    {
        public static void Seek(this BinaryStream b, long offset, SeekOrigin origin)
        {
            if (b.Stream.CanSeek)
                b.Stream.Seek(offset, origin);
            else
                throw new Exception("Stream invalid, cannot perform seek operation.");
        }

        /// <summary>Returns a formatted string describing where the cursor is in the file</summary>
        public static string Tell(this BinaryStream b)
        {
            return String.Format("Cursor position: 0x{0:X}", b.Stream.Position);
        }
    }

    static class BinaryStreamReaderExtensions
    {
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
}
