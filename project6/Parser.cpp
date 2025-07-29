#include "Parser.h"
#include <algorithm>

using namespace std;

Parser::Parser(ifstream &file) : input(file) {}

bool Parser::hasMoreCommands() {
    return input.peek() != EOF;
}
void Parser::advance() {
    string line;
    while (getline(input, line)) {
        currentCommand = cleanLine(line);
        if (!currentCommand.empty())
            break;
    }
}

void Parser::restart() {
    input.clear();
    input.seekg(0, ios::beg);
}

char Parser::commandType() {
    if (currentCommand.empty())
        return 'N';
    if (currentCommand[0] == '@')
        return 'A';
    if (currentCommand[0] == '(')
        return 'L';
    return 'C';
}

string Parser::symbol() {
    if (commandType() == 'A')
        return currentCommand.substr(1);
    else if (commandType() == 'L')
        return currentCommand.substr(1, currentCommand.length() - 2);
    return "";
}

string Parser::dest() {
    size_t eqPos = currentCommand.find('=');
    if (eqPos != string::npos) {
        return currentCommand.substr(0, eqPos);
    }
    return "null";
}

string Parser::comp() {
    size_t eqPos = currentCommand.find('=');
    size_t scPos = currentCommand.find(';');
    if (eqPos != string::npos && scPos != string::npos)
        return currentCommand.substr(eqPos + 1, scPos - eqPos - 1);
    if (eqPos != string::npos)
        return currentCommand.substr(eqPos + 1);
    if (scPos != string::npos)
        return currentCommand.substr(0, scPos);
    return currentCommand;
}

string Parser::jump() {
    size_t scPos = currentCommand.find(';');
    if (scPos != string::npos)
        return currentCommand.substr(scPos + 1);
    return "null";
}

string Parser::cleanLine(const string &line) {
    string cleaned = line;
    size_t cmPos = cleaned.find("//");
    if (cmPos != string::npos)
        cleaned = cleaned.substr(0, cmPos);
    cleaned.erase(remove_if(cleaned.begin(), cleaned.end(), ::isspace),
                  cleaned.end());
    return cleaned;
}
