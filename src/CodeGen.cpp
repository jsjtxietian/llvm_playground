#include "CodeGen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
class ToIRVisitor : public ASTVisitor {
  // Each compilation unit is represented in LLVM by the Module class and
  // the visitor has a pointer to the module call, M.
  Module *M;
  IRBuilder<> Builder;
  // cache the needed type instance
  Type *VoidTy;
  Type *Int32Ty;
  Type *Int8PtrTy;
  Type *Int8PtrPtrTy;
  Constant *Int32Zero;

  // the current calculated value, which is updated through tree traversal
  Value *V;
  // maps a variable name to the value that's returned by the calc_read()
  // function
  StringMap<Value *> nameMap;

public:
  ToIRVisitor(Module *M) : M(M), Builder(M->getContext()) {
    VoidTy = Type::getVoidTy(M->getContext());
    Int32Ty = Type::getInt32Ty(M->getContext());
    Int8PtrTy = Type::getInt8PtrTy(M->getContext());
    Int8PtrPtrTy = Int8PtrTy->getPointerTo();
    Int32Zero = ConstantInt::get(Int32Ty, 0, true);
  }

  void run(AST *Tree) {
    // or each function, a FunctionType instance must be created. In C
    // ++ terminology, this is a function prototype.
    FunctionType *MainFty =
        FunctionType::get(Int32Ty, {Int32Ty, Int8PtrPtrTy}, false);
    Function *MainFn =
        Function::Create(MainFty, GlobalValue::ExternalLinkage, "main", M);
    BasicBlock *BB = BasicBlock::Create(M->getContext(), "entry", MainFn);
    Builder.SetInsertPoint(BB);

    Tree->accept(*this);

    FunctionType *CalcWriteFnTy = FunctionType::get(VoidTy, {Int32Ty}, false);
    Function *CalcWriteFn = Function::Create(
        CalcWriteFnTy, GlobalValue::ExternalLinkage, "calc_write", M);
    Builder.CreateCall(CalcWriteFnTy, CalcWriteFn, {V});

    Builder.CreateRet(Int32Zero);
  }

  virtual void visit(Factor &Node) override {
    if (Node.getKind() == Factor::Ident) {
      V = nameMap[Node.getVal()];
    } else {
      int intval;
      Node.getVal().getAsInteger(10, intval);
      V = ConstantInt::get(Int32Ty, intval, true);
    }
  };

  virtual void visit(BinaryOp &Node) override {
    Node.getLeft()->accept(*this);
    Value *Left = V;
    Node.getRight()->accept(*this);
    Value *Right = V;
    switch (Node.getOperator()) {
    case BinaryOp::Plus:
      V = Builder.CreateNSWAdd(Left, Right);
      break;
    case BinaryOp::Minus:
      V = Builder.CreateNSWSub(Left, Right);
      break;
    case BinaryOp::Mul:
      V = Builder.CreateNSWMul(Left, Right);
      break;
    case BinaryOp::Div:
      V = Builder.CreateSDiv(Left, Right);
      break;
    }
  };

  virtual void visit(WithDecl &Node) override {
    FunctionType *ReadFty = FunctionType::get(Int32Ty, {Int8PtrTy}, false);
    Function *ReadFn =
        Function::Create(ReadFty, GlobalValue::ExternalLinkage, "calc_read", M);
    //  loops through the variable names:
    for (auto I = Node.begin(), E = Node.end(); I != E; ++I) {
      // For each variable, a string with a variable name is created:
      StringRef Var = *I;
      // Create call to calc_read function.
      Constant *StrText = ConstantDataArray::getString(M->getContext(), Var);
      GlobalVariable *Str =
          new GlobalVariable(*M, StrText->getType(),
                             /*isConstant=*/true, GlobalValue::PrivateLinkage,
                             StrText, Twine(Var).concat(".str"));
      // the IR code to call the calc_read() function is created.
      Value *Ptr =
          Builder.CreateInBoundsGEP(Str, {Int32Zero, Int32Zero}, "ptr");
      CallInst *Call = Builder.CreateCall(ReadFty, ReadFn, {Ptr});

      // The returned value is stored in the mapNames map
      nameMap[Var] = Call;
    }

    Node.getExpr()->accept(*this);
  };
};
} // namespace

// creates the global context and the module, runs the tree traversal,
// and dumps the generated IR to the console:
void CodeGen::compile(AST *Tree) {
  LLVMContext Ctx;
  Module *M = new Module("calc.expr", Ctx);
  ToIRVisitor ToIR(M);
  ToIR.run(Tree);
  M->print(outs(), nullptr);
}