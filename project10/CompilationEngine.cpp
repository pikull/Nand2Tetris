#include "CompilationEngine.h"
#include <iostream>

const std::unordered_map<KeywordType, std::string> CompilationEngine::keywordToString = {
    {K_CLASS, "class"}, {K_METHOD, "method"}, {K_FUNCTION, "function"},
    {K_CONSTRUCTOR, "constructor"}, {K_INT, "int"}, {K_BOOLEAN, "boolean"},
    {K_CHAR, "char"}, {K_VOID, "void"}, {K_VAR, "var"}, {K_STATIC, "static"},
    {K_FIELD, "field"}, {K_LET, "let"}, {K_DO, "do"}, {K_IF, "if"},
    {K_ELSE, "else"}, {K_WHILE, "while"}, {K_RETURN, "return"},
    {K_TRUE, "true"}, {K_FALSE, "false"}, {K_NULL, "null"}, {K_THIS, "this"}
};
const std::unordered_map<char, std::string> CompilationEngine::symbolTable = {
    {'<',  "&lt;"},
    {'>',  "&gt;"},
    {'&',  "&amp;"},
    {'"',  "&quot;"}
};

CompilationEngine::CompilationEngine(JackTokenizer* tokenizer, const std::string& outputName)
    : tokenizer(tokenizer), indentLevel(0) {
    outFile.open(outputName);
    if (tokenizer->hasMoreTokens()) {
        tokenizer->advance();
    }
}

CompilationEngine::~CompilationEngine() {
    if (outFile.is_open()) {
        outFile.close();
    }
}

void CompilationEngine::printIndent() {
    for (int i = 0; i < indentLevel; ++i) {
        outFile << "  ";
    }
}

void CompilationEngine::writeCurrentToken() {
    printIndent();
    TokenType type = tokenizer->tokenType();
    
    switch (type) {
        case KEYWORD: {
            std::string kwStr;
            KeywordType k = tokenizer->keyword();
            auto it = keywordToString.find(k);
            const std::string& kwstr = (it != keywordToString.end()) ? it->second : "";
            outFile << "<keyword> " << kwStr << " </keyword>\n";
            break;
        }
        case SYMBOL: {
            char sym = tokenizer->symbol();
            outFile << "<symbol> ";
            auto it = symbolTable.find(sym);
            if (it != symbolTable.end())
            outFile << it->second;
            else
                outFile << sym;
            outFile << " </symbol>\n";
            break;
        }
        case IDENTIFIER:
            outFile << "<identifier> " << tokenizer->identifier() << " </identifier>\n";
            break;
        case INT_CONST:
            outFile << "<integerConstant> " << tokenizer->intVal() << " </integerConstant>\n";
            break;
        case STRING_CONST:
            outFile << "<stringConstant> " << tokenizer->stringVal() << " </stringConstant>\n";
            break;
    }
}

void CompilationEngine::process(const std::string& str) {
    writeCurrentToken();
    if (tokenizer->hasMoreTokens()) tokenizer->advance();
}

void CompilationEngine::process(TokenType type) {
    writeCurrentToken();
    if (tokenizer->hasMoreTokens()) tokenizer->advance();
}

void CompilationEngine::processSymbol(char sym) {
    writeCurrentToken(); 
    if (tokenizer->hasMoreTokens()) tokenizer->advance();
}

void CompilationEngine::processKeyword(KeywordType kw) {
    writeCurrentToken();
    if (tokenizer->hasMoreTokens()) tokenizer->advance();
}

void CompilationEngine::compileClass() {
    printIndent(); outFile << "<class>\n";
    indentLevel++;

    processKeyword(K_CLASS);
    process(IDENTIFIER);
    processSymbol('{');

    while (tokenizer->tokenType() == KEYWORD) {
        KeywordType k = tokenizer->keyword();
        if (k == K_STATIC || k == K_FIELD) {
            compileClassVarDec();
        } else {
            break;
        }
    }

    while (tokenizer->tokenType() == KEYWORD) {
        KeywordType k = tokenizer->keyword();
        if (k == K_CONSTRUCTOR || k == K_FUNCTION || k == K_METHOD) {
            compileSubroutine();
        } else {
            break;
        }
    }

    processSymbol('}');

    indentLevel--;
    printIndent(); outFile << "</class>\n";
}

