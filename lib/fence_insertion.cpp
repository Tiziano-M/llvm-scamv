#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/IR/Verifier.h>


using namespace llvm;

static cl::opt<std::string> FuncName("func_to_fence",
                                     cl::desc("Specify the function to be fenced.\n"),
                                     cl::value_desc("function"),
                                     cl::Hidden);

namespace {  
struct FenceInsertion : public ModulePass {
  static char ID;
  FenceInsertion() : ModulePass(ID) {}
  
  bool runOnModule(Module &M) override {
    llvm::StringRef FN = FuncName;
    Function *F = M.getFunction(FN);
    if (!F)
      return 1;
    
    bool res = 0;
    bool active = true;
    for (Function::iterator FI = F->begin(); FI != F->end() && active; ++FI) {
      for (BasicBlock::iterator BBI = FI->begin(); BBI != FI->end() && active; ++BBI) {
        //errs() << "Instruction: " << *BBI << "\n";
        //errs() << "OP: " << BBI->getOpcodeName() << "\n";
        if (llvm::isa <llvm::BranchInst> (BBI)) {
          BranchInst *BC = cast<BranchInst> (BBI);
          if(BC->isConditional()) {    
            Instruction *PI = BC->getPrevNode();
            if (llvm::isa <llvm::FenceInst> (PI)) {
              continue;
            } 
            else {
              llvm::Instruction *I = &*BBI;
              llvm::IRBuilder<> builder(I);
              builder.SetInsertPoint(I);
              llvm::FenceInst *FI = builder.CreateFence(llvm::AtomicOrdering::SequentiallyConsistent);
              //llvm::errs() << "INSERTED FENCE: " << *FI << "\n";
              res = 1;
              active = false;
            }
          }
        }
      }
    }
    
    llvm::verifyFunction(*F);
    
    llvm::errs() << res << "\n";

    return false;
  }


};
}

char FenceInsertion::ID = 0;

static RegisterPass<FenceInsertion> X("fence-insertion", "Fence Insertion Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
                             
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new FenceInsertion()); });
