#ifndef VMWRITER_H
#define VMWRITER_H

#include <fstream>
#include <string>
#include <unordered_map>

enum Segment { SEG_CONST, SEG_ARG, SEG_LOCAL, SEG_STATIC, SEG_THIS, SEG_THAT, SEG_POINTER, SEG_TEMP };
enum Command { CMD_ADD, CMD_SUB, CMD_NEG, CMD_EQ, CMD_GT, CMD_LT, CMD_AND, CMD_OR, CMD_NOT };

class VMWriter {
public:
    VMWriter(const std::string& outputName);
    void close();
    void writePush(Segment segment, int index);
    void writePop(Segment segment, int index);
    void writeArithmetic(Command command);
    void writeLabel(const std::string& label);
    void writeGoto(const std::string& label);
    void writeIf(const std::string& label);
    void writeCall(const std::string& name, int nArgs);
    void writeFunction(const std::string& name, int nLocals);
    void writeReturn();

private:
    std::ofstream outFile;
    static const std::unordered_map<Segment, std::string> segmentToString;
    static const std::unordered_map<Command, std::string> commandToString;
};

#endif
