#include "CodeWriter.h"
#include "Parser.h"
#include "command.h"
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "incorrect usage" << endl;
        return 1;
    }

    string name = argv[1];

    // open in file, parser
    ifstream in(name);
    if (!in.is_open()) {
        cerr << "could not open " << name << endl;
        return 2;
    }
    Parser p(in);

    // make name for outfile
    size_t dot = name.find_last_of('.');
    name = (dot != string::npos ? name.substr(0, dot) : name) + ".asm";

    // open out file, codewriter
    ofstream out(name);
    if (!out.is_open()) {
        in.close();
        cerr << "could not open " << name << endl;
        return 3;
    }
    CodeWriter cw(out);
    cw.setFileName(name);

    while (p.hasMoreCommands()) {
        p.advance();
        CommandType ct = p.commandType();

        if (ct == C_ARITHMETIC) cw.writeArithmetic(p.arg1());
        else if (ct == C_PUSH || ct == C_POP) cw.writePushPop(ct, p.arg1(), p.arg2());
    }

    cw.close();
    in.close();

    return 0;
}
