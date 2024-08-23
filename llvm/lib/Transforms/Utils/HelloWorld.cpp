//===-- HelloWorld.cpp - Example Transformations --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <time.h>
#include "llvm/Transforms/Utils/HelloWorld.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "stdio.h"
using namespace llvm;
clock_t startfunction () {
  clock_t start_t;
  start_t = clock();
  return start_t;
}
double closefunction (clock_t start_t) {
  clock_t end_t;
  end_t = clock();
  double t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
  return t;
}
/*
Function* createTestFunction(Module *M, LLVMContext &Ctx) {
    // Define the function type: int(int)
    FunctionType *funcType = FunctionType::get(Type::getInt32Ty(Ctx), {Type::getInt32Ty(Ctx)}, false);

    // Create the function within the module
    Function *testFunc = Function::Create(funcType, Function::ExternalLinkage, "testfunction", M);

    // Create a basic block for the function
    BasicBlock *BB = BasicBlock::Create(Ctx, "entry", testFunc);

    // Use IRBuilder to add instructions to the function
    IRBuilder<> builder(BB);

    // Get the function's argument (an int)
    Function::arg_iterator args = testFunc->arg_begin();
    Value *x = args++;

    // Return 0 (or any other simple operation on 'x')
    builder.CreateRet(builder.getInt32(0));

    return testFunc;
}
*/
PreservedAnalyses HelloWorldPass::run(Function &F,
                                      FunctionAnalysisManager &AM) {
  Module *M = F.getParent();
  LLVMContext &Ctx = M->getContext();

  // Define the type of the function you want to call
  FunctionType *startfuncType = FunctionType::get(Type::getInt64Ty(Ctx), false);
  FunctionCallee StartFunction = M->getOrInsertFunction("startfunction", startfuncType);
  llvm::Function *startFunc = llvm::dyn_cast<llvm::Function>(StartFunction.getCallee());

  FunctionType *closefuncType = FunctionType::get(Type::getDoubleTy(Ctx), {Type::getInt64Ty(Ctx)}, false);
  FunctionCallee CloseFunction = M->getOrInsertFunction("closefunction", closefuncType);
  llvm::Function *CloseFunc = llvm::dyn_cast<llvm::Function>(CloseFunction.getCallee());


  llvm::FunctionType *printfType = llvm::FunctionType::get(
    llvm::Type::getInt32Ty(Ctx),                          // Return type: int
    llvm::PointerType::get(llvm::Type::getInt8Ty(Ctx), 0), // First argument: char*
    true);
  llvm::FunctionCallee printfFunc = M->getOrInsertFunction("printf", printfType);

  // Check if the function already exists in the module, otherwise create it
  /*
  if (!Func) {
      Func = createTestFunction(M, Ctx);
  }
  */
  for (auto &B : F) {
      IRBuilder<> builder(&*B.getFirstInsertionPt());
      IRBuilder<> builder2(&*B.getTerminator());
      llvm::Value *startresult = builder.CreateCall(startFunc);
      llvm::Value *formatStr = builder.CreateGlobalStringPtr("Elapsed time: %f seconds\n");
      llvm::Value *closeresult = builder2.CreateCall(CloseFunc, {startresult});
      std::vector<llvm::Value *> printfArgs;
      printfArgs.push_back(formatStr); // The format string
      printfArgs.push_back(closeresult);  // The integer value
      builder2.CreateCall(printfFunc, printfArgs);
      // llvm::verifyFunction(*B);
      break; // We only want to insert the call in the first basic block
  }
  // Output the instruction count for the current function
  errs() << "Function \"" << F.getName() << " instructions.\n";
  return PreservedAnalyses::all();
}