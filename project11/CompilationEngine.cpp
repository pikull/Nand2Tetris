#include "CompilationEngine.h"

CompilationEngine::CompilationEngine(JackTokenizer* tokenizer, const std::string& outputName)
    : tokenizer(tokenizer), vmWriter(outputName), labelCounter(0) {
    if (tokenizer->hasMoreTokens()) { tokenizer->advance(); }
}

Segment CompilationEngine::kindToSegment(Kind kind) {
    switch (kind) {
    case KI_STATIC:
        return SEG_STATIC;
    case KI_FIELD:
        return SEG_THIS;
    case KI_ARG:
        return SEG_ARG;
    case KI_VAR:
        return SEG_LOCAL;
    default:
        return SEG_TEMP;
    }
}

void CompilationEngine::advance() {
    if (tokenizer->hasMoreTokens()) tokenizer->advance();
}

void CompilationEngine::eat(TokenType type) { advance(); }

void CompilationEngine::eatSymbol(char sym) { advance(); }

void CompilationEngine::eatKeyword(KeywordType kw) { advance(); }

void CompilationEngine::compileClass() {
    eatKeyword(K_CLASS);
    className = tokenizer->identifier();
    eat(IDENTIFIER);
    eatSymbol('{');

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

    eatSymbol('}');
    vmWriter.close();
}

void CompilationEngine::compileClassVarDec() {
    KeywordType k = tokenizer->keyword();
    Kind kind = (k == K_STATIC) ? KI_STATIC : KI_FIELD;
    advance();

    std::string type;
    if (tokenizer->tokenType() == KEYWORD) {
        type = tokenizer->identifier();
    } else {
        type = tokenizer->identifier();
    }
    advance();

    std::string name = tokenizer->identifier();
    symbolTable.define(name, type, kind);
    eat(IDENTIFIER);

    while (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ',') {
        eatSymbol(',');
        name = tokenizer->identifier();
        symbolTable.define(name, type, kind);
        eat(IDENTIFIER);
    }
    eatSymbol(';');
}

void CompilationEngine::compileSubroutine() {
    symbolTable.startSubroutine();

    KeywordType subroutineType = tokenizer->keyword();
    eatKeyword(subroutineType);

    advance();

    std::string name = tokenizer->identifier();
    eat(IDENTIFIER);

    std::string funcName = className + "." + name;

    eatSymbol('(');

    if (subroutineType == K_METHOD) { symbolTable.define("this", className, KI_ARG); }

    compileParameterList();
    eatSymbol(')');

    compileSubroutineBody(funcName, subroutineType);
}

void CompilationEngine::compileParameterList() {
    if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ')') return;

    std::string type = tokenizer->identifier();
    advance();

    std::string name = tokenizer->identifier();
    symbolTable.define(name, type, KI_ARG);
    eat(IDENTIFIER);

    while (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ',') {
        eatSymbol(',');
        type = tokenizer->identifier();
        advance();

        name = tokenizer->identifier();
        symbolTable.define(name, type, KI_ARG);
        eat(IDENTIFIER);
    }
}

void CompilationEngine::compileSubroutineBody(const std::string& name, KeywordType subroutineType) {
    eatSymbol('{');

    while (tokenizer->tokenType() == KEYWORD && tokenizer->keyword() == K_VAR) {
        compileVarDec();
    }

    int nLocals = symbolTable.varCount(KI_VAR);
    vmWriter.writeFunction(name, nLocals);

    if (subroutineType == K_CONSTRUCTOR) {
        int nFields = symbolTable.varCount(KI_FIELD);
        vmWriter.writePush(SEG_CONST, nFields);
        vmWriter.writeCall("Memory.alloc", 1);
        vmWriter.writePop(SEG_POINTER, 0);
    } else if (subroutineType == K_METHOD) {
        vmWriter.writePush(SEG_ARG, 0);
        vmWriter.writePop(SEG_POINTER, 0);
    }

    compileStatements();
    eatSymbol('}');
}

void CompilationEngine::compileVarDec() {
    eatKeyword(K_VAR);

    std::string type = tokenizer->identifier();
    advance();

    std::string name = tokenizer->identifier();
    symbolTable.define(name, type, KI_VAR);
    eat(IDENTIFIER);

    while (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ',') {
        eatSymbol(',');
        name = tokenizer->identifier();
        symbolTable.define(name, type, KI_VAR);
        eat(IDENTIFIER);
    }
    eatSymbol(';');
}

void CompilationEngine::compileStatements() {
    while (tokenizer->tokenType() == KEYWORD) {
        switch (tokenizer->keyword()) {
        case K_LET:
            compileLet();
            break;
        case K_IF:
            compileIf();
            break;
        case K_WHILE:
            compileWhile();
            break;
        case K_DO:
            compileDo();
            break;
        case K_RETURN:
            compileReturn();
            break;
        default:
            return;
        }
    }
}

