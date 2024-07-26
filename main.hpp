#pragma once
#ifndef MAIN_HPP
#define MAIN_HPP

#include <cstdint>

enum RSName : int8_t {
  None = 0b0000,
  ALURS1 = 0b0001,
  ALURS2 = 0b0010,
  ALURS3 = 0b0011
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
  BGEU,
  LB,
  LH,
  LW,
  LBU,
  LHU,
  SB,
  SH,
  SW,
  JALR
};

extern int32_t reg[33];
extern int32_t reg_depend[33];
extern int32_t &pc;
extern int32_t &pc_depend;
extern int8_t mem[1 << 20];
extern int32_t clk;
extern int32_t nxt_pc;

class CommonDataBus;
extern CommonDataBus cdb;
class ReservationStation;
extern ReservationStation rs;
class FpOpQueue;
extern FpOpQueue foq;
class ReorderBuffer;
extern ReorderBuffer rob;
class LSBuffer;
extern LSBuffer lsb;
class BranchPredictor;
extern BranchPredictor bp;

constexpr int32_t EOI = 0x0ff00513;

#endif