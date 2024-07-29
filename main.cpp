#include "BP.hpp"
#include "headers.hpp"
#include "utility.hpp"
#include <cstdint>
#include <iostream>

int32_t reg[33] = {};
int32_t reg_depend[33] = {};
int32_t &pc = reg[32];
int32_t &pc_depend = reg_depend[32];
int8_t mem[1 << 20] = {};
int32_t clk = 0;
int32_t nxt_pc = 0;

CommonDataBus cdb;
ReservationStation rs;
FpOpQueue foq;
ReorderBuffer rob;
LSBuffer lsb;
BranchPredictor bp;

int main() {
  while (true) { // clock cycle

    pc = nxt_pc;

    cdb.execute();
    bp.monitor();
    foq.try_unlock();
    foq.fetch();
    foq.launch();
    rs.execute();
    lsb.execute();
    reg_update();

    if (fetch(pc) == EOI && rob.empty()) {
      std::cout << get_bits(reg[10], 7, 0);
      return 0;
    }

    rob.pop();
    ++clk;
  }

  return 0;
}