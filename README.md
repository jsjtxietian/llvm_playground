# llvm_playground

《Learn LLVM 12》

calc:
with a, b: a * (4 + b)

calc : ("with" ident ("," ident)* ":")? expr ;
expr : term (( "+" | "-" ) term)* ;
term : factor (( "*" | "/") factor)* ;
factor : ident | number | "(" expr ")" ;
ident : ([a-zAZ])+ ;
number : ([0-9])+ ;


$ calc "with a: a*3" | llc –filetype=obj –o=expr.obj
$ cl expr.obj rtcalc.c
$ expr