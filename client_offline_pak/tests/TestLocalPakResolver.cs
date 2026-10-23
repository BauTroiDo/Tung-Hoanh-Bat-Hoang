using System;
using System.IO;
using Jx3D.OfflineAssets;
public static class TestLocalPakResolver
{
    private static int successes;
    private static void Check(bool yes,string label)
    {
        if(!yes)throw new InvalidDataException("Failed: "+label);
        ++successes;
        Console.WriteLine("PASS "+label);
    }
    public static int Main(string[] args)
    {
        try
        {
            if(args.Length!=1)throw new ArgumentException("Usage: <update-path>");
            using(LocalPakResolver resolver=new LocalPakResolver(Path.Combine(args[0],"paks")))
            {
                Check(resolver.ResourceCount==10764,"exactly 10764 unique resources");
                string[] paths={
                    "script/item/saveskill_book.lua",
                    "Settings/skills.txt",
                    "Settings/maps/map_list.ini",
                    "UI/loadtip.ini",
                    "spr/emote/01.spr"
                };
                foreach(string p in paths)
                {
                    byte[] payload;
                    bool found=resolver.TryRead(p,5000000,out payload);
                    Check(found && payload!=null && payload.Length>0,"direct byte access "+p);
                }
                byte[] noFile;
                Check(!resolver.TryRead("UI/../../Settings/skills.txt",5000000,out noFile),
                      "path traversal rejected");
                Check(!resolver.TryRead("Settings/file-not-present.txt",5000000,out noFile),
                      "unknown path rejected");
                Check(resolver.Contains("UI/LOADTIP.ini"),"case-insensitive lookup");
                bool rejected=false;
                try {resolver.TryRead("Settings/skills.txt",10,out noFile);}
                catch(InvalidDataException){rejected=true;}
                Check(rejected,"memory safety bound");
            }
            Console.WriteLine("LOCAL_CSHARP_RESOLVER_PASS "+successes+" checks");
            return 0;
        }
        catch(Exception e)
        {
            Console.Error.WriteLine(e);
            return 1;
        }
    }
}