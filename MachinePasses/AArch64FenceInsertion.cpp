#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include "AArch64.h"
#include "AArch64InstrInfo.h"
#include "AArch64RegisterInfo.h"
#include "AArch64Subtarget.h"
#include "AArch64MachineFunctionInfo.h"

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"

#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineSSAUpdater.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSchedule.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"



using namespace llvm;

#define DEBUG_TYPE "aarch64-fence-insertion"

#define AARCH64_FENCE_INSERTION_NAME "AArch64 fence insertion pass"



namespace {  
class AArch64FenceInsertion : public MachineFunctionPass {
public:
  static char ID;  
  AArch64FenceInsertion() : MachineFunctionPass(ID) {
    initializeAArch64FenceInsertionPass(*PassRegistry::getPassRegistry());
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

  StringRef getPassName() const override {
    return AARCH64_FENCE_INSERTION_NAME;
  }
  
};


bool AArch64FenceInsertion::runOnMachineFunction(MachineFunction &MF) {
  bool Modified = false;
  //if (skipFunction(Fn.getFunction()))
  //  return false;
  
  const TargetSubtargetInfo &STI = MF.getSubtarget();
  const TargetInstrInfo *TII = STI.getInstrInfo();
  
  //https://github.com/llvm/llvm-project/blob/main/llvm/lib/Target/X86/X86SpeculativeLoadHardening.cpp#L567
  SmallSetVector<MachineBasicBlock *, 8> Blocks;
  for (MachineBasicBlock &MBB : MF) {
    // If there are no or only one successor, nothing to do here.
    if (MBB.succ_size() <= 1)
      continue;

    // Skip blocks unless their terminators start with a branch. Other
    // terminators don't seem interesting for guarding against misspeculation.
    auto TermIt = MBB.getFirstTerminator();
    if (TermIt == MBB.end() || !TermIt->isBranch())
      continue;

    // Add all the non-EH-pad succossors to the blocks we want to harden. We
    // skip EH pads because there isn't really a condition of interest on
    // entering.
    for (MachineBasicBlock *SuccMBB : MBB.successors())
      if (!SuccMBB->isEHPad())
        Blocks.insert(SuccMBB);
  }

  for (MachineBasicBlock *MBB : Blocks) {
    auto InsertPt = MBB->SkipPHIsAndLabels(MBB->begin());
    BuildMI(*MBB, InsertPt, DebugLoc(), TII->get(AArch64::DSB)).addImm(0xf);
    BuildMI(*MBB, InsertPt, DebugLoc(), TII->get(AArch64::ISB)).addImm(0xf);
  }

  return Modified;
}


char AArch64FenceInsertion::ID = 0;
} // end of anonymous namespace

INITIALIZE_PASS(AArch64FenceInsertion, "aarch64-fence-insertion",
                AARCH64_FENCE_INSERTION_NAME, false, false)



FunctionPass *llvm::createAArch64FenceInsertionPass() {
  return new AArch64FenceInsertion();
}

