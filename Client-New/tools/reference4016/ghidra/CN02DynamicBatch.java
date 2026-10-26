// CN02-BULK-03: dynamic bounded RVA/span probe, read-only GameAssembly analysis.
// Usage in Ghidra Headless: -postScript CN02DynamicBatch.java 0x821410:286 ...
// This prints only metrics; no reconstructed source code is exported.
// @category JX1.CN02
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Instruction;
import ghidra.program.model.listing.InstructionIterator;
import ghidra.app.cmd.disassemble.DisassembleCommand;

public class CN02DynamicBatch extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length == 0 || args.length > 8) {
            throw new IllegalArgumentException("CN02: choose 1..8 bounded native functions");
        }
        int totalSpan=0;
        for (String arg : args) {
            if (!arg.matches("0x[0-9a-fA-F]{1,8}:[0-9]{1,5}")) {
                throw new IllegalArgumentException("CN02: expected 0xRVA:BYTES");
            }
            int n=Integer.parseInt(arg.split(":")[1]);
            if(n<64 || n>2048)throw new IllegalArgumentException("CN02: span exceeds [64,2048]");
            totalSpan+=n;
        }
        if (totalSpan>8192)throw new IllegalArgumentException("CN02: aggregate >8192");
        DecompInterface dec=new DecompInterface();
        dec.setSimplificationStyle("decompile");
        if(!dec.openProgram(currentProgram))throw new IllegalStateException("Cannot open program");
        Address base=currentProgram.getImageBase();
        println("CN02_BATCH_BEGIN size="+args.length+" imageBase="+base.toString());
        try {
            for(String arg:args) {
                if(monitor.isCancelled())break;
                String[] split=arg.split(":");
                long rva=Long.parseLong(split[0].substring(2),16);
                int n=Integer.parseInt(split[1]);
                Address at=base.add(rva);
                AddressSet bounded=new AddressSet(at,at.add(n-1));
                DisassembleCommand dis=new DisassembleCommand(at,bounded,true);
                boolean accepted=dis.applyTo(currentProgram,monitor);
                AddressSet decoded=new AddressSet();
                InstructionIterator iter=currentProgram.getListing().getInstructions(bounded,true);
                while(iter.hasNext()){
                    Instruction i=iter.next();
                    decoded.add(i.getMinAddress(),i.getMaxAddress());
                }
                Function fn=getFunctionAt(at);
                if(fn==null)fn=createFunction(at,"CN02_BATCH_"+Long.toHexString(rva));
                if(fn!=null && decoded.getNumAddresses()>fn.getBody().getNumAddresses()) {
                    try { fn.setBody(decoded); }
                    catch(Exception e) {
                        println("CN02_BATCH_BODY_SET_ERROR rva=0x"+Long.toHexString(rva)+" reason="+e.getClass().getSimpleName());
                    }
                }
                boolean ok=false;
                int chars=0;
                if(fn!=null) {
                    DecompileResults result=dec.decompileFunction(fn,12,monitor);
                    ok=result.decompileCompleted() && result.getDecompiledFunction()!=null;
                    if(ok)chars=result.getDecompiledFunction().getC().length();
                }
                println("CN02_BATCH_ROW rva=0x"+Long.toHexString(rva)+
                   " nativeSpan="+n+" decodedBytes="+decoded.getNumAddresses()+
                   " functionBodyBytes="+(fn==null?0:fn.getBody().getNumAddresses())+
                   " disasmAccepted="+accepted+" decompiled="+ok+" chars="+chars);
            }
        } finally { dec.dispose(); }
        println("CN02_BATCH_END");
    }
}
