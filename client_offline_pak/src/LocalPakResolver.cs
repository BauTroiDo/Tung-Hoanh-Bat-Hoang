// Read-only native-C# compatible PAK resolver for offline Unity integration.
// No HTTP server required. Call Mount() BEFORE UI/loadtip.ini or streaming assets.
// This is source-level implementation; installed IL2CPP binary cannot load it
// without rebuilding the authorized Unity project.
using System;
using System.Collections.Generic;
using System.IO;

namespace Jx3D.OfflineAssets
{
    public sealed class LocalPakResolver : IDisposable
    {
        private readonly Dictionary<string, PakFile> archiveForName;
        private readonly List<PakFile> openPaks;
        private bool disposed;
        public int ResourceCount { get { return archiveForName.Count; } }

        public LocalPakResolver(string packFolder)
        {
            if (String.IsNullOrWhiteSpace(packFolder))
                throw new ArgumentException("Pack path is empty","packFolder");
            string[] files={
                "script.v110.pak",
                "settings.v149.pak",
                "maps.v111.pak",
                "UI.v102.pak",
                "spr.v113.pak"
            };
            archiveForName=new Dictionary<string,PakFile>(StringComparer.OrdinalIgnoreCase);
            openPaks=new List<PakFile>();
            try
            {
                foreach (string file in files)
                {
                    PakFile pak=new PakFile(Path.Combine(packFolder,file));
                    openPaks.Add(pak);
                    foreach(PakEntry item in pak.Entries)
                    {
                        if (archiveForName.ContainsKey(item.Name))
                            throw new InvalidDataException("Ambiguous resource path: "+item.Name);
                        archiveForName.Add(item.Name,pak);
                    }
                }
                if (archiveForName.Count!=10764)
                    throw new InvalidDataException("Unexpected installed PAK catalog version");
            }
            catch
            {
                Dispose();
                throw;
            }
        }

        public bool Contains(string logicalResource)
        {
            if (disposed)throw new ObjectDisposedException("LocalPakResolver");
            return !String.IsNullOrWhiteSpace(logicalResource)
                && archiveForName.ContainsKey(logicalResource);
        }

        public bool TryRead(string logicalResource,int maxBytes,out byte[] content)
        {
            if(disposed)throw new ObjectDisposedException("LocalPakResolver");
            content=null;
            PakFile target;
            if(String.IsNullOrWhiteSpace(logicalResource)
                || !archiveForName.TryGetValue(logicalResource,out target))
                return false;
            content=target.ReadBytes(logicalResource,maxBytes);
            return true;
        }

        public void Dispose()
        {
            if(disposed)return;
            disposed=true;
            foreach(PakFile file in openPaks)file.Dispose();
        }
    }
}