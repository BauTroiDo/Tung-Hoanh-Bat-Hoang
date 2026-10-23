// Standalone smoke+negative tests for PakFile.cs (no Unity, no network).
using System;
using System.IO;
using System.Collections.Generic;
using Jx3D.OfflineAssets;

public static class OfflinePakSmoke
{
    private static int passes=0;
    private static void Assert(bool value,string name)
    {
        if (!value) throw new Exception("TEST FAILED: "+name);
        passes++;
        Console.WriteLine("PASS "+name);
    }

    public static int Main(string[] args)
    {
        try
        {
            if (args.Length!=1) throw new ArgumentException("Usage: OfflinePakSmoke <Client-3D-2/update>");
            string root=Path.GetFullPath(args[0]);
            string folder=Path.Combine(root,"paks");
            string[] files={"script.v110.pak","settings.v149.pak","maps.v111.pak",
                            "UI.v102.pak","spr.v113.pak"};
            int[] expected={407,312,379,258,9408};
            int total=0;
            for (int i=0;i<files.Length;i++)
            {
                using(PakFile file=new PakFile(Path.Combine(folder,files[i])))
                {
                    Assert(file.Count==expected[i],"index count "+files[i]);
                    total+=file.Count;
                }
            }
            Assert(total==10764,"all 10,764 entries indexed");
            using(PakFile pak=new PakFile(Path.Combine(folder,"UI.v102.pak")))
            {
                byte[] res=pak.ReadBytes("UI/loadtip.ini",20000);
                byte[] loose=File.ReadAllBytes(Path.Combine(root,"Game_Data/StreamingAssets/UI/loadtip.ini"));
                Assert(res.Length==572,"loadtip length 572");
                Assert(EqualBytes(res,loose),"loadtip exact bytes equal loose copy");
                Assert(pak.Contains("ui/LOADTIP.ini"),"case-insensitive lookup");
                Assert(!pak.Contains("UI/../Settings/skills.txt"),"path traversal denied");
                bool fails=false;
                try { pak.ReadBytes("UI/nonexistent.ini",2000); }
                catch(FileNotFoundException) { fails=true; }
                Assert(fails,"unknown asset rejected");
                fails=false;
                try { pak.ReadBytes("UI/loadtip.ini",32); }
                catch(InvalidDataException) { fails=true; }
                Assert(fails,"memory limit enforced");
            }
            using(PakFile pak=new PakFile(Path.Combine(folder,"settings.v149.pak")))
            {
                Assert(pak.ReadBytes("Settings/skills.txt",2000000).Length==904947,
                       "skills data read and CRC validated");
            }
            using(PakFile pak=new PakFile(Path.Combine(folder,"spr.v113.pak")))
            {
                PakEntry smallest=null;
                foreach(PakEntry e in pak.Entries)
                    if(smallest==null || e.ExpandedBytes<smallest.ExpandedBytes)smallest=e;
                Assert(smallest!=null,"SPR sample found");
                byte[] bytes=pak.ReadBytes(smallest.Name,16000000);
                Assert((ulong)bytes.Length==smallest.ExpandedBytes,"SPR binary read and CRC validated");
            }
            Console.WriteLine("OFFLINE_CSHARP_PAK_PASS "+passes+" checks");
            return 0;
        }
        catch(Exception error)
        {
            Console.Error.WriteLine(error.ToString());
            return 1;
        }
    }

    private static bool EqualBytes(byte[] a,byte[] b)
    {
        if(a.Length!=b.Length)return false;
        for(int i=0;i<a.Length;i++)
            if(a[i]!=b[i])return false;
        return true;
    }
}