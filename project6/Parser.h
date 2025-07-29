#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>

using namespace std;

class Parser {
  public:
    Parser(ifstream &file);
    bool hasMoreCommands();
    void advance();
    void restart();
    char commandType();
    string symbol();
    string dest();
    string comp();
    string jump();

  private:
    ifstream &input;
    string currentCommand;
    string cleanLine(const string &line);
};
#endif // PARSER_H
