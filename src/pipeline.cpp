#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <cctype>
#include <fstream> 

#include "../include/assembly_to_binary.h"

using namespace std;
// --------- Global Variables ---------

vector<string> instruction_memory;

// ------------- Stage Registers --------------
int ALUSelect, ALUZeroFlag;

class REG {
public:
    int value = 0;
    int ins = -1;
    REG() : value(0), ins(-1) {}
};

vector<REG> reg(32);
vector<int> mem(1024, 1e9);

class pc_t {
public:
    int value = 0;
    bool valid = false;
    bool stall = true;
} PC;

class control_word {
public:
    int regRead = 0;
    int ALUSrc = 0;
    char ALUOp = ' ';
    int memWrite = 0;
    int memRead = 0;
    int regWrite = 0;
    int mem2Reg = 0;
    int BnJ = 0;
} CW;

class stage1_t {
public:
    string IR;
    int DPC = 0, NPC = 0;
    bool stall = false;
    bool valid = false;
} IFID;

class stage2_t {
public:
    string imm, func, rdl, rs1, rs2, immbr, immst;
    int JPC = 0, DPC = 0;
    control_word cw;
    bool stall = false;
    bool valid = false;
} IDEX;

class stage3_t {
public:
    string ALUOUT;
    control_word cw;
    string rs2, rdl, immst;
    bool stall = false;
    bool valid = false;
} EXMO;

class stage4_t {
public:
    string ALUOUT, LDOUT;
    control_word cw;
    string rdl, immst;
    bool stall = false;
    bool valid = false;
} MOWB;

// Control Unit
void control_unit(control_word &cw, const string &opcode) {
    cw.regRead = 1;
    if (opcode == "0110011") { // R
        cw.ALUSrc = 0; cw.mem2Reg = 0; cw.regWrite = 1; cw.memRead = 0; cw.memWrite = 0; cw.BnJ = 0; cw.ALUOp = 'R'; return;
    }
    if (opcode == "0010011") { // I
        cw.ALUSrc = 1; cw.mem2Reg = 0; cw.regWrite = 1; cw.memRead = 0; cw.memWrite = 0; cw.BnJ = 0; cw.ALUOp = 'I'; return;
    }
    if (opcode == "0000011") { // L
        cw.ALUSrc = 1; cw.mem2Reg = 1; cw.regWrite = 1; cw.memRead = 1; cw.memWrite = 0; cw.BnJ = 0; cw.ALUOp = 'L'; return;
    }
    if (opcode == "0100011") { // S
        cw.ALUSrc = 0; cw.mem2Reg = 0; cw.regWrite = 0; cw.memRead = 0; cw.memWrite = 1; cw.BnJ = 0; cw.ALUOp = 'S'; return;
    }
    if (opcode == "1100011") { // B
        cw.ALUSrc = 0; cw.mem2Reg = 0; cw.regWrite = 0; cw.memRead = 0; cw.memWrite = 0; cw.BnJ = 1; cw.ALUOp = 'B'; return;
    }
}

// ALU Control
string alu_control(char ALUOp, char func7bit, const string &func3) {
    string ALUctrl;
    if (ALUOp == 'L' || ALUOp == 'S')
        ALUctrl = "ADD";
    else if (ALUOp == 'B') {
        if (func3 == "000")       ALUctrl = "BEQ";
        else if (func3 == "001")  ALUctrl = "BNE";
        else if (func3 == "100")  ALUctrl = "BLT";
        else if (func3 == "101")  ALUctrl = "BGE";
        else if (func3 == "110")  ALUctrl = "BLTU";
        else if (func3 == "111")  ALUctrl = "BGEU";
    } else if (ALUOp == 'R') {
        if (func3 == "000") {
            if (func7bit == '0')      ALUctrl = "ADD";
            else                      ALUctrl = "SUB";
        } else if (func3 == "101") {
            if (func7bit == '0')      ALUctrl = "SRL";
            else                      ALUctrl = "SRA";
        } else if (func3 == "100")    ALUctrl = "XOR";
        else if (func3 == "110")      ALUctrl = "OR";
        else if (func3 == "111")      ALUctrl = "AND";
        else if (func3 == "001")      ALUctrl = "SLL";
    } else if (ALUOp == 'I') {
        if (func3 == "000")           ALUctrl = "ADD";
        else if (func3 == "101") {
            if (func7bit == '0')      ALUctrl = "SRL";
            else                      ALUctrl = "SRA";
        } else if (func3 == "100")    ALUctrl = "XOR";
        else if (func3 == "110")      ALUctrl = "OR";
        else if (func3 == "111")      ALUctrl = "AND";
        else if (func3 == "001")      ALUctrl = "SLL";
    }
    return ALUctrl;
}

