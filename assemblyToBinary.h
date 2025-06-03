#pragma once
#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

string hexaToBinary(string &s)
{
    string bin;

    int i = 2;
    while (s[i] == '0')
        i++;

    unordered_map<char, string> mpp;
    mpp['0'] = "0000";
    mpp['1'] = "0001";
    mpp['2'] = "0010";
    mpp['3'] = "0011";
    mpp['4'] = "0100";
    mpp['5'] = "0101";
    mpp['6'] = "0110";
    mpp['7'] = "0111";
    mpp['8'] = "1000";
    mpp['9'] = "1001";
    mpp['A'] = "1010";
    mpp['B'] = "1011";
    mpp['C'] = "1100";
    mpp['D'] = "1101";
    mpp['E'] = "1110";
    mpp['F'] = "1111";

    for (int j = i; j < s.size(); j++)
    {
        bin += mpp[s[j]];
    }

    return bin;
}

string decimalToBinary(string &s, int len, bool imm)
{
    string bin;
    if (!imm)
    {
        int num = stoi(s);
        while (num != 0)
        {
            bin += ('0' + num % 2);
            num /= 2;
        }

        reverse(bin.begin(), bin.end());

        if (bin.size() < len)
        {
            string ext;
            int cnt = len - bin.size();
            while (cnt--)
                ext += '0';
            ext += bin;
            return ext;
        }
        else
        {
            return bin.substr(bin.size() - len);
        }
    }
    else
    {
        int sgn = 0;
        int num = 0;
        if (s[0] == '-' || s[0] == '+')
        {
            sgn++;
            num = stoi(s.substr(1));
        }
        else
            num = stoi(s);

        while (num != 0)
        {
            bin += ('0' + num % 2);
            num /= 2;
        }

        reverse(bin.begin(), bin.end());

        if (bin.size() < len)
        {
            string ext;
            int cnt = len - bin.size();
            while (cnt--)
                ext += '0';
            ext += bin;
            if (sgn > 0)
                ext[0] = '1';
            else
                ext[0] = '0';
            return ext;
        }
        else
        {
            string ext = bin.substr(bin.size() - len);
            if (sgn > 0)
                ext[0] = '1';
            else
                ext[0] = '0';
            return ext;
        }
    }
}

int binaryToDecimal(string &s, bool imm)
{
    int sgn = 0;
    if (imm)
    {
        if (s[0] == '1')
            sgn++;
        s = s.substr(1);
    }
    int dec = 0;
    int po = 1;
    for (int i = s.size() - 1; i >= 0; i--)
    {
        dec += ((s[i] - '0') * po);
        po *= 2;
    }

    if (sgn > 0)
        dec = dec * (-1);
    return dec;
}

vector<string> parseInstruction(string &s)
{
    vector<string> v;
    string str;
    for (int i = 0; i < s.size(); i++)
    {
        if (s[i] == ' ' || s[i] == ',')
        {
            v.push_back(str);
            str.clear();
        }
        else
        {
            str += s[i];
        }
    }
    v.push_back(str);
    return v;
}

string rType(vector<string> &v)
{
    string ins;

    // func7
    if (v[0] == "SUB" || v[0] == "SRA")
        ins += "0100000";
    else
        ins += "0000000";

    // rs2
    string rs2 = v[3];
    string regno = rs2.substr(1);
    string temp = decimalToBinary(regno, 5, false);
    rs2.clear();
    rs2 += temp;
    ins += rs2;

    // rs1
    string rs1 = v[2];
    regno.clear();
    regno += rs1.substr(1);
    temp.clear();
    temp += decimalToBinary(regno, 5, false);
    rs1.clear();
    rs1 += temp;
    ins += rs1;

    // func3
    unordered_map<string, string> mpp;
    mpp["ADD"] = "000";
    mpp["SUB"] = "000";
    mpp["SLL"] = "001";
    mpp["SLT"] = "010";
    mpp["SLTU"] = "011";
    mpp["XOR"] = "100";
    mpp["SRL"] = "101";
    mpp["SRA"] = "101";
    mpp["OR"] = "110";
    mpp["AND"] = "111";

    ins += mpp[v[0]];

    // rd
    string rd = v[1];
    regno.clear();
    regno += rd.substr(1);
    temp.clear();
    temp += decimalToBinary(regno, 5, false);
    rd.clear();
    rd += temp;
    ins += rd;

    // opcode
    ins += "0110011";

    return ins;
}

