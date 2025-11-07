#include "CodeWriter.h"

using namespace std;

CodeWriter::CodeWriter(ofstream &file) : output(file) { labelCount = 0; }

void CodeWriter::setFileName(const string &name) {
    size_t dot = name.find_last_of('.');
    fileName = (dot != string::npos ? name.substr(0, dot) : name);
    size_t slash = fileName.find_last_of("/\\");
    fileName = (slash != string::npos ? fileName.substr(slash + 1) : fileName);
}

void CodeWriter::writeArithmetic(const string &command) {
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

void CodeWriter::writePushPop(const CommandType &commandType, const string &segment, const int &index) const {
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
