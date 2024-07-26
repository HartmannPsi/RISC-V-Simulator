#pragma once
#ifndef FOQ_HPP
#define FOQ_HPP

#include "instruction.hpp"
#include "queue.hpp"
#include <cstdint>

class FpOpQueue {
  Queue<Inst, 6> opq;
  bool locked = false;
  int32_t unlock_inst = 0;

public:
  bool fetch();

  bool launch();

  FpOpQueue() {}

  ~FpOpQueue() {}

  void lock();

  void try_unlock();

  void clear();
};

#endif