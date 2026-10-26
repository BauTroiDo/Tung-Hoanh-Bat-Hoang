// CN02-BULK-02: narrow, read-only PE64 native decompilation smoke.
// Only reports counts/status; never emits source code to tracked artifacts.
// @category JX1.CN02
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.program.model.listing.Function;

public class CN02BodyPilot extends GhidraScript {
    private static final long[] RVAS = {
        0x008DFD10L, // MapService.Mps2WorldPos
        0x008DFBF0L, // MapService.Map2WorldPos
        0x008E0DD0L, // MapService.UpdateWorldInfo
        0x00C75DE0L, // GameWorld.SubWorld.LoadMap
        0x00C38990L  // Network.Header.ProcessGameUnity.OnSyncWorld
    };
    // Native PE .pdata extents; LoadMap includes TWO adjacent unwind records.
    private static final long[] NATIVE_SPANS = { 239L, 276L, 1032L, 12996L, 1112L };
    @Override
    protected void run() throws Exception {
        println("CN02_GHIDRA_PILOT_START count=" + RVAS.length +
            " binary=" + currentProgram.getName());
        Address base = currentProgram.getImageBase();
        DecompInterface dec = new DecompInterface();
        dec.setSimplificationStyle("decompile");
        if (!dec.openProgram(currentProgram)) {
            println("CN02_GHIDRA_OPEN_FAILED");
            return;
        }
        try {
            for (int i = 0; i < RVAS.length; i++) {
                long rva = RVAS[i];
                if (monitor.isCancelled()) break;
                Address addr = base.add(rva);
                AddressSet bounded = new AddressSet(addr, addr.add(NATIVE_SPANS[i] - 1));
                DisassembleCommand dis = new DisassembleCommand(addr, bounded, true);
                boolean decoded = dis.applyTo(currentProgram, monitor);
                AddressSet decodedInstructions = new AddressSet();
                ghidra.program.model.listing.InstructionIterator it =
                    currentProgram.getListing().getInstructions(bounded, true);
                while (it.hasNext()) {
                    ghidra.program.model.listing.Instruction ins = it.next();
                    decodedInstructions.add(ins.getMinAddress(), ins.getMaxAddress());
                }
                println("CN02_GHIDRA_DISASM rva=0x" + Long.toHexString(rva) +
                    " decoded=" + decoded + " nativeSpan=" + NATIVE_SPANS[i] +
                    " decodedBytes=" + decodedInstructions.getNumAddresses());
                Function fn = getFunctionAt(addr);
                if (fn == null) {
                    fn = createFunction(addr, "CN02_RVA_" + Long.toHexString(rva));
                }
                if (fn != null && decodedInstructions.getNumAddresses() >
                    fn.getBody().getNumAddresses()) {
                    try {
                        fn.setBody(decodedInstructions);
                    } catch (Exception e) {
                        println("CN02_GHIDRA_BODY_UPDATE_FAILED rva=0x" +
                            Long.toHexString(rva) + " reason=" + e.getClass().getSimpleName());
                    }
                }
                if (fn == null) {
                    println("CN02_GHIDRA_RESULT rva=0x" + Long.toHexString(rva) +
                        " status=NO_FUNCTION");
                    continue;
                }
                DecompileResults res = dec.decompileFunction(fn, 12, monitor);
                boolean ok = res.decompileCompleted() && res.getDecompiledFunction() != null;
                int chars = ok ? res.getDecompiledFunction().getC().length() : 0;
                int bodyCount = (int)fn.getBody().getNumAddresses();
                println("CN02_GHIDRA_RESULT rva=0x" + Long.toHexString(rva) +
                    " status=" + (ok ? "DECOMPILED" : "FAILED") +
                    " chars=" + chars + " bodyAddresses=" + bodyCount);
            }
        } finally {
            dec.dispose();
        }
        println("CN02_GHIDRA_PILOT_END");
    }
}
