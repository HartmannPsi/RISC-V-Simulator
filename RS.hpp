#pragma once
#ifndef RS_HPP
#define RS_HPP

#include <cstdint>

template <const int length> class ReservationStation {
public:
  class Inst {
    int8_t name;
    bool busy;
    int8_t opcode;
    int32_t vj, vk;
    int8_t qj, qk;
    int32_t imm;
  } buffer[length];

  ReservationStation();

  virtual ~ReservationStation() = 0;

  virtual bool full() const = 0;

  virtual bool launch() = 0;

  virtual bool empty() const = 0;
};

#endif