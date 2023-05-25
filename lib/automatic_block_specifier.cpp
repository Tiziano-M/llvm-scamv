#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include <llvm/IR/Instructions.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Transforms/Utils/CodeExtractor.h>

#include <string>



using namespace llvm;


static cl::opt<std::string> FuncName("func",
                                     cl::desc("Specify the function name.\n"),
                                     cl::value_desc("function"),
                                     cl::Hidden);
                                    
static cl::opt<unsigned> Threshold("t",
                                   cl::desc("Approximation of total size.\n"),
                                   cl::value_desc("threshold"),
                                   cl::Hidden);

static cl::opt<bool> SuccessorTraversal("test-st",
                                        cl::desc("test"),
                                        cl::Hidden);

namespace {  
struct AutomaticBlockSpecifier : public ModulePass {
  static char ID;
  AutomaticBlockSpecifier() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) override {
    llvm::StringRef FN = FuncName;
    Function *F = M.getFunction(FN);
    if (!F || !Threshold) {
      return 1;
    }

    std::string res_str;
    
    for (Function::iterator I = F->begin(), E = F->end(); I != E; ++I) {
      //errs() << "\n\nIteration of " << I->getName() << "\n";
      
      if (not SuccessorTraversal) {
        llvm::SmallVector<BasicBlock *, 32> BBs;
        
        unsigned NInsts = 0;
        for (Function::iterator BB = I, BE = F->end(); BB != BE; ++BB) {
          //errs() << "Basic block (name=" << BB->getName() << ") has " << BB->size() << " instructions.\n";
          
          NInsts += BB->size();
          if (NInsts <= Threshold) {
            BasicBlock *bb = &*BB;
            BBs.push_back(bb);
          }
          else {
            break;
          }
        }
        //errs() << "SmallVector size: " << BBs.size() << "\n";

        res_str = printSpecifiedBasicBlocks(res_str, BBs, FN);
      }
      else {        
        llvm::SmallVector<llvm::BasicBlock *, 32> BBs;
        BasicBlock *bb = dyn_cast<BasicBlock>(&*I);
        BBs.push_back(bb);
        BBs = getSuccessors(BBs, bb);
        std::sort(BBs.begin(), BBs.end());
        auto last = std::unique(BBs.begin(), BBs.end());
        BBs.erase(last, BBs.end());

        res_str = printSpecifiedBasicBlocks(res_str, BBs, FN);
      }
    }

    if (!res_str.empty())
      errs() << res_str;


    return false;
  }
  
  llvm::SmallVector<BasicBlock *, 32> getSuccessors(llvm::SmallVector<BasicBlock *, 32> SuccList, BasicBlock *bb) {
    unsigned ni = 0;
    for (llvm::SmallVector<BasicBlock *, 32>::iterator IT = SuccList.begin(), IE = SuccList.end(); IT != IE; ++IT) {
      BasicBlock *BBIT = cast<BasicBlock>(*IT);
      ni += BBIT->size();
    }
    
    for (BasicBlock *SBB : successors(bb)) {
      ni += SBB->size();
      if (ni <= Threshold) {
        SuccList.push_back(SBB);
        SuccList = getSuccessors(SuccList, SBB);
      }
      else {
        break;
      }
    }
    return SuccList;
  }

  std::string printSpecifiedBasicBlocks(std::string out, llvm::SmallVector<BasicBlock *, 32> bbs, llvm::StringRef fn) {
    if (!bbs.empty()) {
        CodeExtractorAnalysisCache CEAC(*bbs[0]->getParent()); //*F
        llvm::CodeExtractor CE(bbs);
        //errs() << "Is the extraction eligible? " << (CE.isEligible() ? "true": "false") << "\n";
        
        if (CE.isEligible()) {
          //errs() << "\nResult:\n";
          //errs() << fn << ":";
          std::vector<std::string> bbs_str;
          
          for (auto *BB : bbs){
            //errs() << BB->getName() << ";";
            if (!BB->getName().empty())
              bbs_str.push_back(BB->getName().str());
          }
          //errs() << "\n";
          if (!bbs_str.empty()) {
            out.append(fn) ;
            out.append(":");
            for (auto bb : bbs_str) {
              out.append(bb);
              out.append(";");
            }
            out.append("\n");
          }
        }
    }
    return out;
  }

};
}

char AutomaticBlockSpecifier::ID = 0;

static RegisterPass<AutomaticBlockSpecifier> X("automatic-block-specifier", "AutomaticBlockSpecifier Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
                             
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new AutomaticBlockSpecifier()); });
