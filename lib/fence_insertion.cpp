#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/IR/Verifier.h>

#include <llvm/IR/InlineAsm.h>


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
    if (!F) {
      llvm::errs() << "23\n";
      return 1;
    }
    
    bool res = 0;
    bool active = true;
    for (Function::iterator FI = F->begin(); FI != F->end() && active; ++FI) {
      for (BasicBlock::iterator BBI = FI->begin(); BBI != FI->end() && active; ++BBI) {
        //errs() << "Instruction: " << *BBI << "\n";
        //errs() << "OP: " << BBI->getOpcodeName() << "\n";
        if (llvm::isa <llvm::BranchInst> (BBI)) {
          BranchInst *BC = cast<BranchInst> (BBI);
          if(BC->isConditional()) {
            BasicBlock *bb1 = BC->getSuccessor(0);
            BasicBlock *bb2 = BC->getSuccessor(1);
            //label1, label2
            Instruction *bb1I = bb1->getFirstNonPHI();
            Instruction *bb2I = bb2->getFirstNonPHI();
            //llvm::errs() << "I1\n" << *bb1I << "\n";
            //llvm::errs() << "I2\n" << *bb2I << "\n";
            
            LLVMContext &ctx= F->getParent()->getContext();
            FunctionType *FT = FunctionType::get(Type::getVoidTy(ctx), false);
            StringRef asmString = "dsb sy\nisb";
            StringRef constraints;
            llvm::InlineAsm *IA = llvm::InlineAsm::get(FT,asmString,constraints,true,false,InlineAsm::AD_ATT);
            ArrayRef<Value *> Args = None;
            
            if (CallInst *callI1 = llvm::dyn_cast <llvm::CallInst> (bb1I)) {
              if (callI1->isInlineAsm()) {
                continue;
              }
            }
            else {
              llvm::IRBuilder<> builder1(bb1I);
              llvm::CallInst *Ptr1 = builder1.CreateCall(IA,Args);
              res = 1;
              active = false;
            }
            if (CallInst *callI2 = llvm::dyn_cast <llvm::CallInst> (bb2I)) {
              if (callI2->isInlineAsm()) {
                continue;
              }
            }
            else {
              llvm::IRBuilder<> builder2(bb2I);
              llvm::CallInst *Ptr2 = builder2.CreateCall(IA,Args);
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
