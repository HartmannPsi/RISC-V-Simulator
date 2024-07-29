#pragma once
#ifndef INSTRUCTON_HPP
#define INSTRUCTON_HPP

#include "main.hpp"
#include <cstdint>

class Inst {
public:
  uint8_t opcode = -1, subop1 = -1, subop2 = -1;
  uint8_t rs1 = -1, rs2 = -1, rd = -1;
  int32_t imm = 0;
  int32_t serial = 0;

  Inst() {}

  ~Inst() {}

  Inst(int32_t command);

  void decode(int32_t command);

  void print();
};

#endif