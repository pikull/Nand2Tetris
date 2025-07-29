#include "Code.h"
#include "Parser.h"
#include "SymbolTable.h"
#include <algorithm>
#include <bitset>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[]) {
    string name = argv[1];
    ifstream ifile(name);
    if (!ifile.is_open())
        return 1;

    Parser p(ifile);
    SymbolTable s;
    Code c;

    // first pass for symbols
    int lineNum = 0;
    while (p.hasMoreCommands()) {
        p.advance();
        if (p.commandType() == 'L') {
            if (s.getAddress(p.symbol()) == -1)
                s.add(p.symbol(), lineNum);
        } else {
            ++lineNum;
        }
    }

    ofstream ofile(name.substr(0, name.length() - 4) + ".hack");
    if (!ofile.is_open())
        return 1;

    // second pass for translation
    int nextAddress = 16;
    p.restart();
    while (p.hasMoreCommands()) {
        p.advance();
        switch (p.commandType()) {
        case 'A': {
            ofile << '0';
            string sym = p.symbol();
            int address;
            if (!all_of(sym.begin(), sym.end(), ::isdigit)) {
                int existing = s.getAddress(sym);
                if (existing == -1) {
                    s.add(sym, nextAddress);
                    address = nextAddress++;
                } else {
                    address = existing;
                }
            } else {
                address = stoi(sym);
            }
            ofile << bitset<15>(address);
            ofile << endl;
            break;
        }
        case 'C':
            ofile << "111";
            ofile << c.comp(p.comp());
            ofile << c.dest(p.dest());
            ofile << c.jump(p.jump());
            ofile << endl;
            break;
        default:
            break;
        }
    }

    return 0;
}
