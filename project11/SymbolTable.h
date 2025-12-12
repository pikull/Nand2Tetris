#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

enum Kind { KI_STATIC, KI_FIELD, KI_ARG, KI_VAR, KI_NONE };

struct Symbol {
    std::string type;
    Kind kind;
    int index;
};

class SymbolTable {
public:
    SymbolTable();
    void startSubroutine();
    void define(const std::string& name, const std::string& type, Kind kind);
    int varCount(Kind kind);
    Kind kindOf(const std::string& name);
    std::string typeOf(const std::string& name);
    int indexOf(const std::string& name);

private:
    std::unordered_map<std::string, Symbol> classScope;
    std::unordered_map<std::string, Symbol> subroutineScope;
    std::unordered_map<Kind, int> counts;
};
#endif
