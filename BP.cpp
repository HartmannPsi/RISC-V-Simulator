#include "headers.hpp"
#include "main.hpp"
#include "utility.hpp"

bool BranchPredictor::read(const Inst &inst) {
  const bool branch = predict();
  brq.push({inst.serial, branch});
  ++branch_num;
  return branch;
}

void BranchPredictor::monitor() {
  if (brq.empty() || !cdb.active()) {
    return;
  }

  const std::pair<int32_t, bool> branch = brq.top();

  if (branch.first == cdb.src()) {
    const bool res =
        (cdb.val() ? true : false); // true for taken and false for not
    fsm.update(res);

    if (res == branch.second) { // prediction is right
      ++success_num;
      brq.pop();

    } else { // prediction is wrong
      brq.clear();
      foq.clear();
      rs.clear();
      lsb.clear();
      rob.clear();
      reg_depend_clear();
    }
  }
}