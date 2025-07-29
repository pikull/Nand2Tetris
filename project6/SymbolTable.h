#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>

using namespace std;

class SymbolTable {
  public:
    SymbolTable();
    void add(const string &element, int address);
    int getAddress(const string &element);

  private:
    unordered_map<string, int> table;
};
#endif // SYMBOLTABLE_H
