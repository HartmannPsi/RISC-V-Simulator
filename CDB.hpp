#pragma once
#ifndef CDB_HPP
#define CDB_HPP

#include "main.hpp"
#include <cstdint>

class CommonDataBus {
  int32_t value = 0, nxt_value = 0;
  int32_t _src = 0, nxt_src = 0;

public:
  CommonDataBus() {}

  ~CommonDataBus() {}

  bool active() const;

  int32_t val() const;

  int32_t src() const;

  void broadcast(int32_t name, int32_t val);

  void execute();

  void reset();
};

#endif