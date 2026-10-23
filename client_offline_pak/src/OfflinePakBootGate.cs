// Offline-first boot gate for an authorized Unity source integration.
// C# source-only: the installed IL2CPP Game.exe is not modified by this module.
using System;
using System.Collections.Generic;
using System.IO;

namespace Jx3D.OfflineAssets
{
    public enum OfflineBootState { NotStarted, Ready, Failed, Disposed }

    public sealed class OfflinePakBootGate : IDisposable
    {
        private readonly Func<string, LocalPakResolver> resolverFactory;
        private LocalPakResolver resolver;
        public OfflineBootState State { get; private set; }
        public string Error { get; private set; }
        public int ResourceCount { get { return resolver==null?0:resolver.ResourceCount; } }

        public OfflinePakBootGate() : this(path => new LocalPakResolver(path)) { }

        // Test injection only: lets tests prove gating and fault handling.
        public OfflinePakBootGate(Func<string,LocalPakResolver> resolverFactory)
        {
            if (resolverFactory==null)throw new ArgumentNullException("resolverFactory");
            this.resolverFactory=resolverFactory;
            State=OfflineBootState.NotStarted;
        }

        // Invoke BEFORE login UI and LoadResource.WarmupBootAssetsAsync.
        // Idempotent on Ready; failure does not fall back to unvalidated CDN.
        public bool Prepare(string packFolder)
        {
            if (State==OfflineBootState.Disposed)
                throw new ObjectDisposedException("OfflinePakBootGate");
            if (State==OfflineBootState.Ready)
                return true;
            if (State==OfflineBootState.Failed)
                return false;
            try
            {
                if (String.IsNullOrWhiteSpace(packFolder))
                    throw new ArgumentException("Empty PACK folder");
                resolver=resolverFactory(packFolder);
                if(resolver==null)
                    throw new InvalidDataException("Factory did not return a reader");
                if (resolver.ResourceCount!=10764)
                    throw new InvalidDataException("PACK index differs from validated version");
                Require("UI/loadtip.ini",20000);
                Require("Settings/skills.txt",3000000);
                Require("Settings/maps/map_list.ini",500000);
                Require("script/item/saveskill_book.lua",200000);
                Require("spr/emote/01.spr",500000);
                State=OfflineBootState.Ready;
                Error=null;
                return true;
            }
            catch(Exception e)
            {
                Error=e.GetType().Name+": "+e.Message;
                if (resolver!=null){resolver.Dispose();resolver=null;}
                State=OfflineBootState.Failed;
                return false;
            }
        }

        private void Require(string logicalPath,int maxBytes)
        {
            byte[] data;
            if (!resolver.TryRead(logicalPath,maxBytes,out data) || data==null || data.Length==0)
                throw new InvalidDataException("Missing offline bootstrap asset: "+logicalPath);
        }

        public bool TryLoadPakBytes(string logicalPath,int maxBytes,out byte[] content)
        {
            content=null;
            if(State!=OfflineBootState.Ready)
                return false;
            return resolver.TryRead(logicalPath,maxBytes,out content);
        }

        public void Dispose()
        {
            if(resolver!=null){resolver.Dispose();resolver=null;}
            State=OfflineBootState.Disposed;
        }
    }
}