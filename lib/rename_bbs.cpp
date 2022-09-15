#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"



using namespace llvm;



namespace {  
struct RenameBasicBlocks : public FunctionPass {
  static char ID;
  RenameBasicBlocks() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) override {
    MetaRename(F);

    return false;
  }
  
  //https://github.com/llvm/llvm-project/blob/main/llvm/lib/Transforms/Utils/MetaRenamer.cpp#L108
  void MetaRename(Function &F) {
    for (Argument &Arg : F.args())
      if (!Arg.getType()->isVoidTy())
        Arg.setName("arg");

    for (auto &BB : F) {
      BB.setName("bb");

      for (auto &I : BB)
        if (!I.getType()->isVoidTy())
          I.setName("tmp");
    }
  }

};
}

char RenameBasicBlocks::ID = 0;

static RegisterPass<RenameBasicBlocks> X("rename-blocks", "RenameBasicBlocks Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
                             
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new RenameBasicBlocks()); });
