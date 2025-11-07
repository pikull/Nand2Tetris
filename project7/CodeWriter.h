#ifndef CODEWRITER_H
#define CODEWRITER_H

#include "command.h"
#include <fstream>

using namespace std;

class CodeWriter {
public:
    CodeWriter(ofstream &file);
    void setFileName(const string &name);
    void writeArithmetic(const string &command);
    void writePushPop(const CommandType &commandType, const string &segment, const int &index) const;
    void close() const;

private:
    ofstream &output;
    string fileName;
    unsigned int labelCount;
};
#endif // CODEWRITER_H
