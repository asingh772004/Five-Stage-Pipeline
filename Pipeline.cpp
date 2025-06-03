#include <bits/stdc++.h>
#include <unistd.h>
#include "assemblyToBinary.h"
using namespace std;
#define int long long

vector<string> instructionMemory;

///////////////////////////////////////////////////////////////////////////////////////////////////////////

int ALUSelect, ALUZeroFlag;

class REG
{
public:
    int value = 0;
    int ins = -1;
    REG()
    {
        value = 0;
        ins = -1;
    }
};

vector<REG> reg(32);
vector<int> mem(1024, 1e9);

class pc
{
public:
    int value = 0;
    bool valid = false;
    bool stall = true;
} PC;

class controlWord
{
public:
    int regRead = 0;
    int ALUSrc = 0;
    char ALUOp;
    int memWrite = 0;
    int memRead = 0;
    int regWrite = 0;
    int mem2Reg = 0;
    int BnJ = 0;
} CW;

class stage1
{
public:
    string IR;
    int DPC, NPC;
    bool stall = false;
    bool valid = false;
} IFID;

class stage2
{
public:
    string imm, func, rdl, rs1, rs2, immbr, immst;
    int JPC, DPC;
    controlWord cw;
    bool stall = false;
    bool valid = false;
} IDEX;

class stage3
{
public:
    string ALUOUT;
    controlWord cw;
    string rs2;
    string rdl, immst;
    bool stall = false;
    bool valid = false;
} EXMO;

class stage4
{
public:
    string ALUOUT, LDOUT;
    controlWord cw;
    string rdl, immst;
    bool stall = false;
    bool valid = false;
} MOWB;

void controlUnit(controlWord &cw, string opcode)
{
    cw.regRead = 1;
    if (opcode == "0110011")
    {
        cw.ALUSrc = 0;
        cw.mem2Reg = 0;
        cw.regWrite = 1;
        cw.memRead = 0;
        cw.memWrite = 0;
        cw.BnJ = 0;
        cw.ALUOp = 'R';
        return;
    }
    if (opcode == "0010011")
    {
        cw.ALUSrc = 1;
        cw.mem2Reg = 0;
        cw.regWrite = 1;
        cw.memRead = 0;
        cw.memWrite = 0;
        cw.BnJ = 0;
        cw.ALUOp = 'I';
        return;
    }
    if (opcode == "0000011")
    {
        cw.ALUSrc = 1;
        cw.mem2Reg = 1;
        cw.regWrite = 1;
        cw.memRead = 1;
        cw.memWrite = 0;
        cw.BnJ = 0;
        cw.ALUOp = 'L';
        return;
    }
    if (opcode == "0100011")
    {
        cw.ALUSrc = 0;
        cw.mem2Reg = 0;
        cw.regWrite = 0;
        cw.memRead = 0;
        cw.memWrite = 1;
        cw.BnJ = 0;
        cw.ALUOp = 'S';
        return;
    }
    if (opcode == "1100011")
    {
        cw.ALUSrc = 0;
        cw.mem2Reg = 0;
        cw.regWrite = 0;
        cw.memRead = 0;
        cw.memWrite = 0;
        cw.BnJ = 1;
        cw.ALUOp = 'B';
        return;
    }
}

