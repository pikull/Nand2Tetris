#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    counts[KI_STATIC] = 0;
    counts[KI_FIELD] = 0;
    counts[KI_ARG] = 0;
    counts[KI_VAR] = 0;
}

void SymbolTable::startSubroutine() {
    subroutineScope.clear();
    counts[KI_ARG] = 0;
    counts[KI_VAR] = 0;
}

void SymbolTable::define(const std::string& name, const std::string& type, Kind kind) {
    Symbol s = {type, kind, counts[kind]++};
    if (kind == KI_STATIC || kind == KI_FIELD) {
        classScope[name] = s;
    } else {
        subroutineScope[name] = s;
    }
}

int SymbolTable::varCount(Kind kind) {
    return counts[kind];
}

Kind SymbolTable::kindOf(const std::string& name) {
    if (subroutineScope.count(name)) return subroutineScope[name].kind;
    if (classScope.count(name)) return classScope[name].kind;
    return KI_NONE;
}

std::string SymbolTable::typeOf(const std::string& name) {
    if (subroutineScope.count(name)) return subroutineScope[name].type;
    if (classScope.count(name)) return classScope[name].type;
    return "";
}

int SymbolTable::indexOf(const std::string& name) {
    if (subroutineScope.count(name)) return subroutineScope[name].index;
    if (classScope.count(name)) return classScope[name].index;
    return -1;
}
