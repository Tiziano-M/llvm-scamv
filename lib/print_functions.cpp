#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"



using namespace llvm;

namespace {  
struct PrintFunctions : public ModulePass {
  static char ID;
  PrintFunctions() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) override {
    for (Module::iterator F=M.begin(), E=M.end(); F != E; ++F) {
      if (!F->isDeclaration()) {
        errs() << F->getName() << "\n";
      }
    }
    return false;
  }

};
}

char PrintFunctions::ID = 0;

static RegisterPass<PrintFunctions> X("print-functions", "PrintFunctions Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
                             
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new PrintFunctions()); });
