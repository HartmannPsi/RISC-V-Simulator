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

  void clear() {
    busy = false;
    mode = NONE;
    serial = 0;
    vj = 0, vk = 0;
    qj = 0, qk = 0;
    imm = 0;
  }
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

  void clear();
};
#endif