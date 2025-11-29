#include "CodeWriter.h"
#include "Parser.h"
#include "command.h"
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

void processFile(fs::path filePath, CodeWriter& cw) {
    ifstream in(filePath);
    if (!in.is_open()) {
        cerr << "Could not open " << filePath << endl;
        return;
    }

    cw.setFileName(filePath.stem().string());

    Parser p(in);
    while (p.hasMoreCommands()) {
        p.advance();
        CommandType ct = p.commandType();

        if (ct == C_ARITHMETIC) cw.writeArithmetic(p.arg1());
        else if (ct == C_PUSH || ct == C_POP) cw.writePushPop(ct, p.arg1(), p.arg2());
        else if (ct == C_LABEL) cw.writeLabel(p.arg1());
        else if (ct == C_GOTO) cw.writeGoto(p.arg1());
        else if (ct == C_IF) cw.writeIf(p.arg1());
        else if (ct == C_FUNCTION) cw.writeFunction(p.arg1(), p.arg2());
        else if (ct == C_CALL) cw.writeCall(p.arg1(), p.arg2());
        else if (ct == C_RETURN) cw.writeReturn();
    }
    in.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "incorrect usage" << endl;
        return 1;
    }

    fs::path inputPath(argv[1]);
    fs::path outputPath;

    if (fs::is_directory(inputPath)) {
        string dirName = inputPath.filename().string();
        if (dirName.empty()) dirName = inputPath.parent_path().filename().string();
        outputPath = inputPath / (dirName + ".asm");
    } else {
        string fileName = inputPath.stem().string();
        outputPath = inputPath.parent_path() / (fileName + ".asm");
    }

    ofstream out(outputPath);
    if (!out.is_open()) {
        cerr << "Could not open output file: " << outputPath << endl;
        return 3;
    }

    CodeWriter cw(out);

    if (fs::is_directory(inputPath)) {
        cw.writeInit();

        for (const auto& entry : fs::directory_iterator(inputPath)) {
            if (entry.path().extension() == ".vm") processFile(entry.path(), cw);
        }
    } else processFile(inputPath, cw);

    cw.close();
    return 0;
}
