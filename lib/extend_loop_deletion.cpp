#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"


#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Analysis/MemorySSA.h>
#include <llvm/Transforms/Utils/LoopUtils.h>



using namespace llvm;


namespace {	
struct ExtendLoopDeletion : public LoopPass {
  static char ID;
  ExtendLoopDeletion() : LoopPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addPreserved<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addPreserved<LoopInfoWrapperPass>();
    AU.addRequired<ScalarEvolutionWrapperPass>();
    AU.addPreserved<ScalarEvolutionWrapperPass>();
  }


  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    //auto *Preheader = L->getLoopPreheader();
    auto *ExitBlock = L->getUniqueExitBlock();
    //if (!Preheader || !L->hasDedicatedExits())
    //  errs() << "Deletion requires Loop with preheader and dedicated exits.\n";
        
    LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    DominatorTree &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();
    auto *MSSAAnalysis = getAnalysisIfAvailable<MemorySSAWrapperPass>();
    MemorySSA *MSSA = nullptr;
    if (MSSAAnalysis)
      MSSA = &MSSAAnalysis->getMSSA();
    
    SE.forgetLoop(L);
    // Set incoming value to poison for phi nodes in the exit block.
    /*for (PHINode &P : ExitBlock->phis()) {
      std::fill(P.incoming_values().begin(), P.incoming_values().end(), PoisonValue::get(P.getType()));
    }*/
    
    //breakLoopBackedge(L, DT, SE, LI, MSSA);
    deleteDeadLoop(L, &DT, &SE, &LI, MSSA);
    LPM.markLoopAsDeleted(*L);

    return false;
  }

};
}

char ExtendLoopDeletion::ID = 0;

static RegisterPass<ExtendLoopDeletion> X("extend-loop-deletion", "ExtendLoopDeletion Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
                             
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new ExtendLoopDeletion()); });