void CompilationEngine::compileClassVarDec() {
    printIndent(); outFile << "<classVarDec>\n";
    indentLevel++;

    writeCurrentToken(); 
    if(tokenizer->hasMoreTokens()) tokenizer->advance();

    writeCurrentToken(); 
    if(tokenizer->hasMoreTokens()) tokenizer->advance();

    process(IDENTIFIER);

    while (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ',') {
        processSymbol(',');
        process(IDENTIFIER);
    }

    processSymbol(';');

    indentLevel--;
    printIndent(); outFile << "</classVarDec>\n";
}

void CompilationEngine::compileSubroutine() {
    printIndent(); outFile << "<subroutineDec>\n";
    indentLevel++;

    writeCurrentToken();
    if(tokenizer->hasMoreTokens()) tokenizer->advance();

    writeCurrentToken();
    if(tokenizer->hasMoreTokens()) tokenizer->advance();

    process(IDENTIFIER);

    processSymbol('(');
    compileParameterList();
    processSymbol(')');

    compileSubroutineBody();

    indentLevel--;
    printIndent(); outFile << "</subroutineDec>\n";
}

void CompilationEngine::compileParameterList() {
    printIndent(); outFile << "<parameterList>\n";
    indentLevel++;

    bool hasParam = false;
    if (tokenizer->tokenType() == KEYWORD || tokenizer->tokenType() == IDENTIFIER) {
        hasParam = true;
    }

    if (hasParam) {
        writeCurrentToken();
        if(tokenizer->hasMoreTokens()) tokenizer->advance();
        
        process(IDENTIFIER);

        while (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ',') {
            processSymbol(',');
            
            writeCurrentToken();
            if(tokenizer->hasMoreTokens()) tokenizer->advance();
            
            process(IDENTIFIER);
        }
    }

    indentLevel--;
    printIndent(); outFile << "</parameterList>\n";
}

void CompilationEngine::compileSubroutineBody() {
    printIndent(); outFile << "<subroutineBody>\n";
    indentLevel++;

    processSymbol('{');

    while (tokenizer->tokenType() == KEYWORD && tokenizer->keyword() == K_VAR) {
        compileVarDec();
    }

    compileStatements();

    processSymbol('}');

    indentLevel--;
    printIndent(); outFile << "</subroutineBody>\n";
}

void CompilationEngine::compileVarDec() {
    printIndent(); outFile << "<varDec>\n";
    indentLevel++;

    processKeyword(K_VAR);
    
    writeCurrentToken();
    if(tokenizer->hasMoreTokens()) tokenizer->advance();

    process(IDENTIFIER);

    while (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ',') {
        processSymbol(',');
        process(IDENTIFIER);
    }

    processSymbol(';');

    indentLevel--;
    printIndent(); outFile << "</varDec>\n";
}

void CompilationEngine::compileStatements() {
    printIndent(); outFile << "<statements>\n";
    indentLevel++;

    bool checking = true;
    while (checking && tokenizer->tokenType() == KEYWORD) {
        switch (tokenizer->keyword()) {
            case K_LET: compileLet(); break;
            case K_IF: compileIf(); break;
            case K_WHILE: compileWhile(); break;
            case K_DO: compileDo(); break;
            case K_RETURN: compileReturn(); break;
            default: checking = false; break;
        }
    }

    indentLevel--;
    printIndent(); outFile << "</statements>\n";
}

void CompilationEngine::compileDo() {
    printIndent(); outFile << "<doStatement>\n";
    indentLevel++;

    processKeyword(K_DO);
    
    process(IDENTIFIER);
    
    if (tokenizer->symbol() == '(') {
        processSymbol('(');
        compileExpressionList();
        processSymbol(')');
    } else if (tokenizer->symbol() == '.') {
        processSymbol('.');
        process(IDENTIFIER);
        processSymbol('(');
        compileExpressionList();
        processSymbol(')');
    }

    processSymbol(';');

    indentLevel--;
    printIndent(); outFile << "</doStatement>\n";
}

