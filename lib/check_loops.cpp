#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <llvm/Analysis/LoopPass.h>




using namespace llvm;

namespace {	
struct CheckLoops : public LoopPass {
  static char ID;
  CheckLoops() : LoopPass(ID) {}


  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    errs() << "loop found\n";
   
    return false;
  }

};
}

char CheckLoops::ID = 0;

static RegisterPass<CheckLoops> X("check-loops", "CheckLoops Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
                             
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new CheckLoops()); });