string iType(vector<string> &v)
{
    string ins;

    // imm
    string temp = v[3];
    string imm;
    imm += decimalToBinary(temp, 12, true);
    ins += imm;

    // rs1
    string regno;
    string rs1 = v[2];
    regno.clear();
    regno += rs1.substr(1);
    temp.clear();
    temp += decimalToBinary(regno, 5, false);
    rs1.clear();
    rs1 += temp;
    ins += rs1;

    // func3
    unordered_map<string, string> mpp;
    mpp["ADDI"] = "000";
    mpp["SLLI"] = "001";
    mpp["SLTI"] = "010";
    mpp["SLTIU"] = "011";
    mpp["XORI"] = "100";
    mpp["SRLI"] = "101";
    mpp["SRAI"] = "101";
    mpp["ORI"] = "110";
    mpp["ANDI"] = "111";
    ins += mpp[v[0]];

    // rd
    string rd = v[1];
    regno.clear();
    regno += rd.substr(1);
    temp.clear();
    temp += decimalToBinary(regno, 5, false);
    rd.clear();
    rd += temp;
    ins += rd;

    // opcode
    ins += "0010011";

    return ins;
}

vector<string> loadParser(vector<string> &vec)
{
    vector<string> v(4);
    v[0] = vec[0];
    v[1] = vec[1];

    int i = 0;
    string imm;
    while (vec[2][i] != '[')
    {
        imm += vec[2][i];
        i++;
    }
    v[3] = imm;

    i++;

    string rs1;
    while (vec[2][i] != ']')
    {
        rs1 += vec[2][i];
        i++;
    }
    v[2] = rs1;

    return v;
}

string loadType(vector<string> &vec)
{
    string ins;

    vector<string> v = loadParser(vec);

    // imm
    string temp = v[3];
    string imm;
    imm += decimalToBinary(temp, 12, true);
    ins += imm;

    // rs1
    string regno;
    string rs1 = v[2];
    regno.clear();
    regno += rs1.substr(1);
    temp.clear();
    temp += decimalToBinary(regno, 5, false);
    rs1.clear();
    rs1 += temp;
    ins += rs1;

    // func3
    unordered_map<string, string> mpp;
    mpp["LB"] = "000";
    mpp["LH"] = "001";
    mpp["LW"] = "010";

    mpp["LBU"] = "100";
    mpp["LHU"] = "101";

    ins += mpp[v[0]];

    // rd
    string rd = v[1];
    regno.clear();
    regno += rd.substr(1);
    temp.clear();
    temp += decimalToBinary(regno, 5, false);
    rd.clear();
    rd += temp;
    ins += rd;

    // opcode
    ins += "0000011";

    return ins;
}

vector<string> storeParser(vector<string> &vec)
{
    cout << endl;
    vector<string> v(4);
    v[0] = vec[0];
    v[1] = vec[1];

    int i = 0;
    string imm;
    while (vec[2][i] != '[')
    {
        imm += vec[2][i];
        i++;
    }
    v[3] = imm;

    i++;

    string rs2;
    while (vec[2][i] != ']')
    {
        rs2 += vec[2][i];
        i++;
    }
    v[2] = rs2;

    return v;
}

string storeType(vector<string> &vec)
{
    string ins;

    vector<string> v = storeParser(vec);

    // imm[11:5]
    string temp = v[3];
    string imm;
    imm += decimalToBinary(temp, 12, true);
    ins += imm.substr(0, 7);

    // rs2
    string rs2 = v[1];
    string regno = rs2.substr(1);
    temp.clear();
    temp = decimalToBinary(regno, 5, false);
    rs2.clear();
    rs2 += temp;
    ins += rs2;

    // rs1
    string rs1 = v[2];
    regno.clear();
    regno += rs1.substr(1);
    temp.clear();
    temp += decimalToBinary(regno, 5, false);
    rs1.clear();
    rs1 += temp;
    ins += rs1;

    // func3
    unordered_map<string, string> mpp;
    mpp["SB"] = "000";
    mpp["SH"] = "001";
    mpp["SW"] = "010";

    ins += mpp[v[0]];

    // imm[4:0]
    ins += imm.substr(7, 5);

    // opcode
    ins += "0100011";

    return ins;
}

