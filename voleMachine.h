#ifndef MACHINE_H
#define MACHINE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <cmath>

using namespace std;

class CPU {
public:
    static int hexdecToDecimal_(const string& hex);
    static string decimalToHexdec_(int decimal);
    string sec_complement_(string value1, string value2);
    string AddFloatPnt_(const string& hex1, const string& hex2);
    string ORingBitwise_(string v1, string v2);
    string ANDingBitwise_(string value1, string value2);
    string XORintBitwise_(string value1, string value2);
    string Rotate_(string str, int x);
};

class Memory {
public:
    vector<string> Register_;
    vector<pair<pair<string, string>, pair<string, string>>> Instruct_;

    Memory();
    string Fetch_(const string& address);
    int Jump_(const string& address);
    void SetCell_(int index, string address);
};

class Machine : public CPU {
public:
    Memory memo_;

    void loadingProgramFile_();
    void executingProgram_();
    void PrintState_();
};

#endif // MACHINE_H

