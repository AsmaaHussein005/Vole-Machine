#include "voleMachine.h"

int CPU::hexdecToDecimal_(const string& hex) {
    int DecVal_ = 0;
    int Base_ = 1;
    for (int v = hex.size() - 1; v >= 0; --v) {
        char HexaDig_ = hex[v];
        if (isdigit(HexaDig_))
            DecVal_ += (HexaDig_ - '0') * Base_;
        else if (HexaDig_ >= 'A' && HexaDig_ <= 'F')
            DecVal_ += (HexaDig_ - 'A' + 10) * Base_;
        Base_ *= 16;
    }
    return DecVal_;
}

string CPU::decimalToHexdec_(int decimal) {
    if (decimal == 0) return "00";
    string HexaString_;
    const char hexaDigs_[] = "0123456789ABCDEF";
    while (decimal > 0) {
        HexaString_ = hexaDigs_[decimal % 16] + HexaString_;
        decimal /= 16;
    }
    if (HexaString_.length() == 1) HexaString_ = "0" + HexaString_;
    return HexaString_;
}

string CPU::sec_complement_(string value1, string value2) {
    int decimal1_ = hexdecToDecimal_(value1);
    int decimal2_ = hexdecToDecimal_(value2);

    if (decimal1_ >= 128) {
        decimal1_ -= 256;
    }
    if (decimal2_ >= 128) {
        decimal2_ -= 256;
    }
    int Sumation_ = decimal1_ + decimal2_;
    if (Sumation_ > 127) {
        Sumation_ -= 256;
    }
    else if (Sumation_ < -128) {
        Sumation_ += 256;
    }
    return decimalToHexdec_(Sumation_);
}

string CPU::AddFloatPnt_(const string& hex1, const string& hex2) {
    int Bias_ = 4;
    auto HexToBinary_ = [](const string& hexStr) {
        return bitset<8>(stoi(hexStr, nullptr, 16)).to_string();
        };
    auto binToFloat = [Bias_](const string& binaryStr) {
        int SignBit_ = binaryStr[0] - '0', Exp_ = stoi(binaryStr.substr(1, 3), nullptr, 2) - Bias_;
        float Mantis_ = 0.0;
        for (int i = 0; i < 4; ++i)
            if (binaryStr[4 + i] == '1') Mantis_ += pow(2, -(i + 1));
        return pow(-1, SignBit_) * (1 + Mantis_) * pow(2, Exp_);
        };
    auto FoatToBinary_ = [Bias_](float value) {
        string Res_(8, '0');
        Res_[0] = (value < 0) ? '1' : '0'; value = fabs(value);
        int Exp_ = 0;
        while (value >= 2) { value /= 2; Exp_++; }
        while (value < 1) { value *= 2; Exp_--; }
        Res_.replace(1, 3, bitset<3>(Exp_ + Bias_).to_string());
        float Mantis_ = value - 1;
        for (int i = 0; i < 4; ++i) {
            if (Mantis_ >= pow(2, -(i + 1))) {
                Res_[4 + i] = '1';
                Mantis_ -= pow(2, -(i + 1));
            }
        }
        return Res_;
        };
    auto BinToHex_ = [](const string& binaryStr) {
        int HexaRes_ = bitset<8>(binaryStr).to_ulong();
        stringstream ss;
        ss << uppercase << hex << setw(2) << setfill('0') << HexaRes_;
        return ss.str();
        };
    float ResFloat_ = binToFloat(HexToBinary_(hex1)) + binToFloat(HexToBinary_(hex2));
    return BinToHex_(FoatToBinary_(ResFloat_));
}

string CPU::ORingBitwise_(string v1, string v2) {
    int num1 = hexdecToDecimal_(v2);
    int num2 = hexdecToDecimal_(v1);
    return decimalToHexdec_(num1 | num2);
}

string CPU::ANDingBitwise_(string value1, string value2) {
    int num1 = hexdecToDecimal_(value1);
    int num2 = hexdecToDecimal_(value2);
    return decimalToHexdec_(num1 & num2);
}

string CPU::XORintBitwise_(string value1, string value2) {
    int num1 = hexdecToDecimal_(value1);
    int num2 = hexdecToDecimal_(value2);
    return decimalToHexdec_(num1 ^ num2);
}

string CPU::Rotate_(string str, int x) {
    int decimal_ = hexdecToDecimal_(str);
    bitset<8> bin(decimal_);
    string bin1 = bin.to_string();
    int size = bin.size();
    x = x % size;

    string BinRes_ = bin1.substr(size - x) + bin1.substr(0, size - x);
    int decValue = stoi(BinRes_, nullptr, 2);
    return decimalToHexdec_(decValue);
}

