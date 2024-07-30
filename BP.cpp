#include "BP.hpp"
#include "headers.hpp"
#include "main.hpp"
#include "utility.hpp"
#include <functional>

std::pair<bool, bool> BranchPredictor::read(const Inst &inst) {

  // if (brq.full()) {
  //   return {false, false};
  // }

  const bool branch = predict();
  BPUnit unit;
  unit.branch = branch;
  unit.src = inst.serial;
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

  if (branch.src == cdb.src()) {

#ifdef DEBUG
    std::cout << "BRANCH MONITORED: ";
#endif
    const bool res =
        (cdb.val() ? true : false); // true for taken and false for not
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
      lsb.clear();
      rob.clear();
      reg_depend_clear();
      nxt_pc = branch.fail;
    }
  }
}