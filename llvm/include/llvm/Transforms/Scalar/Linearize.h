#ifndef LLVM_TRANSFORMS_SCALAR_LINEARIZE_H
#define LLVM_TRANSFORMS_SCALAR_LINEARIZE_H

#include <llvm/IR/DerivedTypes.h>
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/RandomNumberGenerator.h"

#include <memory>
#include <unordered_map>

namespace llvm {

struct LinearizePass : public PassInfoMixin<LinearizePass> {
	enum labelkind {
		LBL_FALLTHROUGH,
		LBL_FRESH,
	};

	struct label {
		labelkind kind;
		size_t fresh;
	};

	PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
	bool runOnFunction(Function &F);

	std::pair<size_t, Align> getDiscardSizeAlign(Function &F, const DataLayout &DataLayout);
	size_t getLabels(Function &F);
	void shuffleBlocks(Function &F, size_t numlbl);
	void eliminatePhis(Function &F);
	void handleIntrinsics(BasicBlock &BB);
	void resolveContinuityErrors(Function &F);
	Value *prepareBlock(BasicBlock &bb, Value *on, AllocaInst *selector, label label, AllocaInst *discard);
	
	template <typename T>
	void linearizeIns(T *ins, Value *on, AllocaInst *discard);

	std::unordered_map<BasicBlock *, struct label> labels;
	std::unique_ptr<RandomNumberGenerator> rng;
	Function *NopFun;
};
}

#endif // LLVM_TRANSFORMS_SCALAR_LINEARIZE_H
