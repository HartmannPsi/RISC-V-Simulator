#include "utility.hpp"
#include "headers.hpp"
#include "main.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

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

void input_process() {
  std::string line;
  int32_t addr = 0;
  while (!std::cin.eof()) {
    std::getline(std::cin, line);
    if (line[0] == '@') { // change addr
      std::stringstream in(line.substr(1, 8));
      in >> std::hex >> addr;
      continue;
    }

    std::stringstream in(line);
    while (!in.eof()) {
      int tmp = 0;
      in >> std::hex >> tmp;
      mem[addr] = uint8_t(tmp);
      ++addr;
    }
    --addr;
  }
}

void print_mem(int32_t beg, int32_t end) {
  int nl = 0;
  for (int32_t addr = beg; addr != end + 1; ++addr) {
    uint32_t tmp = uint8_t(mem[addr]);
    std::cout << std::setfill('0') << std::setw(2) << std::hex << tmp << ' ';
    ++nl;
    if (nl == 16) {
      std::cout << '\n';
      nl = 0;
    }
  }

  std::cout << '\n';
}

void print_reg() {
  std::cout << "REG STATE:\n";
  for (int32_t addr = 0; addr != 32; ++addr) {
    std::cout << "reg " << std::setfill('0') << std::setw(2) << addr << ": "
              << std::setfill('0') << std::setw(8) << std::hex << reg[addr];
    std::cout << " DEP: ";
    if (reg_depend[addr] != 0) {
      std::cout << reg_depend[addr] << '\n';
    } else {
      std::cout << "NONE\n";
    }
  }
}