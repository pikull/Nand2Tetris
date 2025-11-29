#ifndef PARSER_H
#define PARSER_H

#include "command.h"
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

class Parser {
public:
    Parser(ifstream& file);
    bool hasMoreCommands() const;
    void advance();
    CommandType commandType() const;
    string arg1() const;
    int arg2() const;

private:
    ifstream& input;
    string currentLine;
    string cleanLine(const string& line) const;
    unordered_map<string, CommandType> commands;
};
#endif // PARSER_H
