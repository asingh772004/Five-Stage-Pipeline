#include <bits/stdc++.h>
#include "assembly_to_binary.h"
using namespace std;


// --- Helper Functions ---

string hexa_to_binary(const string &s) {
    static unordered_map<char, string> hex_table = {
        {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"},
        {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
        {'8', "1000"}, {'9', "1001"},
        {'A', "1010"}, {'B', "1011"}, {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"},
        {'a', "1010"}, {'b', "1011"}, {'c', "1100"}, {'d', "1101"}, {'e', "1110"}, {'f', "1111"}
    };
    string binary_number;
    int i = 2; // skip possible "0x"
    while (i < s.size() && s[i] == '0') i++; // skip leading zeroes
    for (; i < s.size(); i++) {
        if (hex_table.find(s[i]) != hex_table.end())
            binary_number += hex_table[s[i]];
    }
    return binary_number;
}

bool is_number(const string &s) {
    if (s.empty()) return false;
    int start = (s[0] == '-') ? 1 : 0;
    for (int i=start; i<s.size(); i++) if (!isdigit(s[i])) return false;
    return true;
}

string decimal_to_binary(const string &s, int len, bool imm) {
    int number = stoi(s);
    string binary(len, '0');
    if (!imm && number < 0) number = (1 << len) + number; // two's complement
    else if (imm && number < 0) number = (1 << len) + number;
    for (int i = len - 1; i >= 0; i--) {
        binary[i] = (number & 1) ? '1' : '0';
        number >>= 1;
    }
    return binary;
}

int binary_to_decimal(const string &s, bool imm) {
    int num = 0;
    int n = s.length();
    if ((imm || n==32) && s[0]=='1') { // negative two's complement
        string inverted;
        for(char c : s) inverted += (c=='1'?'0':'1');
        num = -(stoi(inverted, nullptr, 2) + 1);
    } else num = stoi(s, nullptr, 2);
    return num;
}

vector<string> parse_instruction(const string &s) {
    vector<string> tokens;
    string src = s;
    replace(src.begin(), src.end(), ',', ' ');
    istringstream iss(src);
    string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}

// --- Helper Maps ---

unordered_map<string, string> opcode_map = {
    {"add",   "0110011"}, {"sub",   "0110011"}, {"sll",   "0110011"},
    {"slt",   "0110011"}, {"sltu",  "0110011"}, {"xor",   "0110011"},
    {"srl",   "0110011"}, {"sra",   "0110011"}, {"or",    "0110011"},
    {"and",   "0110011"}, {"addi",  "0010011"}, {"ori",   "0010011"},
    {"xori",  "0010011"}, {"jalr",  "1100111"}, {"lw",    "0000011"},
    {"sw",    "0100011"}, {"sb",    "0100011"}, {"beq",   "1100011"},
    {"bne",   "1100011"}, {"blt",   "1100011"}, {"bge",   "1100011"},
    {"bltu",  "1100011"}, {"bgeu",  "1100011"}, {"lui",   "0110111"},
    {"auipc", "0010111"}, {"jal",   "1101111"}
};

unordered_map<string, string> funct3_map = {
    {"add", "000"}, {"sub", "000"}, {"sll", "001"}, {"slt", "010"},
    {"sltu", "011"}, {"xor", "100"}, {"srl", "101"}, {"sra", "101"},
    {"or", "110"}, {"and", "111"}, {"addi", "000"}, {"ori", "110"},
    {"xori", "100"}, {"jalr", "000"}, {"lw", "010"}, {"sw", "010"},
    {"sb", "000"}, {"beq", "000"}, {"bne", "001"}, {"blt", "100"},
    {"bge", "101"}, {"bltu", "110"}, {"bgeu", "111"}
};

unordered_map<string, string> funct7_map = {
    {"add", "0000000"}, {"sub", "0100000"}, {"sll", "0000000"}, {"slt", "0000000"},
    {"sltu", "0000000"}, {"xor", "0000000"}, {"srl", "0000000"}, {"sra", "0100000"},
    {"or", "0000000"}, {"and", "0000000"}
};

unordered_map<string, string> rtype_inst = {
    {"add", "add"}, {"sub", "sub"}, {"sll", "sll"}, {"slt", "slt"},
    {"sltu", "sltu"}, {"xor", "xor"}, {"srl", "srl"}, {"sra", "sra"},
    {"or", "or"}, {"and", "and"}
};

unordered_map<string, string> itype_inst = {
    {"addi", "addi"}, {"ori", "ori"}, {"xori", "xori"}, {"jalr", "jalr"}
};

unordered_map<string, string> ltype_inst = { {"lw", "lw"} };
unordered_map<string, string> stype_inst = { {"sw", "sw"}, {"sb", "sb"} };
unordered_map<string, string> btype_inst = {
    {"beq", "beq"}, {"bne", "bne"}, {"blt", "blt"}, {"bge", "bge"},
    {"bltu", "bltu"}, {"bgeu", "bgeu"}
};
unordered_map<string, string> utype_inst = { {"lui", "lui"}, {"auipc", "auipc"} };
unordered_map<string, string> jtype_inst = { {"jal", "jal"} };

int regnumber(const string& s) {
    // s assumed of form xN
    if (s.size() > 1 && s[0] == 'x')
        return stoi(s.substr(1));
    else
        return stoi(s);
}

string get_reg_bin(const string& s) {
    return decimal_to_binary(to_string(regnumber(s)), 5, true);
}

// --- Type-Specific Encodings ---

string r_type(const vector<string> &v) {
    // add x1, x2, x3        => opcode(7) | rd(5) | funct3(3) | rs1(5) | rs2(5) | funct7(7)
    string instr = v[0];
    string rd = get_reg_bin(v[1]); // destination
    string rs1 = get_reg_bin(v[2]);
    string rs2 = get_reg_bin(v[3]);

    string opcode = opcode_map[instr]; // e.g. "0110011"
    string funct3 = funct3_map[instr]; // e.g. "000"
    string funct7 = funct7_map[instr]; // e.g. "0000000" or "0100000"

    string result = funct7 + rs2 + rs1 + funct3 + rd + opcode;
    return result;
}

string i_type(const vector<string> &v) {
    // addi x1, x2, 10   => opcode(7) | rd(5) | funct3(3) | rs1(5) | imm(12)
    string instr = v[0];
    string rd = get_reg_bin(v[1]);
    string rs1 = get_reg_bin(v[2]);
    string imm = decimal_to_binary(v[3], 12, true);
    string opcode = opcode_map[instr];
    string funct3 = funct3_map[instr];

    string result;
    if (instr == "jalr") {
        // jalr x1, x2, 8
        result = imm + rs1 + funct3 + rd + opcode;
    } else {
        result = imm + rs1 + funct3 + rd + opcode;
    }
    return result;
}

string load_type(const vector<string> &v) {
    // lw x1, offset(x2)
    // v: lw x1, offset(x2)
    string instr = v[0];
    string rd = get_reg_bin(v[1]);
    string offsetstr, rs1str;
    size_t open = v[2].find('('), close = v[2].find(')');
    if (open != string::npos && close != string::npos) {
        offsetstr = v[2].substr(0, open);
        rs1str = v[2].substr(open+1, close-open-1);
    } else {
        // fallback, in case not written traditionally
        offsetstr = v[2];
        rs1str = v[3];
    }
    string immediate = decimal_to_binary(offsetstr, 12, true);
    string rs1 = get_reg_bin(rs1str);
    string opcode = opcode_map[instr];
    string funct3 = funct3_map[instr];
    string result = immediate + rs1 + funct3 + rd + opcode;
    return result;
}

string store_type(const vector<string> &v) {
    // sw x1, offset(x2)
    // sw rs2, offset(rs1)
    string instr = v[0];
    string rs2 = get_reg_bin(v[1]);
    string offsetstr, rs1str;
    size_t open = v[2].find('('), close = v[2].find(')');
    if (open != string::npos && close != string::npos) {
        offsetstr = v[2].substr(0, open);
        rs1str = v[2].substr(open+1, close-open-1);
    } else {
        offsetstr = v[2];
        rs1str = v[3];
    }
    string immediate = decimal_to_binary(offsetstr, 12, true);
    string immhi = immediate.substr(0, 7);
    string immlo = immediate.substr(7, 5);
    string rs1 = get_reg_bin(rs1str);
    string opcode = opcode_map[instr];
    string funct3 = funct3_map[instr];
    string result = immhi + rs2 + rs1 + funct3 + immlo + opcode;
    return result;
}

string b_type(const vector<string> &v) {
    // beq x1, x2, offset
    string instr = v[0];
    string rs1 = get_reg_bin(v[1]);
    string rs2 = get_reg_bin(v[2]);
    string offset = decimal_to_binary(v[3], 13, true); // Branch uses imm[12|10:5|4:1|11]
    // Rearrangement per RV spec
    string imm = "";
    imm += offset[0];                // imm[12] - bit 0
    imm += offset.substr(2,6);       // imm[10:5] - bit 1..6 (6 bits)
    imm += rs2;                      // rs2
    imm += rs1;                      // rs1
    string funct3 = funct3_map[instr];
    imm += funct3;
    imm += offset.substr(8,4);       // imm[4:1] - bits 7..10 (4 bits)
    imm += offset[1];                // imm[11] - bit 11
    string opcode = opcode_map[instr];
    imm += opcode;
    return imm;
}

string u_type(const vector<string> &v) {
    // lui x1, imm20
    string instr = v[0];
    string rd = get_reg_bin(v[1]);
    string immediate = decimal_to_binary(v[2], 20, true);
    string opcode = opcode_map[instr];
    string result = immediate + rd + opcode;
    return result;
}

string j_type(const vector<string> &v) {
    // jal x1, offset
    string instr = v[0];
    string rd = get_reg_bin(v[1]);
    string offset = decimal_to_binary(v[2], 21, true);
    //JAL has immediate in a shuffled order: [20][10:1][11][19:12]
    string result;
    result  = offset[0];        // 20
    result += offset.substr(10,10); // [10:1]
    result += offset[9];            // [11]
    result += offset.substr(1,8);   // [19:12]
    result += rd;
    result += opcode_map[instr];
    return result;
}

char instruction_type(const string &s) {
    string lower = s;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (rtype_inst.find(lower) != rtype_inst.end()) return 'r';
    if (itype_inst.find(lower) != itype_inst.end()) return 'i';
    if (ltype_inst.find(lower) != ltype_inst.end()) return 'l';
    if (stype_inst.find(lower) != stype_inst.end()) return 's';
    if (btype_inst.find(lower) != btype_inst.end()) return 'b';
    if (utype_inst.find(lower) != utype_inst.end()) return 'u';
    if (jtype_inst.find(lower) != jtype_inst.end()) return 'j';
    return ' ';
}
