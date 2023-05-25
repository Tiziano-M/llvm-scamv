#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"



using namespace llvm;

namespace {  
struct AddAArch64SLHAttribute : public FunctionPass {
  static char ID;
  AddAArch64SLHAttribute() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) override {
    //AttributeList Attrs = F.getAttributes();

    Attribute attr = Attribute::get(F.getContext(), Attribute::SpeculativeLoadHardening);
    //AttributeList NewAttrs = Attrs.addAttributeAtIndex(F.getContext(), 0, attr);
    //F.addParamAttr(0, AttributeSet::get(F.getContext(), AttributeList::FunctionIndex, attr));
    F.addFnAttr(attr);

    //F.setAttributes(NewAttrs);
    return false;
  }

};
}

char AddAArch64SLHAttribute::ID = 0;

static RegisterPass<AddAArch64SLHAttribute> X("add-aarch64-slh-attribute", "AddAArch64SLHAttribute Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
                             
static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new AddAArch64SLHAttribute()); });
