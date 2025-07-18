#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <cctype>

using namespace std;

string hexa_to_binary(const string &s);
string decimal_to_binary(const string &s, int len, bool imm);
int binary_to_decimal(const string &s, bool imm);
vector<string> parse_instruction(const string &s);

// Type conversion functions
string r_type(const vector<string> &v);
string i_type(const vector<string> &v);
string load_type(const vector<string> &v);
string store_type(const vector<string> &v);
string b_type(const vector<string> &v);
string u_type(const vector<string> &v);
string j_type(const vector<string> &v);

// Utility
char instruction_type(const string &s);