string ALUControl(char ALUOp, char func7bit, string func3)
{
    string ALUctrl;
    if (ALUOp == 'L' || ALUOp == 'S')
        ALUctrl = "ADD";
    else if (ALUOp == 'B')
    {
        if (func3 == "000")
            ALUctrl = "BEQ";
        else if (func3 == "001")
            ALUctrl = "BNE";
        else if (func3 == "100")
            ALUctrl = "BLT";
        else if (func3 == "101")
            ALUctrl = "BGE";
        else if (func3 == "110")
            ALUctrl = "BLTU";
        else if (func3 == "111")
            ALUctrl = "BGEU";
    }
    else if (ALUOp == 'R')
    {
        if (func3 == "000")
        {
            if (func7bit == '0')
                ALUctrl = "ADD";
            else
                ALUctrl = "SUB";
        }
        else if (func3 == "101")
        {
            if (func7bit == '0')
                ALUctrl = "SRL";
            else
                ALUctrl = "SRA";
        }
        else if (func3 == "100")
            ALUctrl = "XOR";
        else if (func3 == "110")
            ALUctrl = "OR";
        else if (func3 == "111")
            ALUctrl = "AND";
        else if (func3 == "001")
            ALUctrl = "SLL";
    }
    else if (ALUOp == 'I')
    {
        if (func3 == "000")
            ALUctrl = "ADD";
        else if (func3 == "101")
        {
            if (func7bit == 0)
                ALUctrl = "SRL";
            else
                ALUctrl = "SRA";
        }
        else if (func3 == "100")
            ALUctrl = "XOR";
        else if (func3 == "110")
            ALUctrl = "OR";
        else if (func3 == "111")
            ALUctrl = "AND";
        else if (func3 == "001")
            ALUctrl = "SLL";
    }
    return ALUctrl;
}

string ALU(int val1, int val2, string ALUctrl, bool &branchFlag)
{
    // cout<<"val1 "<<val1<<" val2 "<<val2<<" ALUctrl "<<ALUctrl<<endl;
    if (ALUctrl == "BEQ")
    {
        if (val1 == val2)
        {
            branchFlag |= true;
            IFID.valid = false;
            return "";
        }
        else
        {
            return "";
        }
    }
    else if (ALUctrl == "BNE")
    {
        if (val1 != val2)
        {
            branchFlag |= true;
            IFID.valid = false;
            return "";
        }
        else
        {
            return "";
        }
    }
    else if (ALUctrl == "BLT")
    {
        if (val1 < val2)
        {
            branchFlag |= true;
            IFID.valid = false;
            return "";
        }
        else
        {
            return "";
        }
    }
    else if (ALUctrl == "BGE")
    {
        if (val1 >= val2)
        {
            branchFlag |= true;
            IFID.valid = false;
            return "";
        }
        else
        {
            return "";
        }
    }

    EXMO.valid = true;
    if (ALUctrl == "ADD")
    {
        int res = val1 + val2;
        string sres = to_string(res);
        return decimalToBinary(sres, 32, true);
    }
    if (ALUctrl == "SUB")
    {
        int res = val1 - val2;
        string sres = to_string(res);
        return decimalToBinary(sres, 32, true);
    }
    if (ALUctrl == "OR")
    {
        int res = val1 | val2;
        string sres = to_string(res);
        return decimalToBinary(sres, 32, true);
    }
    if (ALUctrl == "XOR")
    {
        int res = val1 ^ val2;
        string sres = to_string(res);
        return decimalToBinary(sres, 32, true);
    }
    if (ALUctrl == "AND")
    {
        int res = val1 & val2;
        string sres = to_string(res);
        return decimalToBinary(sres, 32, true);
    }
    if (ALUctrl == "SLL")
    {
        int res = val1 << val2;
        string sres = to_string(res);
        return decimalToBinary(sres, 32, true);
    }
    if (ALUctrl == "SRL")
    {
        int res = val1 >> val2;
        string sres = to_string(res);
        return decimalToBinary(sres, 32, true);
    }
    if (ALUctrl == "SRA")
    {
        int res = val1 >> val2;
        string sres = to_string(res);
        return decimalToBinary(sres, 32, true);
    }
    return "";
}

void instructionFetch()
{
    if (IFID.stall)
        return;
    if (PC.valid)
    {
        if (PC.value < 0 || PC.value >= instructionMemory.size())
        {
            IFID.valid = false;
            PC.valid = false;
            return;
        }
        IFID.IR = instructionMemory[PC.value];
        IFID.DPC = PC.value;
        IFID.NPC = PC.value + 1;
        IFID.valid = true;
        PC.value += 1;
    }
    return;
}

string signedExtend()
{
    return IFID.IR.substr(0, 20);
}

