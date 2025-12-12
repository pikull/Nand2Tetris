#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H

#include "keyword.h"
#include "token.h"
#include <fstream>
#include <string>
#include <unordered_map>

class JackTokenizer {
public:
    JackTokenizer(const std::string& filename);

    bool hasMoreTokens() const;
    void advance();
    TokenType tokenType() const;

    KeywordType keyword() const;
    char symbol() const;
    std::string identifier() const;
    int intVal() const;
    std::string stringVal() const;

private:
    std::string file;
    size_t pos;
    std::string current;
    TokenType currentType;

    static const std::unordered_map<std::string, KeywordType> keywordTable;

    void skip();
    bool isSymChar(char c) const;
};
#endif
