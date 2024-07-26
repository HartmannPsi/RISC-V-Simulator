#pragma once
#ifndef LSB_HPP
#define LSB_HPP

#include "instruction.hpp"
#include "main.hpp"
#include "queue.hpp"
#include <cstdint>

struct LSInst {
  Mode mode = NONE;
  int32_t serial = 0;
  int32_t imm = 0;
  int32_t vj = 0, vk = 0, qj = 0, qk = 0;
  int8_t state = 0; // -1 for pending, 0 ~ 2 for carrying, 3 for done

  LSInst() {}

  ~LSInst() {}
};

class LSBuffer {
public:
  Queue<LSInst, 6> buffer;

  LSBuffer() {}

  ~LSBuffer() {}

  bool full() const;

  bool empty() const;

  bool read(const Inst &inst);

  void execute();
};

#endif