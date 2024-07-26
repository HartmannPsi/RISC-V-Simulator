#include "headers.hpp"
#include "utility.hpp"

bool FpOpQueue::fetch() {
  if (opq.full()) {
    return false;
  }

  opq.push(::fetch(pc));
  nxt_pc = pc + 4;

  return true;
}

bool FpOpQueue::launch() {
  if (opq.empty()) {
    return false;
  }

  Inst inst(opq.top());

  if (inst.opcode == 0b110'1111) { // jal

    // TODO
    return false;

  } else if (inst.opcode == 0b110'0111) { // jalr

    // TODO
    return false;

  } else if (inst.opcode == 0b000'0011 ||
             inst.opcode == 0b010'0011) { // ls insts

    const bool res = lsb.read(inst);
    if (res) {
      rob.push(inst.serial);
      opq.pop();
      return true;
    } else {
      return false;
    }

  } else { // alu insts

    const bool res = rs.read(inst);
    if (res) {
      rob.push(inst.serial);
      opq.pop();
      return true;
    } else {
      return false;
    }
  }
}