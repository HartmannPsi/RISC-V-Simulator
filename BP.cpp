#include "BP.hpp"
#include "headers.hpp"
#include "main.hpp"
#include "utility.hpp"
#include <functional>

bool BranchPredictor::read(const Inst &inst) {
  const bool branch = predict();
  BPUnit unit;
  unit.branch = branch;
  unit.src = inst.serial;
  unit.fail = (branch ? pc + 4 : inst.imm);
  brq.push(unit);
  ++branch_num;
  return branch;
}

void BranchPredictor::monitor() {
  if (brq.empty() || !cdb.active()) {
    return;
  }

  const auto &branch = brq.top();

  if (branch.src == cdb.src()) {
    const bool res =
        (cdb.val() ? true : false); // true for taken and false for not
    fsm.update(res);

    if (res == branch.branch) { // prediction is right
      ++success_num;
      brq.pop();

    } else { // prediction is wrong
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