// ALU simulation
string alu(int val1, int val2, const string &ALUctrl, bool &branch_flag) {
    if (ALUctrl == "BEQ") {
        if (val1 == val2) branch_flag = true;
        return "";
    }
    if (ALUctrl == "BNE") {
        if (val1 != val2) branch_flag = true;
        return "";
    }
    if (ALUctrl == "BLT") {
        if (val1 < val2) branch_flag = true;
        return "";
    }
    if (ALUctrl == "BGE") {
        if (val1 >= val2) branch_flag = true;
        return "";
    }
    if (ALUctrl == "ADD")    return decimal_to_binary(to_string(val1+val2), 32, true);
    if (ALUctrl == "SUB")    return decimal_to_binary(to_string(val1-val2), 32, true);
    if (ALUctrl == "OR")     return decimal_to_binary(to_string(val1|val2), 32, true);
    if (ALUctrl == "XOR")    return decimal_to_binary(to_string(val1^val2), 32, true);
    if (ALUctrl == "AND")    return decimal_to_binary(to_string(val1&val2), 32, true);
    if (ALUctrl == "SLL")    return decimal_to_binary(to_string(val1<<val2), 32, true);
    if (ALUctrl == "SRL")    return decimal_to_binary(to_string((uint32_t)val1>>val2), 32, true);
    if (ALUctrl == "SRA")    return decimal_to_binary(to_string(val1>>val2), 32, true);
    return "";
}

// STAGE 1: Fetch
void instruction_fetch() {
    if (IFID.stall) return;
    if (PC.valid) {
        if (PC.value < 0 || PC.value >= (int)instruction_memory.size()) {
            IFID.valid = false;
            PC.valid = false;
            return;
        }
        IFID.IR = instruction_memory[PC.value];
        IFID.DPC = PC.value;
        IFID.NPC = PC.value + 1;
        IFID.valid = true;
        PC.value += 1;
    }
    return;
}

// STAGE 2: Decode
void instruction_decode() {
    if (IFID.valid) {
        IDEX.DPC = IFID.DPC;
        IDEX.imm = IFID.IR.substr(0, 12);
        IDEX.func = IFID.IR.substr(17, 3);
        IDEX.rdl = IFID.IR.substr(20, 5);
        IDEX.immst = IFID.IR.substr(0, 7) + IFID.IR.substr(20, 5);
        string s = IFID.IR.substr(0, 1) + IFID.IR.substr(20, 1) + IFID.IR.substr(1, 6) + IFID.IR.substr(21, 4);
        IDEX.immbr = s;
        string opcode = IFID.IR.substr(25, 7);
        control_unit(IDEX.cw, opcode);
        if (IDEX.cw.regRead) {
            string rs1 = IFID.IR.substr(12, 5);
            int irs1 = binary_to_decimal(rs1, false);
            if (reg[irs1].ins == -1)
                IDEX.rs1 = IFID.IR.substr(12, 5);
            else {
                IFID.stall = true;
                IDEX.valid = false;
                return;
            }
        }
        string rs2 = IFID.IR.substr(7, 5);
        int irs2 = binary_to_decimal(rs2, false);
        if ((!IDEX.cw.ALUSrc || opcode == "0100011") && reg[irs2].ins != -1) {
            IFID.stall = true;
            IDEX.valid = false;
            return;
        }
        IDEX.rs2 = IFID.IR.substr(7, 5);
        int rdl = binary_to_decimal(IDEX.rdl, false);
        if (IDEX.cw.regWrite) reg[rdl].ins = IDEX.DPC;
        IDEX.valid = true;
        IFID.stall = false;
    } else {
        IDEX.valid = false;
    }
    return;
}

// STAGE 3: Execute
void instruction_execution() {
    if (IDEX.valid) {
        string aluSelect = alu_control(IDEX.cw.ALUOp, IFID.IR[1], IDEX.func);
        string rs1 = IDEX.rs1;
        int irs1 = binary_to_decimal(rs1, false);
        int val1 = reg[irs1].value;
        int val2;
        if (IDEX.cw.ALUSrc == 0) {
            if (IDEX.cw.ALUOp == 'S') {
                int imm = binary_to_decimal(IDEX.immst, true);
                val2 = imm;
            } else {
                string rs2 = IDEX.rs2;
                int irs2 = binary_to_decimal(rs2, false);
                val2 = reg[irs2].value;
            }
        } else {
            int imm = binary_to_decimal(IDEX.imm, true);
            val2 = imm;
        }
        bool branch_flag = false;
        EXMO.ALUOUT = alu(val1, val2, aluSelect, branch_flag);

        if (IDEX.cw.BnJ == 1 && branch_flag) {
            PC.value = IDEX.DPC;
            int imm = binary_to_decimal(IDEX.immbr, true);
            PC.value += imm;
            PC.valid = true;
        }
        EXMO.immst = IDEX.immst;
        EXMO.cw = IDEX.cw;
        EXMO.rdl = IDEX.rdl;
        EXMO.rs2 = IDEX.rs2;
        EXMO.valid = true;
    } else
        EXMO.valid = false;
    return;
}

