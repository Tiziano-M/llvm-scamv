#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <llvm/IR/Instructions.h>
#include <llvm/Support/CommandLine.h>



using namespace llvm;


static cl::opt<std::string> FuncName(cl::Positional,
                                     cl::desc("Approximation of the number of instructions. Specify the function name.\n"),
                                     cl::value_desc("function"),
                                     cl::Hidden);

namespace {	
struct AnalyzeFunction : public ModulePass {
  static char ID;
  AnalyzeFunction() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) override {
    llvm::StringRef FN = FuncName;
    Function *F = M.getFunction(FN);
    if (F) {
      count_inst(*F);
    }
    else {
      return 1;
    }
		

    return false;
  }
	
	
  void count_inst(Function &F) {		
    unsigned ICount = 0;
    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        if (CallInst *callI = dyn_cast<CallInst>(&I)) {
          if (Function *calledF = callI->getCalledFunction()) {
            if (!calledF->isDeclaration()) {
              ICount += calledF->getInstructionCount();
         	  }
          }
        }
        else {
      	  ++ICount;
        }
	    }
    }
    errs() << ICount << "\n";
  }

};
}

char AnalyzeFunction::ID = 0;

static RegisterPass<AnalyzeFunction> X("analyze-function", "AnalyzeFunction Pass",
                             false /* Only looks at CFG */,
                              false /* Analysis Pass */);
                             
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new AnalyzeFunction()); });
