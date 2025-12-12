#include "JackTokenizer.h"
#include "token.h"
#include <sstream>

const std::unordered_map<std::string, KeywordType> JackTokenizer::keywordTable = {
    {"class", K_CLASS},   {"method", K_METHOD},   {"function", K_FUNCTION}, {"constructor", K_CONSTRUCTOR},
    {"int", K_INT},       {"boolean", K_BOOLEAN}, {"char", K_CHAR},         {"void", K_VOID},
    {"var", K_VAR},       {"static", K_STATIC},   {"field", K_FIELD},       {"let", K_LET},
    {"do", K_DO},         {"if", K_IF},           {"else", K_ELSE},         {"while", K_WHILE},
    {"return", K_RETURN}, {"true", K_TRUE},       {"false", K_FALSE},       {"null", K_NULL},
    {"this", K_THIS}};

JackTokenizer::JackTokenizer(const std::string& filename) : pos(0) {
    std::ifstream in(filename);
    std::stringstream buffer;
    buffer << in.rdbuf();
    file = buffer.str();
}

bool JackTokenizer::hasMoreTokens() const {
    size_t p = pos;
    while (p < file.size() && isspace(file[p]))
        ++p;
    return p < file.size();
}

void JackTokenizer::advance() {
    skip();
    if (pos >= file.size()) return;

    char c = file[pos];

    current.clear();
    // symbol
    if (isSymChar(c)) {
        currentType = SYMBOL;
        current = c;
        ++pos;
        return;
    }
    // string const
    if (c == '"') {
        currentType = STRING_CONST;
        ++pos;
        while (pos < file.size() && file[pos] != '"') {
            current.push_back(file[pos]);
            pos++;
        }
        ++pos;
        return;
    }
    // int const
    if (isdigit(c)) {
        currentType = INT_CONST;
        while (pos < file.size() && isdigit(file[pos])) {
            current.push_back(file[pos]);
            ++pos;
        }
        return;
    }
    // id/kw
    if (isalpha(c) || c == '_') {
        while (pos < file.size() && (isalnum(file[pos]) || file[pos] == '_')) {
            current.push_back(file[pos]);
            ++pos;
        }
        if (keywordTable.count(current)) currentType = KEYWORD;
        else currentType = IDENTIFIER;
        return;
    }
}
TokenType JackTokenizer::tokenType() const { return currentType; }

KeywordType JackTokenizer::keyword() const { return keywordTable.at(current); }

char JackTokenizer::symbol() const { return current[0]; }

std::string JackTokenizer::identifier() const { return current; }

int JackTokenizer::intVal() const { return std::stoi(current); }

std::string JackTokenizer::stringVal() const { return current; }

void JackTokenizer::skip() {
    while (pos < file.size()) {
        // whitespace
        if (isspace(file[pos])) {
            ++pos;
            continue;
        }
        // line comment
        if (pos + 1 < file.size() && file[pos] == '/' && file[pos + 1] == '/') {
            pos += 2;
            while (pos < file.size() && file[pos] != '\n')
                pos++;
            continue;
        }
        // block comment
        if (pos + 1 < file.size() && file[pos] == '/' && file[pos + 1] == '*') {
            pos += 2;
            while (pos + 1 < file.size() && !(file[pos] == '*' && file[pos + 1] == '/'))
                pos++;
            pos += 2; // skip closing */
            continue;
        }

        break;
    }
}

bool JackTokenizer::isSymChar(char c) const {
    static const std::string sy = "{}()[].,;+-*/&|<>=~";
    return sy.find(c) != std::string::npos;
}
