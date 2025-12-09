#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "JackTokenizer.h"
#include <fstream>
#include <string>

class CompilationEngine {
public:
    CompilationEngine(JackTokenizer* tokenizer, const std::string& outputName);
    ~CompilationEngine();

    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();

private:
    JackTokenizer* tokenizer;
    std::ofstream outFile;
    int indentLevel;

    static const std::unordered_map<KeywordType, std::string> keywordToString;
    static const std::unordered_map<char, std::string> symbolTable;

    void printIndent();
    void process(const std::string& str);
    void process(TokenType type);
    void processKeyword(KeywordType kw);
    void processSymbol(char sym);
    
    void writeCurrentToken();
    
    bool isOp();
};

#endif