// STAGE 4: Memory Access
void memory_operation() {
    if (EXMO.valid) {
        if (EXMO.cw.memRead || EXMO.cw.memWrite) {
            int ea = binary_to_decimal(EXMO.ALUOUT, true);
            if (ea < 0 || ea >= 1023) {
                cout << "EA out of bounds" << endl;
                return;
            }
            if (EXMO.cw.memWrite) {
                int irs2 = binary_to_decimal(EXMO.rs2, false);
                mem[ea] = reg[irs2].value;
            }
            if (EXMO.cw.memRead) {
                string smem = to_string(mem[ea]);
                MOWB.LDOUT = decimal_to_binary(smem, 32, true);
            }
        }
        MOWB.immst = EXMO.immst;
        MOWB.cw = EXMO.cw;
        MOWB.ALUOUT = EXMO.ALUOUT;
        MOWB.rdl = EXMO.rdl;
        if (EXMO.cw.regWrite)
            MOWB.valid = true;
        else
            MOWB.valid = false;
    } else
        MOWB.valid = false;
}

// STAGE 5: Writeback
void write_back() {
    if (MOWB.valid) {
        string write_data;
        if (MOWB.cw.mem2Reg)
            write_data = MOWB.LDOUT;
        else
            write_data = MOWB.ALUOUT;
        if (MOWB.cw.regWrite) {
            int rdl = binary_to_decimal(MOWB.rdl, false);
            reg[rdl].value = binary_to_decimal(write_data, true);
            reg[rdl].ins = -1;
        }
    }
}
// Print register and memory content with cycle info
void printer(int cycle) {
    cout << "Register/Memory Contents after Cycle " << cycle << ":" << endl;
    for (int i = 0; i < 32; i++) {
        if (reg[i].value != 0)
            cout << "Reg" << i << " = " << reg[i].value << "; ";
    }
    cout << endl;
    for (int i = 0; i < 1023; i++) {
        if (mem[i] != 1e9)
            cout << "Mem" << i << " = " << mem[i] << "; ";
    }
    cout << endl << endl;
}

// ----- MAIN -----
int main() {
    ifstream fin("../../test_instructions.txt"); // from build/Debug
    if (!fin) {
        cout << "Failed to open test_instructions.txt! Check your path." << endl;
        return 1;
    }
    vector<string> all_lines;
    string line;
    while (getline(fin, line)){
        cout << line << endl;
        if (!line.empty() && line[0] != '#') all_lines.push_back(line);
    }
        

    int n = all_lines.size();
    PC.value = 0;
    for (int i=0; i<n; ++i) {
        string s = all_lines[i];
        auto parsed = parse_instruction(s);
        char type = instruction_type(parsed[0]);
        string bincode;

        if (type == 'r')
            bincode = r_type(parsed);
        else if (type == 'i')
            bincode = i_type(parsed);
        else if (type == 'l')
            bincode = load_type(parsed);
        else if (type == 's')
            bincode = store_type(parsed);
        else if (type == 'b')
            bincode = b_type(parsed);
        else if (type == 'u')
            bincode = u_type(parsed);
        else if (type == 'j')
            bincode = j_type(parsed);

        instruction_memory.push_back(bincode);
    }


    for (int i = 0; i < (int)instruction_memory.size(); i++)
        cout << "Binary Code for Instruction " << i + 1 << " is: " << instruction_memory[i] << endl;

    // PIPELINE
    bool validcheck = true;
    PC.valid = true;
    int cycle = 1;

    while (validcheck) {
        cout << "Cycle: " << cycle << endl;
        write_back();
        memory_operation();
        instruction_execution();
        instruction_decode();
        instruction_fetch();
        printer(cycle); // pass cycle number to printer
        cycle++;
        validcheck = PC.valid | IFID.valid | IDEX.valid | EXMO.valid | MOWB.valid;
    }
    return 0;
}
