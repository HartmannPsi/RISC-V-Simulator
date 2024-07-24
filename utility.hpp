#pragma once
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstdint>

// fetch 4-byte instruction from memory
int32_t fetch(int32_t addr);

// signed-extend an [bits-1:0] number to 32 bits
int32_t signed_extend(int32_t num, int bits);

int32_t unsigned_extend(int32_t num, int bits);

int32_t upper_imm();

#endif