void instructionDecode()
{
    if (IFID.valid)
    {
        // IDEX.JPC=IFID.NPC+(stoi(signedExtend()));
        IDEX.DPC = IFID.DPC;
        IDEX.imm = IFID.IR.substr(0, 12);
        IDEX.func = IFID.IR.substr(17, 3);
        IDEX.rdl = IFID.IR.substr(20, 5);
        IDEX.immst = IFID.IR.substr(0, 7) + IFID.IR.substr(20, 5);
        string s = IFID.IR.substr(0, 1) + IFID.IR.substr(20, 1) + IFID.IR.substr(1, 6) + IFID.IR.substr(21, 4);
        IDEX.immbr = s;
        string opcode = IFID.IR.substr(25, 7);
        controlUnit(IDEX.cw, opcode);
        if (IDEX.cw.regRead)
        {
            string rs1 = IFID.IR.substr(12, 5);
            int irs1 = binaryToDecimal(rs1, false);
            if (reg[irs1].ins == -1)
                IDEX.rs1 = IFID.IR.substr(12, 5);
            else
            {
                IFID.stall = true;
                IDEX.valid = false;
                return;
            }
        }
        string rs2 = IFID.IR.substr(7, 5);
        int irs2 = binaryToDecimal(rs2, false);
        if ((!IDEX.cw.ALUSrc || opcode == "0100011") && reg[irs2].ins != -1)
        {
            IFID.stall = true;
            IDEX.valid = false;
            return;
        }
        IDEX.rs2 = IFID.IR.substr(7, 5);
        int rdl = binaryToDecimal(IDEX.rdl, false);
        if (IDEX.cw.regWrite)
        {
            reg[rdl].ins = IDEX.DPC;
        }
        IDEX.valid = true;
        IFID.stall = false;
    }
    else
        IDEX.valid = false;
    return;
}

void instructionExecution()
{
    if (IDEX.valid)
    {
        string ALUSelect = ALUControl(IDEX.cw.ALUOp, IFID.IR[1], IDEX.func);

        string rs1 = IDEX.rs1;
        int irs1 = binaryToDecimal(rs1, false);
        int val1 = reg[irs1].value;

        int val2;
        if (IDEX.cw.ALUSrc == 0)
        {
            if (IDEX.cw.ALUOp == 'S')
            {
                int imm = binaryToDecimal(IDEX.immst, true);
                val2 = imm;
            }
            else
            {
                string rs2 = IDEX.rs2;
                int irs2 = binaryToDecimal(rs2, false);
                val2 = reg[irs2].value;
            }
        }
        else
        {
            int imm = binaryToDecimal(IDEX.imm, true);
            val2 = imm;
        }

        bool branchFlag = false;
        EXMO.ALUOUT = ALU(val1, val2, ALUSelect, branchFlag);

        if (IDEX.cw.BnJ == 1)
        {
            if (branchFlag)
            {
                PC.value = IDEX.DPC;
                int imm = binaryToDecimal(IDEX.immbr, true);
                PC.value += imm;
                PC.valid = true;
            }
        }

        EXMO.immst = IDEX.immst;
        EXMO.cw = IDEX.cw;
        EXMO.rdl = IDEX.rdl;
        EXMO.rs2 = IDEX.rs2;
    }
    else
        EXMO.valid = false;
    return;
}