void CompilationEngine::compileLet() {
    printIndent(); outFile << "<letStatement>\n";
    indentLevel++;

    processKeyword(K_LET);
    process(IDENTIFIER);

    if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == '[') {
        processSymbol('[');
        compileExpression();
        processSymbol(']');
    }

    processSymbol('=');
    compileExpression();
    processSymbol(';');

    indentLevel--;
    printIndent(); outFile << "</letStatement>\n";
}

void CompilationEngine::compileWhile() {
    printIndent(); outFile << "<whileStatement>\n";
    indentLevel++;

    processKeyword(K_WHILE);
    processSymbol('(');
    compileExpression();
    processSymbol(')');
    
    processSymbol('{');
    compileStatements();
    processSymbol('}');

    indentLevel--;
    printIndent(); outFile << "</whileStatement>\n";
}

void CompilationEngine::compileReturn() {
    printIndent(); outFile << "<returnStatement>\n";
    indentLevel++;

    processKeyword(K_RETURN);
    
    if (!(tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ';')) {
        compileExpression();
    }

    processSymbol(';');

    indentLevel--;
    printIndent(); outFile << "</returnStatement>\n";
}

void CompilationEngine::compileIf() {
    printIndent(); outFile << "<ifStatement>\n";
    indentLevel++;

    processKeyword(K_IF);
    processSymbol('(');
    compileExpression();
    processSymbol(')');
    
    processSymbol('{');
    compileStatements();
    processSymbol('}');

    if (tokenizer->tokenType() == KEYWORD && tokenizer->keyword() == K_ELSE) {
        processKeyword(K_ELSE);
        processSymbol('{');
        compileStatements();
        processSymbol('}');
    }

    indentLevel--;
    printIndent(); outFile << "</ifStatement>\n";
}

bool CompilationEngine::isOp() {
    if (tokenizer->tokenType() == SYMBOL) {
        char s = tokenizer->symbol();
        return (s == '+' || s == '-' || s == '*' || s == '/' || s == '&' || 
                s == '|' || s == '<' || s == '>' || s == '=');
    }
    return false;
}

void CompilationEngine::compileExpression() {
    printIndent(); outFile << "<expression>\n";
    indentLevel++;

    compileTerm();

    while (isOp()) {
        processSymbol(tokenizer->symbol());
        compileTerm();
    }

    indentLevel--;
    printIndent(); outFile << "</expression>\n";
}

void CompilationEngine::compileTerm() {
    printIndent(); outFile << "<term>\n";
    indentLevel++;

    TokenType type = tokenizer->tokenType();

    if (type == INT_CONST || type == STRING_CONST) {
        writeCurrentToken();
        if(tokenizer->hasMoreTokens()) tokenizer->advance();
    } 
    else if (type == KEYWORD) {
        writeCurrentToken();
        if(tokenizer->hasMoreTokens()) tokenizer->advance();
    }
    else if (type == SYMBOL) {
        char s = tokenizer->symbol();
        if (s == '(') {
            processSymbol('(');
            compileExpression();
            processSymbol(')');
        } 
        else if (s == '-' || s == '~') {
            processSymbol(s);
            compileTerm();
        }
    }
    else if (type == IDENTIFIER) {
        writeCurrentToken();
        if(tokenizer->hasMoreTokens()) tokenizer->advance();

        if (tokenizer->tokenType() == SYMBOL) {
            char nextSym = tokenizer->symbol();
            
            if (nextSym == '[') {
                processSymbol('[');
                compileExpression();
                processSymbol(']');
            } 
            else if (nextSym == '(') {
                processSymbol('(');
                compileExpressionList();
                processSymbol(')');
            }
            else if (nextSym == '.') {
                processSymbol('.');
                process(IDENTIFIER);
                processSymbol('(');
                compileExpressionList();
                processSymbol(')');
            }
        }
    }

    indentLevel--;
    printIndent(); outFile << "</term>\n";
}

void CompilationEngine::compileExpressionList() {
    printIndent(); outFile << "<expressionList>\n";
    indentLevel++;

    if (!(tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ')')) {
        compileExpression();
        while (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ',') {
            processSymbol(',');
            compileExpression();
        }
    }

    indentLevel--;
    printIndent(); outFile << "</expressionList>\n";
}
