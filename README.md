# llvm_playground

《Learn LLVM 12》

compilationUnit : "MODULE" identifier ";" ( import )* block identifier "." ;
Import : ( "FROM" identifier )? "IMPORT" identList ";" ;
Block: ( declaration )* ( "BEGIN" statementSequence )? "END" ;

declaration
    : "CONST" ( constantDeclaration ";" )*
    | "VAR" ( variableDeclaration ";" )*
    | procedureDeclaration ";" ;

constantDeclaration : identifier "=" expression ;
variableDeclaration : identList ":" qualident ;
qualident : identifier ( "." identifier )* ;
identList : identifier ( "," identifier)* ;

procedureDeclaration : "PROCEDURE" identifier ( formalParameters )? ";" block identifier ;
formalParameters : "(" ( formalParameterList )? ")" ( ":" qualident )? ;
formalParameterList : formalParameter (";" formalParameter )* ;
formalParameter : ( "VAR" )? identList ":" qualident ;

statementSequence : statement ( ";" statement )* ;
statement : qualident ( ":=" expression | ( "(" ( expList )? ")" )? )
    | ifStatement 
    | whileStatement 
    | "RETURN" ( expression )? ;

ifStatement : "IF" expression "THEN" statementSequence ( "ELSE" statementSequence )? "END" ;

whileStatement : "WHILE" expression "DO" statementSequence "END" ;

expList : expression ( "," expression )* ;
expression : simpleExpression ( relation simpleExpression )? ;
relation : "=" | "#" | "<" | "<=" | ">" | ">=" ;
simpleExpression : ( "+" | "-" )? term ( addOperator term )* ;
addOperator : "+" | "-" | "OR" ;
term : factor ( mulOperator factor )* ;
mulOperator : "*" | "/" | "DIV" | "MOD" | "AND" ;
factor : integer_literal | "(" expression ")" | "NOT" factor | qualident ( "(" ( expList )? ")" )? ;