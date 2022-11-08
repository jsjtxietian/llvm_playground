# llvm_playground
from https://www.llvm.org/docs/tutorial/MyFirstLanguageFrontend

clang++-15 -g -O3 toy.cpp `llvm-config --cxxflags`

## Kaleidoscope 
``` 
# Compute the x'th fibonacci number.
def fib(x)
  if x < 3 then
    1
  else
    fib(x-1)+fib(x-2)

# This expression will compute the 40th number.
fib(40)
```

```
extern sin(arg);
extern cos(arg);
extern atan2(arg1 arg2);

atan2(sin(.4), cos(42))
```