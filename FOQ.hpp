#pragma once
#ifndef FOQ_HPP
#define FOQ_HPP

#include "queue.hpp"
#include <cstdint>

class FpOpQueue {
  Queue<int32_t, 6> opq;

public:
  bool fetch();

  bool launch();
};

#endif