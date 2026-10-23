// Full C# CRC regression: verify all 10,764 installed PACK members, no network.
using System;
using System.IO;
using System.Diagnostics;
using Jx3D.OfflineAssets;
public static class OfflinePakVerifyAll
{
    public static int Main(string[] args)
    {
        if (args.Length!=1)
        {
            Console.Error.WriteLine("Specify local Client-3D-2/update path");
            return 2;
        }
        try
        {
            string[] names={"script.v110.pak","settings.v149.pak","maps.v111.pak",
                            "UI.v102.pak","spr.v113.pak"};
            int total=0;
            Stopwatch timer=Stopwatch.StartNew();
            foreach(string filename in names)
            {
                int count=0;
                long expanded=0;
                using(PakFile pak=new PakFile(Path.Combine(args[0],"paks",filename)))
                {
                    foreach(PakEntry entry in pak.Entries)
                    {
                        byte[] data=pak.ReadBytes(entry.Name,3000000);
                        if ((ulong)data.Length!=entry.ExpandedBytes)
                            throw new InvalidDataException("Expanded length mismatch");
                        count++;
                        expanded+=data.LongLength;
                    }
                    if(count!=pak.Count)throw new InvalidDataException("Skipped member");
                }
                total+=count;
                Console.WriteLine("PASS "+filename+" "+count+" members, "+expanded+" uncompressed bytes");
            }
            timer.Stop();
            if(total!=10764)throw new InvalidDataException("Unexpected total count");
            Console.WriteLine("FULL_CSHARP_CRC_PASS "+total+" members, elapsed_seconds="+timer.Elapsed.TotalSeconds.ToString("F2"));
            return 0;
        }
        catch(Exception e)
        {
            Console.Error.WriteLine("CRC_FAIL "+e.Message);
            return 1;
        }
    }
}