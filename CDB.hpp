#pragma once
#ifndef CDB_HPP
#define CDB_HPP

#include <cstdint>

class CommonDataBus {
  int32_t value;
  int8_t inst = 0;

public:
  bool active() const;

  int32_t val() const;

  int8_t name() const;

  bool broadcast(int8_t name, int32_t val);
};

#endif