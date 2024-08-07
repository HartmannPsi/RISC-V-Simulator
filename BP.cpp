#include "BP.hpp"
#include "headers.hpp"
#include "main.hpp"
#include "utility.hpp"

//#define DEBUG

PredictFSM &BranchPredictor::distribute(int32_t addr) {
  int32_t ind = get_bits(addr, 7, 0);

  while (true) {

    if (fsms[ind].addr == 0 || fsms[ind].addr == addr) {
      fsms[ind].addr = addr;
      return fsms[ind];
    }

    ind = (ind + 1) % 300;
  }
}

std::pair<bool, bool> BranchPredictor::read(const Inst &inst) {

  auto addr = inst.addr;
  auto &fsm = distribute(addr);

  const bool branch = fsm.predict();
  BPUnit unit;
  unit.branch = branch;
  unit.src = inst.serial;
  unit.addr = inst.addr;
  unit.fail = (branch ? pc + 4 : inst.imm);
  bool res = brq.push(unit);

  if (!res) {
    throw std::pair<int, int>();
  }

  ++branch_num;
  return {branch, true};
}

void BranchPredictor::monitor() {
  if (brq.empty() || !cdb.active()) {
    return;
  }

  const auto &branch = brq.top();

  if (branch.src == cdb.nxt_src()) {

#ifdef DEBUG
    std::cout << "BRANCH MONITORED: ";
#endif
    const bool res =
        (cdb.nxt_val() ? true : false); // true for taken and false for not

    auto &fsm = distribute(branch.addr);
    fsm.update(res);

    if (res == branch.branch) { // prediction is right
#ifdef DEBUG
      std::cout << "SUCCEEDED\n";
#endif
      ++success_num;
      brq.pop();

    } else { // prediction is wrong
#ifdef DEBUG
      std::cout << "FAILED\n";
#endif
      brq.clear();
      foq.clear();
      rs.clear();
      lsb.clear_after(cdb.src());
      rob.clear();
      reg_depend_clear();
      nxt_pc = branch.fail;
      nxt_pc_src = 3;
    }
  }
}
