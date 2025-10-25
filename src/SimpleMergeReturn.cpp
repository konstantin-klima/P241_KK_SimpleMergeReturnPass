#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

bool runMergeReturn(Function &F) {
  errs() << "SimpleMergeReturnPass: Initial commit." << "\n";

  return false;
}

/* Legacy PM Registration */
struct LegacySimpleMergeReturnPass : public FunctionPass {
  static char ID;
  LegacySimpleMergeReturnPass() : FunctionPass(ID) {}
  bool runOnFunction(Function &F) override { return runMergeReturn(F); }
};

struct SimpleMergeReturn : PassInfoMixin<SimpleMergeReturn> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    if (!runMergeReturn(F))
      return PreservedAnalyses::all();
    return PreservedAnalyses::none();
  }
};

} // namespace

char LegacySimpleMergeReturnPass::ID = 0;

static RegisterPass<LegacySimpleMergeReturnPass> X("simple-mergereturn", "Simple Merge Return Pass",
                                 false /* Only looks at CFG */,
                                 false /* Analysis Pass */);

/* New PM Registration */
llvm::PassPluginLibraryInfo getSimpleMergeReturnPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "Simple MergeReturn", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerVectorizerStartEPCallback(
                [](llvm::FunctionPassManager &PM, OptimizationLevel Level) {
                  PM.addPass(SimpleMergeReturn());
                });
            PB.registerPipelineParsingCallback(
                [](StringRef Name, llvm::FunctionPassManager &PM,
                   ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "simple-mergereturn") {
                    PM.addPass(SimpleMergeReturn());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getSimpleMergeReturnPluginInfo();
}