Memory::Memory() {
    Instruct_.resize(129, { {"00", "00"}, {"00", "00"} });
    int Counter_ = 0;

    for (int i = 0; i < 129; ++i) {
        Instruct_[i].first.first = CPU::decimalToHexdec_(Counter_);
        Instruct_[i].second.second = CPU::decimalToHexdec_(Counter_ + 1);
        Instruct_[i].first.second = "00";
        Instruct_[i].second.first = "00";
        Counter_ += 2;
    }
    Register_.resize(16, "00");
}

string Memory::Fetch_(const string& address) {
    for (const auto& instr_ : Instruct_) {
        if (instr_.first.first == address) {
            return instr_.first.second;
        }
        else if (instr_.second.second == address) {
            return instr_.second.first;
        }
    }
    return "00";
}

int Memory::Jump_(const string& address) {
    int Counter_ = 0;
    for (const auto& instr : Instruct_) {
        if (instr.first.first == address || instr.second.second == address) {
            return Counter_;
        }
        Counter_++;
    }
    return 0;
}

void Memory::SetCell_(int index, string address) {
    for (auto& Instr_ : Instruct_) {
        if (Instr_.first.first == address) {
            Instr_.first.second = Register_[index];
        }
        else if (Instr_.second.second == address) {
            Instr_.second.first = Register_[index];
        }
    }
}

void Machine::loadingProgramFile_() {
    string Address_ = "10";

    fstream File_;
    string FileName_;
    cout << "Enter the file name: ";
    getline(cin, FileName_);
    File_.open(FileName_, ios::in);
    if (!File_.is_open()) {
        cerr << "Error Can't Open The File" << endl;
        return;
    }
    else if (File_.is_open()) {
        cout << "Start default point address 10:\n";

        string Input_;
        getline(cin, Input_);
        if (!Input_.empty()) {
            istringstream stream(Input_);
            stream >> Address_;
        }
        int LineCount_ = hexdecToDecimal_(Address_) / 2;
        string Line_;
        while (getline(File_, Line_)) {
            stringstream stream(Line_);
            string Instr_;
            while (stream >> Instr_) {
                if (Instr_.length() < 4)
                    Instr_ = Instr_.substr(0, 2) + "0" + Instr_.substr(2);
                memo_.Instruct_[LineCount_].first.second = Instr_.substr(0, 2);
                memo_.Instruct_[LineCount_].second.first = Instr_.substr(2, 2);
            }
            LineCount_++;
        }
        File_.close();
    }
}

void Machine::executingProgram_() {
    for (int v = 0; v < 129; v++) {
        string OpCode_ = memo_.Instruct_[v].first.second.substr(0, 1);
        int RegIndex_ = CPU::hexdecToDecimal_(memo_.Instruct_[v].first.second.substr(1, 1));
        string OperandXY_ = memo_.Instruct_[v].second.first;
        if (OpCode_ == "0")
            continue;
        else if (OpCode_ == "1")
            memo_.Register_[RegIndex_] = memo_.Fetch_(OperandXY_);
        else if (OpCode_ == "2")
            memo_.SetCell_(RegIndex_, OperandXY_);
        else if (OpCode_ == "3")
            memo_.Register_[RegIndex_] = AddFloatPnt_(memo_.Register_[RegIndex_], memo_.Fetch_(OperandXY_));
        else if (OpCode_ == "4")
            memo_.Register_[RegIndex_] = sec_complement_(memo_.Register_[RegIndex_], memo_.Fetch_(OperandXY_));
        else if (OpCode_ == "5")
            memo_.Register_[RegIndex_] = ORingBitwise_(memo_.Register_[RegIndex_], memo_.Fetch_(OperandXY_));
        else if (OpCode_ == "6")
            memo_.Register_[RegIndex_] = ANDingBitwise_(memo_.Register_[RegIndex_], memo_.Fetch_(OperandXY_));
        else if (OpCode_ == "7")
            memo_.Register_[RegIndex_] = XORintBitwise_(memo_.Register_[RegIndex_], memo_.Fetch_(OperandXY_));
        else if (OpCode_ == "8") {
            stringstream ss(OperandXY_);
            int n;
            ss >> hex >> n;
            memo_.Register_[RegIndex_] = Rotate_(memo_.Register_[RegIndex_], n);
        }
        else if (OpCode_ == "9")
            v = memo_.Jump_(OperandXY_) - 1;
        else if (OpCode_ == "A" && memo_.Register_[RegIndex_] == "00")
            v = memo_.Jump_(OperandXY_) - 1;
    }
}

void Machine::PrintState_() {
    cout << "Registers:\n";
    for (size_t i = 0; i < memo_.Register_.size(); ++i) {
        cout << "R" << i << ": " << memo_.Register_[i] << endl;
    }
}

