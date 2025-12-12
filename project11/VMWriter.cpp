#include "VMWriter.h"
#include <unordered_map>

const std::unordered_map<Segment, std::string> VMWriter::segmentToString = {
    {SEG_CONST, "constant"}, {SEG_ARG, "argument"}, {SEG_LOCAL, "local"},
    {SEG_STATIC, "static"}, {SEG_THIS, "this"}, {SEG_THAT, "that"},
    {SEG_POINTER, "pointer"}, {SEG_TEMP, "temp"}
};

const std::unordered_map<Command, std::string> VMWriter::commandToString = {
    {CMD_ADD, "add"}, {CMD_SUB, "sub"}, {CMD_NEG, "neg"}, {CMD_EQ, "eq"},
    {CMD_GT, "gt"}, {CMD_LT, "lt"}, {CMD_AND, "and"}, {CMD_OR, "or"},
    {CMD_NOT, "not"}
};

VMWriter::VMWriter(const std::string& outputName) {
    outFile.open(outputName);
}

void VMWriter::close() {
    if (outFile.is_open()) outFile.close();
}

void VMWriter::writePush(Segment segment, int index) {
    outFile << "push " << segmentToString.find(segment)->second << " " << index << '\n';
}

void VMWriter::writePop(Segment segment, int index) {
    outFile << "pop " << segmentToString.find(segment)->second << " " << index << '\n';
}

void VMWriter::writeArithmetic(Command command) {
    outFile << commandToString.find(command)->second << '\n';
}

void VMWriter::writeLabel(const std::string& label) {
    outFile << "label " << label << '\n';
}

void VMWriter::writeGoto(const std::string& label) {
    outFile << "goto " << label << '\n';
}

void VMWriter::writeIf(const std::string& label) {
    outFile << "if-goto " << label << '\n';
}

void VMWriter::writeCall(const std::string& name, int nArgs) {
    outFile << "call " << name << " " << nArgs << '\n';
}

void VMWriter::writeFunction(const std::string& name, int nLocals) {
    outFile << "function " << name << " " << nLocals << '\n';
}

void VMWriter::writeReturn() {
    outFile << "return\n";
}
