#ifndef CODE_H
#define CODE_H

#include <string>

using namespace std;

class Code {
  public:
    string dest(const string &mnemonic);
    string comp(const string &mnemonic);
    string jump(const string &mnemonic);
};
#endif // CODE_H