void memoryOperation()
{
    if (EXMO.valid)
    {
        if (EXMO.cw.memRead || EXMO.cw.memWrite)
        {
            int ea = binaryToDecimal(EXMO.ALUOUT, true);
            if (ea < 0 || ea >= 1023)
            {
                cout << "EA out of bounds" << endl;
                return;
            }
            if (EXMO.cw.memWrite)
            {
                int irs2 = binaryToDecimal(EXMO.rs2, false);
                mem[ea] = reg[irs2].value;
            }
            if (EXMO.cw.memRead)
            {
                string smem = to_string(mem[ea]);
                MOWB.LDOUT = decimalToBinary(smem, 32, true);
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
    }
    else
        MOWB.valid = false;
}

void writeBack()
{
    if (MOWB.valid)
    {
        string writeData;
        if (MOWB.cw.mem2Reg)
            writeData = MOWB.LDOUT;
        else
            writeData = MOWB.ALUOUT;
        if (MOWB.cw.regWrite)
        {
            int rdl = binaryToDecimal(MOWB.rdl, false);
            reg[rdl].value = binaryToDecimal(writeData, true);
            reg[rdl].ins = -1;
        }
    }
}

void printer()
{
    for (int i = 0; i < 32; i++)
    {
        if (reg[i].value != 0)
            cout << "Reg" << i << " " << reg[i].value << " ";
    }
    cout << endl;

    for (int i = 0; i < 1023; i++)
    {
        if (mem[i] != 1e9)
            cout << "Mem" << i << " " << mem[i] << " ";
    }
    cout << endl;
}

void printdebug()
{
    cout << "PC " << PC.value << endl;
    cout << "IFID regs: " << "IFID.DPC " << IFID.DPC << " IFID.NPC " << IFID.NPC << endl;
    cout << "IDEX regs: " << "IDEX.cw.ALUOp " << IDEX.cw.ALUOp << " IDEX.func " << IDEX.func << " IDEX.imm " << IDEX.imm << " IDEX.immst " << IDEX.immst << " IDEX.rs1 " << IDEX.rs1 << " IDEX.rs2 " << IDEX.rs2 << endl;
    cout << "EXMO regs: " << " EXMO.ALUOUT " << EXMO.ALUOUT << " EXMO.rdl " << EXMO.rdl << " EXMO.rs2 " << EXMO.rs2 << endl;
    cout << "MOWB regs: " << " MOWB.ALUOUT " << MOWB.ALUOUT << " MOWB.LDOUT " << MOWB.LDOUT << " MOWB.rdl " << MOWB.rdl << endl;
    return;
}

signed main()
{
    cout << "Enter the number of instructions you wish to enter\n";
    int n;
    cin >> n;
    cin.ignore();
    int n1 = n;
    PC.value = 0;
    while (n1--)
    {
        string s;
        getline(cin, s);
        vector<string> parsed;
        parsed = parseInstruction(s);
        char type = instructionType(parsed[0]);

        string bincode;
        if (type == 'r')
            bincode = rType(parsed);
        else if (type == 'i')
            bincode = iType(parsed);
        else if (type == 'l')
            bincode = loadType(parsed);
        else if (type == 's')
            bincode = storeType(parsed);
        else if (type == 'b')
            bincode = bType(parsed);
        else if (type == 'u')
            bincode = uType(parsed);
        else if (type == 'j')
            bincode = jType(parsed);

        instructionMemory.push_back(bincode);
    }

    for (int i = 0; i < instructionMemory.size(); i++)
    {
        cout << "Binary Code for Instruction " << i + 1 << " is:" << instructionMemory[i] << endl;
    }

    bool validcheck = true;
    PC.valid = true;
    int cycle = 1;

    while (validcheck)
    {
        cout << "Cycle: " << cycle << endl;
        cycle++;

        // sleep(1);
        // cout<<"Calling WriteBack()... "<<endl;
        writeBack();
        // cout<<"Calling MemoryOperation()... "<<endl;
        memoryOperation();
        // cout<<"Calling InstructionExecution()... "<<endl;
        instructionExecution();
        // cout<<"Calling InstructionDecode()... "<<endl;
        instructionDecode();
        // cout<<"Calling InstructionFetch()... "<<endl;
        instructionFetch();
        // cout<<"Calling Printer()... "<<endl;
        printer();
        // printdebug();
        cout << "Validcheck " << PC.valid << " " << IFID.valid << " " << IDEX.valid << " " << EXMO.valid << " " << MOWB.valid << endl
             << endl;
        validcheck = PC.valid | IFID.valid | IDEX.valid | EXMO.valid | MOWB.valid;
    }
    return 0;
}