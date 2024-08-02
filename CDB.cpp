#include "CDB.hpp"
#include "headers.hpp"
#include "main.hpp"

bool CommonDataBus::active() const { return _src != 0; }

int32_t CommonDataBus::val() const { return value; }

int32_t CommonDataBus::src() const { return _src; }

int32_t CommonDataBus::nxt_val() const { return nxt_value; }

int32_t CommonDataBus::nxt_src() const { return _nxt_src; }

void CommonDataBus::broadcast(int32_t name, int32_t val) {
  _nxt_src = name;
  nxt_value = val;
}

void CommonDataBus::execute() {
  _src = _nxt_src;
  value = nxt_value;
}

void CommonDataBus::reset() {
  _nxt_src = 0;
  nxt_value = 0;
}

void CommonDataBus::clear() {
  _src = _nxt_src = 0;
  value = nxt_value = 0;
}

void CommonDataBus::print() {
  if (_src == 0) {
    std::cout << "CDB: NONE" << std::endl;
  } else {
    std::cout << std::hex << "CDB: SRC: " << _src << " VAL: " << value
              << std::endl;
  }
}