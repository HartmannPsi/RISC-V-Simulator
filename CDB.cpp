#include "CDB.hpp"
#include "headers.hpp"
#include "main.hpp"

bool CommonDataBus::active() const { return _src != 0; }

int32_t CommonDataBus::val() const { return value; }

int32_t CommonDataBus::src() const { return _src; }

void CommonDataBus::broadcast(int32_t name, int32_t val) {
  nxt_src = name;
  nxt_value = val;
}

void CommonDataBus::execute() {
  _src = nxt_src;
  value = nxt_value;
  reset();
}

void CommonDataBus::reset() {
  nxt_src = 0;
  nxt_value = 0;
}