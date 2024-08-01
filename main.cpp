//#define DEBUG
//#define VIS

#include "BP.hpp"
#include "headers.hpp"
#include "utility.hpp"
#include <cstdint>
#include <iostream>

int32_t reg[33] = {};
int32_t reg_depend[33] = {};
int32_t &pc = reg[32];
int32_t &pc_depend = reg_depend[32];
uint8_t mem[1 << 20] = {};
int32_t clk = 0;
int32_t nxt_pc = 0;
int8_t nxt_pc_src = 0; // 0 for none, 1 for foq, 2 for bp

CommonDataBus cdb;
ReservationStation rs;
FpOpQueue foq;
ReorderBuffer rob;
LSBuffer lsb;
BranchPredictor bp;

/*
int32_t prev_reg[32] = {};

void comp() {
  bool output = false;
  for (int i = 0; i != 32; ++i) {
    if (reg[i] != prev_reg[i]) {
      output = true;
      prev_reg[i] = reg[i];
    }
  }

  if (output) {
    print_reg(true);
  }
}

void stopat(int t) {
  if (clk > t) {
    exit(0);
  }
}*/

int main() {

  int trouble = 0;

  std::cout << std::hex;
  input_process();

  while (true) { // clock cycle
#ifdef DEBUG
    std::cout << "-------------------------------------------\n";
#endif
    step_pc();

    // reg_update();

    foq.execute();
    cdb.execute();
    bp.monitor();
    lsb.execute();
    rs.execute();
    rob.pop();

    if (fetch(pc) == EOI && rob.empty()) {
      std::cout << std::dec << get_bits(reg[10], 7, 0) << '\n';

#ifdef VIS
      std::cout << "clk: " << clk << " monitor rate: " << std::setprecision(5)
                << bp.rate() << " succeeded: " << bp.scn()
                << " total: " << bp.brn() << '\n';
#endif

      return 0;
    }

    ++clk;

#ifdef DEBUG
    foq.print_top();
    rs.print();
    lsb.print();
    print_reg();
    rob.print_first();
    cdb.print();
    stopat(0xffff);

#endif
  }

  return 0;
}

std::ostream &operator<<(std::ostream &os, const Mode &op) {

  switch (op) {
  case NONE:
    os << "NONE";
    break;
  case ADD:
    os << "ADD";
    break;
  case SUB:
    os << "SUB";
    break;
  case AND:
    os << "AND";
    break;
  case OR:
    os << "OR";
    break;
  case XOR:
    os << "XOR";
    break;
  case ADDI:
    os << "ADDI";
    break;
  case ANDI:
    os << "ANDI";
    break;
  case ORI:
    os << "ORI";
    break;
  case XORI:
    os << "XORI";
    break;
  case SLTI:
    os << "SLTI";
    break;
  case SLTIU:
    os << "SLTIU";
    break;
  case SLLI:
    os << "SLLI";
    break;
  case SRLI:
    os << "SRLI";
    break;
  case SRAI:
    os << "SRAI";
    break;
  case SLL:
    os << "SLL";
    break;
  case SLT:
    os << "SLT";
    break;
  case SLTU:
    os << "SLTU";
    break;
  case SRL:
    os << "SRL";
    break;
  case SRA:
    os << "SRA";
    break;
  case BEQ:
    os << "BEQ";
    break;
  case BNE:
    os << "BNE";
    break;
  case BLT:
    os << "BLT";
    break;
  case BGE:
    os << "BGE";
    break;
  case BLTU:
    os << "BLTU";
    break;
  case BGEU:
    os << "BGEU";
    break;
  case LB:
    os << "LB";
    break;
  case LH:
    os << "LH";
    break;
  case LW:
    os << "LW";
    break;
  case LBU:
    os << "LBU";
    break;
  case LHU:
    os << "LHU";
    break;
  case SB:
    os << "SB";
    break;
  case SH:
    os << "SH";
    break;
  case SW:
    os << "SW";
    break;
  case JALR:
    os << "JALR";
    break;
  }
  return os;
}