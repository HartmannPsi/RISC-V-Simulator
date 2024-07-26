#include "headers.hpp"
#include "main.hpp"

// fetch 4-byte instruction from memory, litte-endian
uint32_t fetch(uint32_t addr) {
  return (uint32_t)mem[addr] | ((uint32_t)mem[addr + 1] << 8) |
         ((uint32_t)mem[addr + 2] << 16) | ((uint32_t)mem[addr + 3] << 24);
}

// signed-extend an [bits-1:0] number to 32 bits
uint32_t signed_extend(uint32_t num, int bits) {
  if (bits == 31) {
    return num;
  }

  if (num & (1 << (bits))) { // negative
    num |= ~((1 << (bits + 1)) - 1);
  }

  return num;
}

uint32_t unsigned_extend(uint32_t num, int bits) { return num; }

// get the [6:0] opcode of a command
uint8_t get_opcode(uint32_t command) { return command & ((1 << 7) - 1); }

// get the [14:12] opcode of a command
uint8_t get_subop1(uint32_t command) {
  const uint32_t mask = ((1 << 15) - 1) & ~((1 << 12) - 1);
  return (command & mask) >> 12;
}

// get the [31:25] opcode of a command
uint8_t get_subop2(uint32_t command) {
  const uint32_t mask = ~((1 << 25) - 1);
  return (command & mask) >> 25;
}

// get the [high:low] opcode of a command
uint32_t get_bits(uint32_t command, uint32_t high_bit, uint32_t low_bit) {

  if (high_bit == 31) {
    const uint32_t mask = ~((1 << low_bit) - 1);
    return (command & mask) >> low_bit;
  }

  const uint32_t mask = ((1 << (high_bit + 1)) - 1) & ~((1 << low_bit) - 1);
  return (command & mask) >> low_bit;
}

void reg_update() {
  reg[0] = reg_depend[0] = 0;
  if (!cdb.active()) {
    return;
  }

  for (int i = 1; i != 32; ++i) {
    if (reg_depend[i] == cdb.src()) {
      reg[i] = cdb.val();
      reg_depend[i] = 0;
    }
  }
}

// clear the register file
void reg_depend_clear() {
  for (int i = 0; i != 32; ++i) {
    reg_depend[i] = 0;
  }
}