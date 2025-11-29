#include "Parser.h"
#include "command.h"
#include <sstream>

using namespace std;

Parser::Parser(ifstream& file) : input(file) {
    commands = {
        {"add", C_ARITHMETIC}, {"sub", C_ARITHMETIC}, {"neg", C_ARITHMETIC},    {"eq", C_ARITHMETIC},
        {"gt", C_ARITHMETIC},  {"lt", C_ARITHMETIC},  {"and", C_ARITHMETIC},    {"or", C_ARITHMETIC},
        {"not", C_ARITHMETIC}, {"push", C_PUSH},      {"pop", C_POP},           {"label", C_LABEL},
        {"goto", C_GOTO},      {"if-goto", C_IF},     {"function", C_FUNCTION}, {"return", C_RETURN},
        {"call", C_CALL},
    };
}

bool Parser::hasMoreCommands() const { return input.peek() != EOF; }

void Parser::advance() {
    string line;
    while (getline(input, line)) {
        currentLine = cleanLine(line);
        if (!currentLine.empty()) break;
    }
}

CommandType Parser::commandType() const {
    istringstream curr(currentLine);
    string command;
    curr >> command;
    return commands.at(command);
}

string Parser::arg1() const {
    istringstream curr(currentLine);
    string arg;
    curr >> arg;
    if (commandType() == C_ARITHMETIC) { return arg; }
    curr >> arg;
    return arg;
}

int Parser::arg2() const {
    istringstream curr(currentLine);
    string skip;
    int arg;
    curr >> skip >> skip >> arg;
    return arg;
}

string Parser::cleanLine(const string& line) const {
    string cleaned = line;

    size_t comment = cleaned.find("//");
    if (comment != string::npos) cleaned = cleaned.substr(0, comment);

    size_t start = cleaned.find_first_not_of(" \t\r\n\f\v");
    if (start == string::npos) return "";

    size_t end = cleaned.find_last_not_of(" \t\r\n\f\v");
    cleaned = cleaned.substr(start, end - start + 1);

    return cleaned;
}
