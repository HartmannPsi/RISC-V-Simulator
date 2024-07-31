#include "instruction.hpp"
#include "headers.hpp"
#include "utility.hpp"
#include <iomanip>
#include <iostream>

Inst::Inst(int32_t command) { decode(command); }

void Inst::decode(int32_t command) {

  // TODO: SIGNED-EXTEND
  rd = -1;

  addr = pc;
  serial = clk + 1;
  opcode = get_opcode(command);
  if (opcode == 0b011'0111) { // lui
    rd = get_bits(command, 11, 7);
    imm = get_bits(command, 31, 12) << 12;

  } else if (opcode == 0b001'0111) { // auipc
    rd = get_bits(command, 11, 7);
    imm = get_bits(command, 31, 12) << 12;
    imm += pc; // imm = pc + offset

  } else if (opcode == 0b110'1111) { // jal
    rd = get_bits(command, 11, 7);
    imm |= get_bits(command, 31, 31) << 20;
    imm |= get_bits(command, 30, 21) << 1;
    imm |= get_bits(command, 20, 20) << 11;
    imm |= get_bits(command, 19, 12) << 12;
    imm = signed_extend(imm, 20);
    imm += pc; // imm = pc + offset

  } else if (opcode == 0b110'0111) { // jalr
    rd = get_bits(command, 11, 7);
    rs1 = get_bits(command, 19, 15);
    imm = get_bits(command, 31, 20);
    imm = signed_extend(imm, 11);

  } else if (opcode == 0b110'0011) { // beq / bne / blt / bge / bltu / bgeu
    subop1 = get_subop1(command);

    rs1 = get_bits(command, 19, 15);
    rs2 = get_bits(command, 24, 20);
    imm |= get_bits(command, 11, 8) << 1;
    imm |= get_bits(command, 7, 7) << 11;
    imm |= get_bits(command, 31, 31) << 12;
    imm |= get_bits(command, 30, 25) << 5;
    imm = signed_extend(imm, 12);
    imm += pc; // imm = pc + offset;

  } else if (opcode == 0b000'0011) { // lb / lh / lw / lbu / lhu
    subop1 = get_bits(command, 14, 12);
    rd = get_bits(command, 11, 7);
    rs1 = get_bits(command, 19, 15);
    imm = get_bits(command, 31, 20);
    imm = signed_extend(imm, 11);

  } else if (opcode == 0b010'0011) { // sb / sh / sw
    subop1 = get_bits(command, 14, 12);
    rs1 = get_bits(command, 19, 15);
    rs2 = get_bits(command, 24, 20);
    imm |= get_bits(command, 11, 7);
    imm |= get_bits(command, 31, 25) << 5;
    imm = signed_extend(imm, 11);

  } else if (opcode == 0b001'0011) { // addi / slti / sltiu / xori / ori / andi
                                     // slli / srli / srai
    subop1 = get_bits(command, 14, 12);
    rd = get_bits(command, 11, 7);
    rs1 = get_bits(command, 19, 15);

    if (subop1 == 0b001) { // slli
      // shamt
      imm = get_bits(command, 24, 20);

    } else if (subop1 == 0b101) { // srli / srai
      // shamt
      imm = get_bits(command, 24, 20);
      subop2 = get_bits(command, 31, 25);

    } else {
      imm = get_bits(command, 31, 20);
    }
    imm = signed_extend(imm, 11);

  } else if (opcode == 0b011'0011) { // add / sub / sll / slt / sltu / xor / srl
                                     // sra / or / and
    subop1 = get_bits(command, 14, 12);
    rd = get_bits(command, 11, 7);
    rs1 = get_bits(command, 19, 15);
    rs2 = get_bits(command, 24, 20);

    if (subop1 == 0b000) { // add / sub
      subop2 = get_bits(command, 31, 25);
    } else if (subop1 == 0b101) { // srl / sra
      subop2 = get_bits(command, 31, 25);
    }
  }
}

void Inst::print() {
  std::cout << '@' << addr << " SER " << serial << ": ";
  std::cout << std::hex;
  if (opcode == 0b011'0111) { // lui
    std::cout << "lui " << uint32_t(rd) << ' ' << imm << '\n';

  } else if (opcode == 0b001'0111) { // auipc
    std::cout << "auipc " << uint32_t(rd) << ' ' << imm << '\n';
  } else if (opcode == 0b110'1111) { // jal
    std::cout << "jal " << uint32_t(rd) << ' ' << imm << '\n';
  } else if (opcode == 0b110'0111) { // jalr
    std::cout << "jalr " << uint32_t(rd) << ' ' << uint32_t(rs1) << ' ' << imm
              << '\n';
  } else if (opcode == 0b110'0011) { // beq / bne / blt / bge / bltu / bgeu
    std::cout << "br? " << uint32_t(rs1) << ' ' << uint32_t(rs2) << ' ' << imm
              << '\n';

  } else if (opcode == 0b000'0011) { // lb / lh / lw / lbu / lhu
    std::cout << "ld? " << uint32_t(rd) << ' ' << imm << '(' << uint32_t(rs1)
              << ")\n";
  } else if (opcode == 0b010'0011) { // sb / sh / sw
    std::cout << "st? " << uint32_t(rs2) << ' ' << imm << '(' << uint32_t(rs1)
              << ")\n";
  } else if (opcode == 0b001'0011) { // addi / slti / sltiu / xori / ori / andi
                                     // slli / srli / srai
    std::cout << "opi? " << uint32_t(rd) << ' ' << uint32_t(rs1) << ' ' << imm
              << '\n';

  } else if (opcode == 0b011'0011) { // add / sub / sll / slt / sltu / xor / srl
                                     // sra / or / and

    std::cout << "op? " << uint32_t(rd) << ' ' << uint32_t(rs1) << ' '
              << uint32_t(rs2) << '\n';
  }
}