#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"
#include <fstream>
#include <string>

class CompilationEngine {
public:
    CompilationEngine(JackTokenizer* tokenizer, const std::string& outputName);

    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileSubroutineBody(const std::string& name, KeywordType subroutineType);
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    int compileExpressionList();

private:
    JackTokenizer* tokenizer;
    SymbolTable symbolTable;
    VMWriter vmWriter;

    std::string className;
    int labelCounter;

    void advance();
    void eat(TokenType type);
    void eatSymbol(char sym);
    void eatKeyword(KeywordType kw);
    Segment kindToSegment(Kind kind);

    bool isOp();
};

#endif
