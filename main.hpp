#pragma once
#ifndef MAIN_HPP
#define MAIN_HPP

#include <cstdint>

enum RSName : int8_t {
  None = 0b0000,
  Add1 = 0b0001,
  Add2 = 0b0010,
  Add3 = 0b0011
  // And1 = 0b0100,
  // And2 = 0b0101,
  // Or1 = 0b0110,
  // Or2 = 0b0111,
  // Xor1 = 0b1000,
  // Xor2 = 0b1001
};

enum Mode : int8_t {
  NONE,
  ADD,
  SUB,
  AND,
  OR,
  XOR,
  ADDI,
  ANDI,
  ORI,
  XORI,
  SLTI,
  SLTIU,
  SLLI,
  SRLI,
  SRAI,
  SLL,
  SLT,
  SLTU,
  SRL,
  SRA,
  BEQ,
  BNE,
  BLT,
  BGE,
  BLTU,
  BGEU
};

extern int32_t reg[33];
extern int32_t reg_depend[33];
extern int32_t &pc;
extern int32_t &pc_depend;
extern int8_t mem[1 << 20];
extern int32_t clk;

class CommonDataBus;
extern CommonDataBus cdb;

#endif