#include "CodeWriter.h"

using namespace std;

CodeWriter::CodeWriter(ofstream& file) : output(file) { labelCount = 0; }

void CodeWriter::setFileName(const string& name) {
    size_t dot = name.find_last_of('.');
    fileName = (dot != string::npos ? name.substr(0, dot) : name);
    size_t slash = fileName.find_last_of("/\\");
    fileName = (slash != string::npos ? fileName.substr(slash + 1) : fileName);
}

void CodeWriter::writeArithmetic(const string& command) {
    if (command == "add" || command == "sub" || command == "and" || command == "or") {

        output << "@SP" << endl;    // go to stack pointer
        output << "AM=M-1" << endl; // A=M -> last filled memory
        output << "D=M" << endl;    // store last variable (y)
        output << "A=A-1" << endl;  // A -> second-last variable (x)

        if (command == "add") output << "M=M+D" << endl;
        else if (command == "sub") output << "M=M-D" << endl;
        else if (command == "and") output << "M=M&D" << endl;
        else if (command == "or") output << "M=M|D" << endl;

    } else if (command == "neg" || command == "not") {

        output << "@SP" << endl;   // go to stack pointer
        output << "A=M-1" << endl; // last filled memory

        if (command == "neg") output << "M=-M" << endl;
        else if (command == "not") output << "M=!M" << endl;

    } else { // eq, gt, lt

        int label = labelCount++;

        output << "@SP" << endl;    // go to stack pointer
        output << "AM=M-1" << endl; // A=M -> last filled memory
        output << "D=M" << endl;    // store last variable (y)
        output << "A=A-1" << endl;  // A -> second-last variable (x)
        output << "D=M-D" << endl;  // D = x-y

        output << "@TRUE" << label << endl; // prepare jump

        if (command == "eq") output << "D;JEQ" << endl;
        else if (command == "gt") output << "D;JGT" << endl;
        else if (command == "lt") output << "D;JLT" << endl;

        // false:
        output << "@SP" << endl;                // point to @SP again
        output << "A=M-1" << endl;              // point to @SP-1
        output << "M=0" << endl;                // store 0 for false
        output << "@CONTINUE" << label << endl; // prep jump
        output << "0;JMP" << endl;              // jump to skip true

        // true:
        output << "(TRUE" << label << ')' << endl; // true label
        output << "@SP" << endl;                   // point to @SP again
        output << "A=M-1" << endl;                 // point to @SP-1
        output << "M=-1" << endl;                  // store -1 for true

        // all:
        output << "(CONTINUE" << label << ')' << endl; // continue label
    }
}

void CodeWriter::writePushPop(const CommandType& commandType, const string& segment, const int& index) const {
    if (commandType == C_PUSH) {
        // set D = value to be pushed
        if (segment == "constant") {
            output << '@' << index << endl;
            output << "D=A" << endl; // D = constant
        } else if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
            // select base address
            if (segment == "local") output << '@' << "LCL" << endl;
            else if (segment == "argument") output << '@' << "ARG" << endl;
            else if (segment == "this") output << '@' << "THIS" << endl;
            else if (segment == "that") output << '@' << "THAT" << endl;

            output << "D=M" << endl;        // D = base value
            output << '@' << index << endl; // A = constant
            output << "A=A+D" << endl;      // address of base + constant
            output << "D=M" << endl;        // D = RAM[base + constant]
        }

        else if (segment == "temp") {
            output << "@R5" << endl;
            output << "D=A" << endl;        // D = base address
            output << '@' << index << endl; // A = index
            output << "A=A+D" << endl;      // address of base + constant
            output << "D=M" << endl;        // D = RAM[base + constant]
        }

        else if (segment == "pointer") {
            output << (index == 0 ? "@THIS" : "@THAT") << endl;
            output << "D=M" << endl;
        }

        else if (segment == "static") {
            output << '@' << fileName << '.' << index << endl;
            output << "D=M" << endl;
        }

        // push D onto stack
        output << "@SP" << endl; // go to next stack location
        output << "A=M" << endl;
        output << "M=D" << endl; // store D
        output << "@SP" << endl; // SP++
        output << "M=M+1" << endl;
    }

    else if (commandType == C_POP) {
        if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
            // select base address
            if (segment == "local") output << '@' << "LCL" << endl;
            else if (segment == "argument") output << '@' << "ARG" << endl;
            else if (segment == "this") output << '@' << "THIS" << endl;
            else if (segment == "that") output << '@' << "THAT" << endl;

            output << "D=M" << endl;
            output << '@' << index << endl;
            output << "D=A+D" << endl; // D = next address = base + index
            output << "@R13" << endl;  // A -> R13 (programmer's use)
            output << "M=D" << endl;   // @R13 = base + index

            // pop stack into D
            output << "@SP" << endl;
            output << "AM=M-1" << endl;
            output << "D=M" << endl;

            // store D into RAM[address R13]
            output << "@R13" << endl;
            output << "A=M" << endl; // go to address in R13 = base + index
            output << "M=D" << endl; // store stack at next address
        }

        else if (segment == "temp") {
            output << "@R5" << endl;
            output << "D=A" << endl; // D = base
            output << '@' << index << endl;
            output << "D=A+D" << endl; // D = next address = base + index
            output << "@R13" << endl;  // A -> R13 (programmer's use)
            output << "M=D" << endl;   // @R13 = base + index

            // pop stack into D
            output << "@SP" << endl;
            output << "AM=M-1" << endl;
            output << "D=M" << endl;

            // store D into RAM[address R13]
            output << "@R13" << endl;
            output << "A=M" << endl; // go to address in R13 = base + index
            output << "M=D" << endl; // store stack at next address
        }

        else if (segment == "pointer") {
            output << "@SP" << endl;
            output << "AM=M-1" << endl;
            output << "D=M" << endl; // pop stack into D
            output << (index == 0 ? "@THIS" : "@THAT") << endl;
            output << "M=D" << endl;
        }

        else if (segment == "static") {
            output << "@SP" << endl;
            output << "AM=M-1" << endl;
            output << "D=M" << endl; // pop stack into D
            output << '@' << fileName << '.' << index << endl;
            output << "M=D" << endl;
        }
    }
}

