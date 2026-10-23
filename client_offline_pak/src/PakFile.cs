// Pure C# offline reader for Client-3D-2 PACK v1 files.
// No Unity dependencies, no network calls and no installed-file writes.
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Text;

namespace Jx3D.OfflineAssets
{
    public sealed class PakEntry
    {
        public readonly string Name;
        public readonly ulong Offset;
        public readonly ulong StoredBytes;
        public readonly ulong ExpandedBytes;
        public readonly uint Crc32;
        public readonly byte Compression;
        public PakEntry(string name, ulong offset, ulong stored, ulong expanded, uint crc, byte compression)
        {
            Name=name; Offset=offset; StoredBytes=stored; ExpandedBytes=expanded;
            Crc32=crc; Compression=compression;
        }
    }

    public sealed class PakFile : IDisposable
    {
        private readonly FileStream file;
        private readonly BinaryReader reader;
        private readonly Dictionary<string, PakEntry> items;
        private readonly object gate=new object();
        public int Count { get { return items.Count; } }

        public PakFile(string path)
        {
            if (!File.Exists(path)) throw new FileNotFoundException("PACK missing",path);
            file=new FileStream(path,FileMode.Open,FileAccess.Read,FileShare.Read);
            reader=new BinaryReader(file,Encoding.UTF8);
            items=new Dictionary<string,PakEntry>(StringComparer.OrdinalIgnoreCase);
            try { ReadIndex(); }
            catch { reader.Dispose();file.Dispose();throw; }
        }

        private void ReadIndex()
        {
            if (file.Length < 21) throw new InvalidDataException("Short PACK");
            byte[] magic=reader.ReadBytes(4);
            if (magic.Length!=4 || Encoding.ASCII.GetString(magic)!="PACK")
                throw new InvalidDataException("Bad PACK magic");
            uint version=reader.ReadUInt32();
            uint indexOffset=reader.ReadUInt32();
            uint unknown=reader.ReadUInt32();
            uint memberCount=reader.ReadUInt32();
            if (version!=1 || unknown!=0 || indexOffset<=20 || indexOffset>=file.Length)
                throw new InvalidDataException("Unsupported PACK header");
            if (memberCount==0 || memberCount>20000)
                throw new InvalidDataException("Bad PACK member count");
            file.Position=indexOffset;
            UTF8Encoding utf8=new UTF8Encoding(false,true);
            for (uint i=0;i<memberCount;i++)
            {
                uint nameLength=reader.ReadUInt32();
                if (nameLength<2 || nameLength>2048)
                    throw new InvalidDataException("Invalid indexed filename length");
                byte[] bytes=reader.ReadBytes((int)nameLength);
                if (bytes.Length!=nameLength) throw new EndOfStreamException();
                string name=utf8.GetString(bytes);
                if (!ValidName(name) || items.ContainsKey(name))
                    throw new InvalidDataException("Untrusted or duplicate PACK member name");
                ulong offset=reader.ReadUInt64();
                ulong stored=reader.ReadUInt64();
                ulong expanded=reader.ReadUInt64();
                uint crc=reader.ReadUInt32();
                byte compression=reader.ReadByte();
                if (offset<20 || offset>(ulong)indexOffset ||
                    stored>(ulong)indexOffset-offset ||
                    expanded>1024UL*1024UL*1024UL || compression>1)
                    throw new InvalidDataException("Invalid PACK member bounds");
                items.Add(name,new PakEntry(name,offset,stored,expanded,crc,compression));
            }
            if (file.Position!=file.Length)
                throw new InvalidDataException("Unexpected PACK index trailer");
        }

        private static bool ValidName(string s)
        {
            if (String.IsNullOrEmpty(s) || s[0]=='/' || s.IndexOf('\\')>=0)
                return false;
            string[] parts=s.Split('/');
            foreach(string p in parts)
                if (p.Length==0 || p=="." || p=="..") return false;
            return true;
        }

        public bool Contains(string name)
        {
            return ValidName(name) && items.ContainsKey(name);
        }

        public IEnumerable<PakEntry> Entries { get { return items.Values; } }

        // Explicit maxOut prevents untrusted PACK data exhausting RAM.
        public byte[] ReadBytes(string name, int maxOut)
        {
            if (!ValidName(name)) throw new ArgumentException("Unsafe resource name","name");
            PakEntry item;
            if (!items.TryGetValue(name,out item))
                throw new FileNotFoundException("PAK resource missing: "+name);
            if (maxOut<0 || item.ExpandedBytes>(ulong)maxOut ||
                item.StoredBytes>Int32.MaxValue || item.ExpandedBytes>Int32.MaxValue)
                throw new InvalidDataException("PACK member exceeds read limit");
            byte[] raw;
            lock(gate)
            {
                file.Position=(long)item.Offset;
                raw=reader.ReadBytes((int)item.StoredBytes);
            }
            if ((ulong)raw.Length!=item.StoredBytes)
                throw new EndOfStreamException("Truncated PACK member");
            byte[] result;
            if (item.Compression==0)
            {
                result=raw;
            }
            else
            {
                using (MemoryStream compressed=new MemoryStream(raw,false))
                using (DeflateStream inflate=new DeflateStream(compressed,CompressionMode.Decompress))
                using (MemoryStream outStream=new MemoryStream((int)item.ExpandedBytes))
                {
                    byte[] scratch=new byte[16384];
                    int n;
                    while ((n=inflate.Read(scratch,0,scratch.Length))>0)
                    {
                        if (outStream.Length+n>(long)item.ExpandedBytes)
                            throw new InvalidDataException("PACK member over-inflated");
                        outStream.Write(scratch,0,n);
                    }
                    result=outStream.ToArray();
                }
            }
            if ((ulong)result.Length!=item.ExpandedBytes ||
                Crc32.Calc(result)!=item.Crc32)
                throw new InvalidDataException("PACK member CRC/size mismatch: "+name);
            return result;
        }

        public void Dispose()
        {
            reader.Dispose();
        }
    }

    internal static class Crc32
    {
        private static readonly uint[] Table=MakeTable();
        private static uint[] MakeTable()
        {
            uint[] t=new uint[256];
            for (uint i=0;i<256;i++)
            {
                uint c=i;
                for (int k=0;k<8;k++)
                    c=(c&1)!=0 ? (0xEDB88320U^(c>>1)) : (c>>1);
                t[i]=c;
            }
            return t;
        }
        public static uint Calc(byte[] bytes)
        {
            uint c=0xFFFFFFFFU;
            foreach(byte value in bytes)
                c=Table[(c^value)&255]^(c>>8);
            return c^0xFFFFFFFFU;
        }
    }
}