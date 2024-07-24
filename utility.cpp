#include "headers.hpp"

// fetch 4-byte instruction from memory
int32_t fetch(int32_t addr) {
  // TODO
  return 0;
}

// signed-extend an [bits-1:0] number to 32 bits
int32_t signed_extend(int32_t num, int bits) {
  if (num & (1 << (bits - 1))) { // negative
    num += ~((1 << bits) - 1);
  }

  return num;
}

int32_t unsigned_extend(int32_t num, int bits) { return num; }

int32_t upper_imm();