using System;
using System.IO;
using Jx3D.OfflineAssets;

public static class TestOfflinePakBootGate
{
    private static int passed;
    private static void Check(bool condition,string label)
    {
        if(!condition)throw new InvalidDataException("Test failed: "+label);
        ++passed;
        Console.WriteLine("PASS "+label);
    }

    public static int Main(string[] args)
    {
        try
        {
            if(args.Length!=1)throw new ArgumentException("Usage: <installed update folder>");
            string folder=Path.Combine(args[0],"paks");
            byte[] output;
            using(OfflinePakBootGate gate=new OfflinePakBootGate())
            {
                Check(gate.State==OfflineBootState.NotStarted,"starts NotStarted");
                Check(!gate.TryLoadPakBytes("UI/loadtip.ini",2048,out output),
                    "cannot read assets before Prepare");
                Check(gate.Prepare(folder),"Prepare five installed PAKs");
                Check(gate.State==OfflineBootState.Ready,"mount gate becomes Ready");
                Check(gate.ResourceCount==10764,"all 10764 unique paths registered");
                Check(gate.TryLoadPakBytes("UI/loadtip.ini",2048,out output)
                    && output.Length==572,"UI early asset available");
                Check(gate.TryLoadPakBytes("Settings/skills.txt",2000000,out output)
                    && output.Length==904947,"Settings early asset available");
                Check(gate.TryLoadPakBytes("Settings/maps/map_list.ini",500000,out output),
                    "Maps lookup available");
                Check(gate.TryLoadPakBytes("script/item/saveskill_book.lua",100000,out output),
                    "Lua lookup available");
                Check(gate.TryLoadPakBytes("spr/emote/01.spr",500000,out output),
                    "SPR lookup available");
                Check(!gate.TryLoadPakBytes("UI/missing_offline.ini",5000,out output),
                    "missing resource does not fall back to remote network");
                Check(gate.Prepare(folder),"Prepare is idempotent on success");
            }
            using(OfflinePakBootGate gate=new OfflinePakBootGate())
            {
                Check(!gate.Prepare(Path.Combine(folder,"missing-directory")),
                    "missing PAK source makes gate fail");
                Check(gate.State==OfflineBootState.Failed,
                    "failure is explicit");
                Check(!String.IsNullOrEmpty(gate.Error),
                    "failure includes diagnostic reason");
                Check(!gate.TryLoadPakBytes("UI/loadtip.ini",2048,out output),
                    "failure cannot expose partial resources");
                Check(!gate.Prepare(folder),
                    "failed gate cannot silently retry through alternate files");
            }
            using(OfflinePakBootGate gate=new OfflinePakBootGate())
            {
                Check(!gate.Prepare(null),"invalid path fails closed");
            }
            OfflinePakBootGate disposed=new OfflinePakBootGate();
            disposed.Dispose();
            bool refused=false;
            try {disposed.Prepare(folder);}
            catch(ObjectDisposedException){refused=true;}
            Check(refused,"cannot start disposed gate");
            Console.WriteLine("OFFLINE_BOOT_GATE_PASS "+passed+" checks");
            return 0;
        }
        catch(Exception e)
        {
            Console.Error.WriteLine(e);
            return 1;
        }
    }
}