string bType(vector<string> &v)
{
    string ins;

    // imm[12]
    string temp = v[3];
    string imm;
    imm += decimalToBinary(temp, 12, true);
    ins += imm.substr(0, 1);

    // imm[10:5]
    ins += imm.substr(2, 6);

    // rs2
    string rs2 = v[1];
    string regno = rs2.substr(1);
    temp.clear();
    temp = decimalToBinary(regno, 5, false);
    rs2.clear();
    rs2 += temp;
    ins += rs2;

    // rs1
    string rs1 = v[2];
    regno.clear();
    regno += rs1.substr(1);
    temp.clear();
    temp += decimalToBinary(regno, 5, false);
    rs1.clear();
    rs1 += temp;
    ins += rs1;

    // func3
    unordered_map<string, string> mpp;
    mpp["BEQ"] = "000";
    mpp["BNE"] = "001";
    mpp["BLT"] = "100";
    mpp["BGE"] = "101";
    mpp["BLTU"] = "110";
    mpp["BGEU"] = "111";

    ins += mpp[v[0]];

    // imm[11]
    ins += imm.substr(1, 1);

    // imm[4:1]
    ins += imm.substr(8, 4);

    // opcode
    ins += "1100011";

    return ins;
}

string uType(vector<string> &v)
{
    string ins;

    // imm
    string temp = v[2];
    string imm;
    imm += decimalToBinary(temp, 8, true);
    ins += imm;
    for (int i = 0; i < 12; i++)
    {
        ins += '0';
    }

    // rd
    string rd = v[1];
    string regno;
    regno += rd.substr(1);
    temp.clear();
    temp += decimalToBinary(regno, 5, false);
    rd.clear();
    rd += temp;
    ins += rd;

    // opcode
    if (v[0] == "LUI")
        ins += "0110111";
    else if (v[0] == "AUIPC")
        ins += "0010111";

    return ins;
}

string jType(vector<string> &v)
{
    if (v[0] == "JALR")
    {
        string jalr = iType(v);
        jalr = jalr.substr(0, 25);
        jalr += "1100111";
        return jalr;
    }
    else
    {
        string ins;

        // imm
        string temp = v[2];
        string imm;
        imm += decimalToBinary(temp, 21, true);

        // imm[20]
        ins += imm.substr(0, 1);

        // imm[10:1]
        ins += imm.substr(10, 10);

        // imm[11]
        ins += imm.substr(9, 1);

        // imm[19:12]
        ins += imm.substr(1, 8);

        // rd
        string rd = v[1];
        string regno = rd.substr(1);
        temp.clear();
        temp += decimalToBinary(regno, 5, false);
        rd.clear();
        rd += temp;
        ins += rd;

        // opcode
        ins += "1101111";

        return ins;
    }
}

char instructionType(string &s)
{
    unordered_set<string> r = {"ADD", "SUB", "XOR", "OR", "AND", "SLL", "SRL", "SRA", "SLT", "SLTU"};
    unordered_set<string> i = {"ADDI", "XORI", "ORI", "ANDI", "SLLI", "SRLI", "SRAI", "SLTI", "SLTUI"};
    unordered_set<string> l = {"LB", "LH", "LW", "LBU", "LHU"};
    unordered_set<string> st = {"SB", "SH", "SW"};
    unordered_set<string> b = {"BEQ", "BNE", "BLT", "BGE", "BLTU", "BGEU"};
    unordered_set<string> u = {"LUI", "AUIPC"};
    unordered_set<string> j = {"JAL", "JALR"};

    if (r.find(s) != r.end())
        return 'r';
    if (i.find(s) != i.end())
        return 'i';
    if (l.find(s) != l.end())
        return 'l';
    if (st.find(s) != st.end())
        return 's';
    if (b.find(s) != b.end())
        return 'b';
    if (u.find(s) != u.end())
        return 'u';
    if (j.find(s) != j.end())
        return 'j';

    return 'G';
}