void CompilationEngine::compileDo() {
    eatKeyword(K_DO);

    std::string name = tokenizer->identifier();
    eat(IDENTIFIER);
    int nArgs = 0;

    if (tokenizer->symbol() == '(') {
        vmWriter.writePush(SEG_POINTER, 0);
        eatSymbol('(');
        nArgs = compileExpressionList() + 1;
        eatSymbol(')');
        vmWriter.writeCall(className + "." + name, nArgs);
    } else if (tokenizer->symbol() == '.') {
        eatSymbol('.');
        std::string method = tokenizer->identifier();
        eat(IDENTIFIER);

        std::string type = symbolTable.typeOf(name);

        if (type != "") {
            Kind k = symbolTable.kindOf(name);
            int idx = symbolTable.indexOf(name);
            vmWriter.writePush(kindToSegment(k), idx);
            name = type + "." + method;
            nArgs = 1;
        } else {
            name = name + "." + method;
            nArgs = 0;
        }

        eatSymbol('(');
        nArgs += compileExpressionList();
        eatSymbol(')');
        vmWriter.writeCall(name, nArgs);
    }

    eatSymbol(';');
    vmWriter.writePop(SEG_TEMP, 0);
}

void CompilationEngine::compileLet() {
    eatKeyword(K_LET);
    std::string varName = tokenizer->identifier();
    eat(IDENTIFIER);

    bool isArray = false;

    if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == '[') {
        isArray = true;

        Kind k = symbolTable.kindOf(varName);
        int idx = symbolTable.indexOf(varName);
        vmWriter.writePush(kindToSegment(k), idx);

        eatSymbol('[');
        compileExpression();
        eatSymbol(']');

        vmWriter.writeArithmetic(CMD_ADD);
    }

    eatSymbol('=');
    compileExpression();
    eatSymbol(';');

    if (isArray) {
        vmWriter.writePop(SEG_TEMP, 0);
        vmWriter.writePop(SEG_POINTER, 1);
        vmWriter.writePush(SEG_TEMP, 0);
        vmWriter.writePop(SEG_THAT, 0);
    } else {
        Kind k = symbolTable.kindOf(varName);
        int idx = symbolTable.indexOf(varName);
        vmWriter.writePop(kindToSegment(k), idx);
    }
}

void CompilationEngine::compileWhile() {
    std::string l1 = "WHILE_EXP" + std::to_string(labelCounter++);
    std::string l2 = "WHILE_END" + std::to_string(labelCounter++);

    eatKeyword(K_WHILE);
    vmWriter.writeLabel(l1);

    eatSymbol('(');
    compileExpression();
    eatSymbol(')');

    vmWriter.writeArithmetic(CMD_NOT);
    vmWriter.writeIf(l2);

    eatSymbol('{');
    compileStatements();
    eatSymbol('}');

    vmWriter.writeGoto(l1);
    vmWriter.writeLabel(l2);
}

void CompilationEngine::compileReturn() {
    eatKeyword(K_RETURN);
    if (!(tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ';')) {
        compileExpression();
    } else {
        vmWriter.writePush(SEG_CONST, 0);
    }
    eatSymbol(';');
    vmWriter.writeReturn();
}

void CompilationEngine::compileIf() {
    std::string lTrue = "IF_TRUE" + std::to_string(labelCounter++);
    std::string lFalse = "IF_FALSE" + std::to_string(labelCounter++);
    std::string lEnd = "IF_END" + std::to_string(labelCounter++);

    eatKeyword(K_IF);
    eatSymbol('(');
    compileExpression();
    eatSymbol(')');

    vmWriter.writeIf(lTrue);
    vmWriter.writeGoto(lFalse);

    vmWriter.writeLabel(lTrue);
    eatSymbol('{');
    compileStatements();
    eatSymbol('}');

    if (tokenizer->tokenType() == KEYWORD && tokenizer->keyword() == K_ELSE) {
        vmWriter.writeGoto(lEnd);
        vmWriter.writeLabel(lFalse);
        eatKeyword(K_ELSE);
        eatSymbol('{');
        compileStatements();
        eatSymbol('}');
        vmWriter.writeLabel(lEnd);
    } else {
        vmWriter.writeLabel(lFalse);
    }
}

bool CompilationEngine::isOp() {
    if (tokenizer->tokenType() == SYMBOL) {
        char s = tokenizer->symbol();
        return (s == '+' || s == '-' || s == '*' || s == '/' || s == '&' || s == '|' || s == '<' || s == '>' ||
                s == '=');
    }
    return false;
}

