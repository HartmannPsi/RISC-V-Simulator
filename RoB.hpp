#pragma once
#ifndef ROB_HPP
#define ROB_HPP

#include "main.hpp"
#include "queue.hpp"
#include <cstdint>

struct RoBUnit {
  bool busy = true;
  int32_t src = 0;
  // int8_t dest = -1;
  int32_t val = 0;
};

class ReorderBuffer {
public:
  Queue<RoBUnit, 100> buf_queue;

  ReorderBuffer() {}

  ~ReorderBuffer() {}

  bool push(int32_t _src);

  bool empty() const;

  bool full() const;

  bool pop();

  void submit(int32_t _src, int32_t _val);
};

#endif