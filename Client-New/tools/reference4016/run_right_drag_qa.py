"""Temporary DEV-only input adapter. Detaches and restores native input on exit."""
from pathlib import Path
import argparse, hashlib, json, sys, threading

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--pid',type=int,required=True)
    p.add_argument('--seconds',type=int,default=0,help='0: keep active until game exit or Ctrl+C')
    p.add_argument('--log',type=Path,required=True)
    p.add_argument('--observe-only',action='store_true')
    args=p.parse_args()
    binary=Path(r'E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll')
    if hashlib.sha256(binary.read_bytes()).hexdigest()!='011357720b210aa1f392a8b62ff8de090d3a895230a6e0135bce707196871069':
        raise SystemExit('Unsupported reference binary')
    sys.path.insert(0,str(Path.home()/'AppData/Local/Temp/codex-client3d-analysis'))
    import frida
    if not any(x.pid==args.pid and x.name.lower()=='game.exe' for x in frida.get_local_device().enumerate_processes()):
        raise SystemExit('Expected game PID absent')
    here=Path(__file__).parent
    source='const QA_OBSERVE_ONLY='+str(args.observe_only).lower()+';\n'+(here/'right_drag_policy.cjs').read_text()+'\n'+(here/'right_drag_qa.js').read_text()
    args.log.parent.mkdir(parents=True,exist_ok=True)
    with args.log.open('w',encoding='utf-8') as log:
        def record(message,data):
            line=json.dumps(message,ensure_ascii=False)
            log.write(line+'\n');log.flush();print(line,flush=True)
        session=frida.attach(args.pid)
        detached=threading.Event()
        session.on('detached',lambda *unused:detached.set())
        script=session.create_script(source)
        script.on('message',record)
        try:
            script.load()
            detached.wait(args.seconds if args.seconds>0 else None)
        finally:
            if not detached.is_set():
                try: script.unload()
                finally: session.detach()

if __name__=='__main__':main()
