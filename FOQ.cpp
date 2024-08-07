#include "headers.hpp"
#include "main.hpp"
#include "utility.hpp"
#include <iomanip>
#include <iostream>

//#define DEBUG

void FpOpQueue::lock() { locked = true; }

void FpOpQueue::try_unlock() {
  if (cdb.src() == unlock_inst) {
    locked = false;
  }
}

void FpOpQueue::clear() {
  while (!opq.empty()) {
    opq.pop();
  }
  locked = false;
  unlock_inst = 0;
}

bool FpOpQueue::fetch() {
  if (opq.full()) {
    return false;
  }
  if (locked) {
    return false;
  }

  Inst inst(::fetch(pc));

#ifdef DEBUG
  inst.print();
  std::cout << std::endl;
#endif

  if (::fetch(pc) == EOI) { // li a0 255
    if (nxt_pc_src == 0) {
      nxt_pc = pc;
      nxt_pc_src = 1;
    }

    return true;
  }

  if (inst.opcode == 0b110'1111) { // jal
    if (nxt_pc_src == 0) {
      nxt_pc = inst.imm;
      nxt_pc_src = 1;
    }
    inst.imm = pc + 4; // store to some regs
    opq.push(inst);
    rob.push(inst.serial, inst.rd);

  } else if (inst.opcode == 0b110'0111) { // jalr

    lock();
    unlock_inst = inst.serial;
    if (nxt_pc_src == 0) {
      nxt_pc = pc;
      nxt_pc_src = 1;
    }
    opq.push(inst);
    rob.push(inst.serial, inst.rd);

  } else if (inst.opcode == 0b110'0011) { // branch's

    const auto bp_res = bp.read(inst);

    if (bp_res.first) { // branch
      if (nxt_pc_src == 0) {
        nxt_pc = inst.imm;
        nxt_pc_src = 1;
      }
    } else { // don't branch
      if (nxt_pc_src == 0) {
        nxt_pc = pc + 4;
        nxt_pc_src = 1;
      }
    }
    opq.push(inst);
    rob.push(inst.serial, inst.rd);

  } else {
    if (nxt_pc_src == 0) {
      nxt_pc = pc + 4;
      nxt_pc_src = 1;
    }
    opq.push(inst);
    if (inst.opcode == 0b010'0011) { // store insts
      rob.push(inst.serial, inst.rd);
    } else {
      rob.push(inst.serial, inst.rd);
    }
  }

  return true;
}

bool FpOpQueue::launch() {
  if (opq.empty()) {
    return false;
  }

  auto &inst = opq.top();

  if (inst.opcode == 0b000'0011 || inst.opcode == 0b010'0011) { // ls insts

    const bool res = lsb.read(inst);
    if (res) {

      opq.pop();
      return true;
    } else {
      return false;
    }

  } else { // alu insts

    const bool res = rs.read(inst);
    if (res) {
      opq.pop();

      return true;
    } else {

      return false;
    }
  }
}

void FpOpQueue::print_top() {
  std::cout << "FOQ TOP: ";
  if (opq.empty()) {
    std::cout << "NONE\n";
    return;
  }
  opq.top().print();
}

void FpOpQueue::execute() {
  try_unlock();
  fetch();
  launch();
  bp.monitor();
}