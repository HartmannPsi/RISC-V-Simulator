#pragma once
#ifndef RS_HPP
#define RS_HPP

#include "instruction.hpp"
#include <cstdint>

class InstBuffer {
public:
  RSName name = None;
  bool busy = false;
  Mode mode = NONE;
  int32_t serial = 0;
  // Inst inst;
  int32_t vj = 0, vk = 0;
  int32_t qj = 0, qk = 0;
  int32_t imm = 0;

  InstBuffer(RSName _name) : name(_name) {}

  InstBuffer() {}

  ~InstBuffer() {}
};

class ReservationStation {
public:
  InstBuffer buffer[3];

  ReservationStation();

  ~ReservationStation() {}

  bool full() const;

  void execute();

  bool read(const Inst &inst);

  bool empty() const;

  int free() const;
};
#endif