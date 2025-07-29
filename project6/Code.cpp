#include "Code.h"
#include <unordered_map>

using namespace std;

string Code::dest(const string &mnemonic) {
    char d1 = '0', d2 = '0', d3 = '0';
    if (mnemonic.find('M') != string::npos)
        d3 = '1';
    if (mnemonic.find('D') != string::npos)
        d2 = '1';
    if (mnemonic.find('A') != string::npos)
        d1 = '1';
    return string() + d1 + d2 + d3;
}

string Code::comp(const string &mnemonic) {
    unordered_map<string, string> compMap = {
        {"0", "0101010"},   {"1", "0111111"},   {"-1", "0111010"},
        {"D", "0001100"},   {"A", "0110000"},   {"M", "1110000"},
        {"!D", "0001101"},  {"!A", "0110001"},  {"!M", "1110001"},
        {"-D", "0001111"},  {"-A", "0110011"},  {"-M", "1110011"},
        {"D+1", "0011111"}, {"A+1", "0110111"}, {"M+1", "1110111"},
        {"D-1", "0001110"}, {"A-1", "0110010"}, {"M-1", "1110010"},
        {"D+A", "0000010"}, {"D+M", "1000010"}, {"D-A", "0010011"},
        {"D-M", "1010011"}, {"A-D", "0000111"}, {"M-D", "1000111"},
        {"D&A", "0000000"}, {"D&M", "1000000"}, {"D|A", "0010101"},
        {"D|M", "1010101"}};
    return compMap.at(mnemonic);
}

string Code::jump(const string &mnemonic) {
    unordered_map<string, string> jumpMap = {
        {"null", "000"}, {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"},
        {"JLT", "100"},  {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}};
    return jumpMap.at(mnemonic);
}
