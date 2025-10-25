#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

void getBlocks(Function &F, std::vector<BasicBlock *> &RetBlocks, std::vector<BasicBlock *> &UnreachableBlocks) {
  for (BasicBlock &BB : F) {
    const Instruction* terminator = BB.getTerminator();

    if (isa<ReturnInst>(terminator))
      RetBlocks.push_back(&BB);
    else if (isa<UnreachableInst>(terminator))
      UnreachableBlocks.push_back(&BB);
  }
}

void constructCommonUnreachableBlock(Function &F, std::vector<BasicBlock *> &UnreachableBlocks) {
  BasicBlock *CommonUnreachableBlock = BasicBlock::Create(F.getContext(), "CommonUnreachableBlock", &F);
  new UnreachableInst(F.getContext(), CommonUnreachableBlock);

  for (BasicBlock *BB : UnreachableBlocks) {
    BB->back().eraseFromParent();
    BranchInst::Create(CommonUnreachableBlock, BB);
  }
}

void constructCommonReturnBlock(Function &F, std::vector<BasicBlock *> &ReturnBlocks) {
    BasicBlock *CommonReturnBlock = BasicBlock::Create(F.getContext(), "CommonReturnBlock", &F);
    PHINode *PN = nullptr;
    Type *ReturnType = F.getReturnType();

    if (ReturnType->isVoidTy()) {
      ReturnInst::Create(F.getContext(), nullptr, CommonReturnBlock);
    }
    else {
      PN = PHINode::Create(ReturnType, ReturnBlocks.size(), "CommonRetVal");
      PN->insertInto(CommonReturnBlock, CommonReturnBlock->end());
      ReturnInst::Create(F.getContext(), PN, CommonReturnBlock);
    }

    for (BasicBlock *BB : ReturnBlocks) {
      ReturnInst *retInst = cast<ReturnInst>(BB->getTerminator());
      if (PN != nullptr) {
        PN->addIncoming(retInst->getReturnValue(), BB);
      }

      retInst->eraseFromParent();
      BranchInst::Create(CommonReturnBlock, BB);
    }
}

// The implementation of the pass
bool runMergeReturn(Function &F) {
  std::vector<BasicBlock *> ReturnBlocks;
  std::vector<BasicBlock *> UnreachableBlocks;
  getBlocks(F, ReturnBlocks, UnreachableBlocks);

  if (ReturnBlocks.size() <= 1 && UnreachableBlocks.size() <= 1)
    return false;

  if (UnreachableBlocks.size() > 1) {
    constructCommonUnreachableBlock(F, UnreachableBlocks);
  }

  if (ReturnBlocks.size() > 1) {
    constructCommonReturnBlock(F, ReturnBlocks);
  }

  return true;
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