void CompilationEngine::compileExpression() {
    compileTerm();
    while (isOp()) {
        char op = tokenizer->symbol();
        eatSymbol(op);
        compileTerm();

        switch (op) {
        case '+':
            vmWriter.writeArithmetic(CMD_ADD);
            break;
        case '-':
            vmWriter.writeArithmetic(CMD_SUB);
            break;
        case '*':
            vmWriter.writeCall("Math.multiply", 2);
            break;
        case '/':
            vmWriter.writeCall("Math.divide", 2);
            break;
        case '&':
            vmWriter.writeArithmetic(CMD_AND);
            break;
        case '|':
            vmWriter.writeArithmetic(CMD_OR);
            break;
        case '<':
            vmWriter.writeArithmetic(CMD_LT);
            break;
        case '>':
            vmWriter.writeArithmetic(CMD_GT);
            break;
        case '=':
            vmWriter.writeArithmetic(CMD_EQ);
            break;
        }
    }
}

void CompilationEngine::compileTerm() {
    TokenType type = tokenizer->tokenType();

    if (type == INT_CONST) {
        vmWriter.writePush(SEG_CONST, tokenizer->intVal());
        advance();
    } else if (type == STRING_CONST) {
        std::string s = tokenizer->stringVal();
        vmWriter.writePush(SEG_CONST, s.length());
        vmWriter.writeCall("String.new", 1);
        for (char c : s) {
            vmWriter.writePush(SEG_CONST, (int)c);
            vmWriter.writeCall("String.appendChar", 2);
        }
        advance();
    } else if (type == KEYWORD) {
        KeywordType k = tokenizer->keyword();
        if (k == K_TRUE) {
            vmWriter.writePush(SEG_CONST, 0);
            vmWriter.writeArithmetic(CMD_NOT);
        } else if (k == K_FALSE || k == K_NULL) {
            vmWriter.writePush(SEG_CONST, 0);
        } else if (k == K_THIS) {
            vmWriter.writePush(SEG_POINTER, 0);
        }
        advance();
    } else if (type == SYMBOL) {
        char s = tokenizer->symbol();
        if (s == '(') {
            eatSymbol('(');
            compileExpression();
            eatSymbol(')');
        } else if (s == '-' || s == '~') {
            eatSymbol(s);
            compileTerm();
            if (s == '-') vmWriter.writeArithmetic(CMD_NEG);
            else vmWriter.writeArithmetic(CMD_NOT);
        }
    } else if (type == IDENTIFIER) {
        std::string name = tokenizer->identifier();
        advance();

        if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == '[') {
            Kind k = symbolTable.kindOf(name);
            int idx = symbolTable.indexOf(name);
            vmWriter.writePush(kindToSegment(k), idx);

            eatSymbol('[');
            compileExpression();
            eatSymbol(']');

            vmWriter.writeArithmetic(CMD_ADD);
            vmWriter.writePop(SEG_POINTER, 1);
            vmWriter.writePush(SEG_THAT, 0);
        } else if (tokenizer->tokenType() == SYMBOL && (tokenizer->symbol() == '(' || tokenizer->symbol() == '.')) {
            int nArgs = 0;
            if (tokenizer->symbol() == '(') {
                vmWriter.writePush(SEG_POINTER, 0);
                eatSymbol('(');
                nArgs = compileExpressionList() + 1;
                eatSymbol(')');
                vmWriter.writeCall(className + "." + name, nArgs);
            } else if (tokenizer->symbol() == '.') {
                eatSymbol('.');
                std::string method = tokenizer->identifier();
                eat(IDENTIFIER);

                std::string type = symbolTable.typeOf(name);
                if (type != "") {
                    Kind k = symbolTable.kindOf(name);
                    int idx = symbolTable.indexOf(name);
                    vmWriter.writePush(kindToSegment(k), idx);
                    name = type + "." + method;
                    nArgs = 1;
                } else {
                    name = name + "." + method;
                    nArgs = 0;
                }

                eatSymbol('(');
                nArgs += compileExpressionList();
                eatSymbol(')');
                vmWriter.writeCall(name, nArgs);
            }
        } else {
            Kind k = symbolTable.kindOf(name);
            if (k != KI_NONE) { vmWriter.writePush(kindToSegment(k), symbolTable.indexOf(name)); }
        }
    }
}

int CompilationEngine::compileExpressionList() {
    int nArgs = 0;
    if (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ')') return 0;

    compileExpression();
    nArgs++;

    while (tokenizer->tokenType() == SYMBOL && tokenizer->symbol() == ',') {
        eatSymbol(',');
        compileExpression();
        nArgs++;
    }
    return nArgs;
}
