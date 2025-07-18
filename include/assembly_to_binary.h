#pragma once
#include <string>
#include <vector>

std::string hexa_to_binary(const std::string &s);
std::string decimal_to_binary(const std::string &s, int len, bool imm);
int binary_to_decimal(const std::string &s, bool imm);
std::vector<std::string> parse_instruction(const std::string &s);

// Type conversion functions
std::string r_type(const std::vector<std::string> &v);
std::string i_type(const std::vector<std::string> &v);
std::string load_type(const std::vector<std::string> &v);
std::string store_type(const std::vector<std::string> &v);
std::string b_type(const std::vector<std::string> &v);
std::string u_type(const std::vector<std::string> &v);
std::string j_type(const std::vector<std::string> &v);

// Utility
char instruction_type(const std::string &s);
