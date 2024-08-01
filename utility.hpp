#pragma once
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstdint>

// fetch 4-byte instruction from memory, little-endian
uint32_t fetch(uint32_t addr);

// signed-extend an [bits-1:0] number to 32 bits
uint32_t signed_extend(uint32_t num, int bits);

uint32_t unsigned_extend(uint32_t num, int bits);

// int32_t upper_imm();

// get the [6:0] opcode of a command
uint8_t get_opcode(uint32_t command);

// get the [14:12] opcode of a command
uint8_t get_subop1(uint32_t command);

// get the [31:25] opcode of a command
uint8_t get_subop2(uint32_t command);

// get the [high:low] opcode of a command
uint32_t get_bits(uint32_t command, uint32_t high_bit, uint32_t low_bit);

// update the registers to newest value
void reg_update();

// clear the register file
void reg_depend_clear();

void input_process();

void print_mem(int32_t beg, int32_t end);

void print_reg(bool no_dep = false);

void step_pc();

#endif