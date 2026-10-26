// In-memory QA adapter for the hash-pinned 4016 reference. Never ship this in a product.
const m=Process.getModuleByName('GameAssembly.dll');
if(m.path.replace(/\\/g,'/').toLowerCase()!=='e:/games/vltk3d/swordonline/client-3d-2/update/gameassembly.dll')throw Error('Unexpected loaded module path');
const exp=(n,r,a)=>new NativeFunction(m.getExportByName(n),r,a);
const domain=exp('il2cpp_domain_get','pointer',[]);
const assemblies=exp('il2cpp_domain_get_assemblies','pointer',['pointer','pointer']);
const image=exp('il2cpp_assembly_get_image','pointer',['pointer']);
const klass=exp('il2cpp_class_from_name','pointer',['pointer','pointer','pointer']);
const method=exp('il2cpp_class_get_method_from_name','pointer',['pointer','pointer','int']);
function type(ns,n){let size=Memory.alloc(8),ar=assemblies(domain(),size);for(let i=0;i<size.readU32();i++){let c=klass(image(ar.add(i*8).readPointer()),Memory.allocUtf8String(ns),Memory.allocUtf8String(n));if(!c.isNull())return c;}throw Error('Missing type '+n);}
function address(c,n,argc){let p=method(c,Memory.allocUtf8String(n),argc);if(p.isNull())throw Error('Missing method '+n);return p.readPointer();}
const input=type('UnityEngine','Input');
const keyAddr=address(input,'GetKeyInt',1),buttonAddr=address(input,'GetMouseButton',1);
const downAddr=address(input,'GetMouseButtonDown',1);
const rawButton=new NativeFunction(buttonAddr,'bool',['int','pointer']);
const rawPosition=new NativeFunction(address(input,'get_mousePosition',0),'pointer',['pointer','pointer']);
const frame=new NativeFunction(address(type('UnityEngine','Time'),'get_frameCount',0),'int',['pointer']);
const focus=new NativeFunction(address(type('UnityEngine','Application'),'get_isFocused',0),'bool',['pointer']);
const es=type('UnityEngine.EventSystems','EventSystem');
const current=new NativeFunction(address(es,'get_current',0),'pointer',['pointer']);
const overUi=new NativeFunction(address(es,'IsPointerOverGameObject',1),'bool',['pointer','int','pointer']);
// The HUD has full-screen raycast targets. Use the client's own blocking-UI filter.
const clickType=type('','PlayerClickDetector');
const field=exp('il2cpp_class_get_field_from_name','pointer',['pointer','pointer']);
const staticRead=exp('il2cpp_field_static_get_value','void',['pointer','pointer']);
const instanceField=field(clickType,Memory.allocUtf8String('_instance'));
if(instanceField.isNull())throw Error('Missing PlayerClickDetector instance');
const instanceBuffer=Memory.alloc(8);
const canClick=new NativeFunction(address(clickType,'CanProcessWorldClick',0),'bool',['pointer','pointer']);
const gesture=new RightDrag(),position=Memory.alloc(12),contexts=new Map();
let lastFrame=-1,state={orbit:false,click:false},raw=false,held=false;
let cameraCalls=0,orbitFrames=0,deferredClicks=0,suppressedDown=0,errors=0;
let samples=0,heldFrames=0,uiFrames=0,unfocusedFrames=0,lastSample=null;
function sample(){
 const f=frame(ptr(0));if(f===lastFrame)return;lastFrame=f;
 raw=true;
 try{
  held=rawButton(1,ptr(0));rawPosition(position,ptr(0));
  const ui=current(ptr(0));const genericUi=!ui.isNull()&&overUi(ui,-1,ptr(0));
  staticRead(instanceField,instanceBuffer);const detector=instanceBuffer.readPointer();
  const allowed=!detector.isNull()&&canClick(detector,ptr(0));
  const focused=focus(ptr(0));samples++;if(held)heldFrames++;if(!allowed)uiFrames++;if(!focused)unfocusedFrames++;
  lastSample={held,focused,allowed,genericUi,x:position.readFloat(),y:position.add(4).readFloat(),frame:f};
  state=gesture.sample(held,lastSample.x,lastSample.y,focused,allowed);
  if(state.orbit)orbitFrames++;if(state.click)deferredClicks++;
 }finally{raw=false;}
}
function scope(rva,name){Interceptor.attach(m.base.add(rva),{
 onEnter(){this.t=Process.getCurrentThreadId();this.old=contexts.get(this.t);try{sample();contexts.set(this.t,name);if(name==='camera')cameraCalls++;}catch(e){state={orbit:false,click:false};if(errors++<3)send({kind:'error',message:String(e)});}},
 onLeave(){if(this.old===undefined)contexts.delete(this.t);else contexts.set(this.t,this.old);}
});}
scope(0xa3ec70,'camera');scope(0x8334e0,'click');
Interceptor.attach(keyAddr,{onEnter(a){this.force=!QA_OBSERVE_ONLY&&!raw&&contexts.get(Process.getCurrentThreadId())==='camera'&&state.orbit&&[307,308].includes(a[0].toInt32());},onLeave(r){if(this.force)r.replace(1);}});
Interceptor.attach(buttonAddr,{onEnter(a){this.scope=raw||QA_OBSERVE_ONLY?null:contexts.get(Process.getCurrentThreadId());this.button=a[0].toInt32();},onLeave(r){if(this.scope==='camera'&&this.button===0&&state.orbit)r.replace(1);if(this.scope==='click'&&this.button===1)r.replace(0);}});
Interceptor.attach(downAddr,{onEnter(a){this.change=!QA_OBSERVE_ONLY&&!raw&&contexts.get(Process.getCurrentThreadId())==='click'&&a[0].toInt32()===1;},onLeave(r){if(this.change){if(r.toInt32())suppressedDown++;r.replace(state.click?1:0);}}});
send({kind:'ready',pid:Process.id,observeOnly:QA_OBSERVE_ONLY,mode:'right drag camera; stationary right click deferred to release',diskMutations:false});
setInterval(()=>send({kind:'counters',observeOnly:QA_OBSERVE_ONLY,cameraCalls,orbitFrames,deferredClicks,suppressedDown,errors,samples,heldFrames,uiFrames,unfocusedFrames,lastSample}),5000);
