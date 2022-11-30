# llvm_playground

《Learn LLVM 12》

The IR code is not target-independent, even if it often looks like it. 

A dynamic call to a method is then executed with the following steps:
1. Calculate the offset of the vtable pointer via the getelementptr instruction.
2. Load the pointer to the vtable.
3. Calculate the offset of the function in the vtable.
4. Load the function pointer.
5. Indirectly call the function via the pointer with the call instruction

in fact, most CPU architectures can perform this dynamic call in just two instructions