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
  bool ready = false;

  LSInst() {}

  ~LSInst() {}

  void clear() { *this = LSInst(); }
};

class LSBuffer {
public:
  Queue<LSInst, 6> buffer;

  LSBuffer() {}

  ~LSBuffer() {}

  bool full() const;

  bool empty() const;

  bool read(const Inst &inst);

  // void update(int32_t src, int32_t res);

  void execute();

  void clear_after(int32_t serial);

  void print();

  void print_first();
};

#endif