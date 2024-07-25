#include "CDB.hpp"
#include "headers.hpp"
#include <cstdint>
#include <iostream>

int32_t reg[33] = {};
int32_t reg_depend[33] = {};
int32_t &pc = reg[32];
int32_t &pc_depend = reg_depend[32];
int8_t mem[1 << 20] = {};
int32_t clk = 0;

CommonDataBus cdb;

int main() {}