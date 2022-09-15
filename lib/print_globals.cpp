#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"



using namespace llvm;

namespace {  
struct PrintGlobals : public ModulePass {
  static char ID;
  PrintGlobals() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) override {
    for (GlobalValue &GV : M.getGlobalList()) {
      errs() << GV.getName() << "\n";
    }
    return false;
  }

};
}

char PrintGlobals::ID = 0;

static RegisterPass<PrintGlobals> X("print-globals", "PrintGlobals Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
                             
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new PrintGlobals()); });