void CodeWriter::close() const { output.close(); }

void CodeWriter::writeInit() {
    output << "@256" << endl;
    output << "D=A" << endl;
    output << "@SP" << endl;
    output << "M=D" << endl;
    writeCall("Sys.init", 0);
}

void CodeWriter::writeLabel(const string& label) { output << '(' << label << ')' << endl; }

void CodeWriter::writeGoto(const string& loc) {
    output << '@' << loc << endl;
    output << "0;JMP" << endl;
}

void CodeWriter::writeIf(const string& loc) {
    output << "@SP" << endl;
    output << "AM=M-1" << endl;
    output << "D=M" << endl;
    output << '@' << loc << endl;
    output << "D;JNE" << endl;
}

void CodeWriter::writeFunction(const string& func, int vars) {
    writeLabel(func);

    while (vars--) {
        writePushPop(C_PUSH, "constant", 0);
    }
}

void CodeWriter::writeCall(const string& func, int args) {
    int id = labelCount++;
    string ret = "RET" + to_string(id);

    // push ret
    output << '@' << ret << endl;
    output << "D=A" << endl; // save address
    output << "@SP" << endl; // back top of stack
    output << "A=M" << endl;
    output << "M=D" << endl; // store address
    output << "@SP" << endl; // increment SP
    output << "M=M+1" << endl;

    // push LCL, ARG, THIS, THAT
    string save[] = {"LCL", "ARG", "THIS", "THAT"};
    for (string s : save) {
        output << '@' << s << endl;
        output << "D=M" << endl; // save value
        output << "@SP" << endl; // back top of stack
        output << "A=M" << endl;
        output << "M=D" << endl; // store value
        output << "@SP" << endl; // increment SP
        output << "M=M+1" << endl;
    }

    // reposition ARG = SP - args - 5
    output << "@SP" << endl; // get SP
    output << "D=M" << endl;
    output << '@' << args + 5 << endl; // prep for - (args + 5)
    output << "D=D-A" << endl;         // D = SP - (args + 5)
    output << "@ARG" << endl;          // store into ARG
    output << "M=D" << endl;

    // LCL = SP
    output << "@SP" << endl;
    output << "D=M" << endl; // D = SP
    output << "@LCL" << endl;
    output << "M=D" << endl; // LCL = D

    writeGoto(func);
    writeLabel(ret);
}

void CodeWriter::writeReturn() {
    // R14 = LCL = frame
    output << "@LCL" << endl; // LCL = frame
    output << "D=M" << endl;
    output << "@R14" << endl; // R13 = LCL = frame
    output << "M=D" << endl;

    // R15 = ret (frame - 5)
    output << "@5" << endl;
    output << "A=D-A" << endl; // A = frame - 5 (= RET)
    output << "D=M" << endl;
    output << "@R15" << endl; // store R15 = A
    output << "M=D" << endl;

    // pop ARG
    writePushPop(C_POP, "argument", 0);

    // SP = ARG + 1
    output << "@ARG" << endl;
    output << "D=M+1" << endl;
    output << "@SP" << endl;
    output << "M=D" << endl;

    // restore THAT, THIS, ARG, LCL
    string restore[] = {"THAT", "THIS", "ARG", "LCL"};
    for (string s : restore) {
        output << "@R14" << endl;
        output << "AM=M-1" << endl; // previous index, decrement R14
        output << "D=M" << endl;    // store into @THAT
        output << '@' << s << endl;
        output << "M=D" << endl;
    }

    // goto ret
    output << "@R15" << endl;
    output << "A=M" << endl;
    output << "0;JMP" << endl;
}
