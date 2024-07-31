#pragma once
#ifndef BP_HPP
#define BP_HPP
#include "instruction.hpp"
#include "queue.hpp"
#include <cstdint>
#include <utility>

enum State : int8_t { SNT, WNT, WT, ST };

class PredictFSM {
  State state;

public:
  int32_t addr = 0;

  PredictFSM() : state(WNT) {}

  PredictFSM(State _state) : state(_state) {}

  ~PredictFSM() {}

  void update(bool branch) {
    switch (state) {
    case SNT:
      state = branch ? WNT : SNT;
      break;
    case WNT:
      state = branch ? WT : SNT;
      break;
    case WT:
      state = branch ? ST : WNT;
      break;
    case ST:
      state = branch ? ST : WT;
    }
  }

  bool predict() const { return state == WT || state == ST; }

  void reset() { state = WNT; }
};

struct BPUnit {
  int32_t src = 0, fail = 0;
  int32_t addr = 0;
  bool branch = false;

  BPUnit() {}

  ~BPUnit() {}
};

class BranchPredictor {
  PredictFSM fsms[300];
  int32_t branch_num = 0, success_num = 0;
  Queue<BPUnit, 100> brq;

public:
  BranchPredictor() {}

  ~BranchPredictor() {}

  // bool predict() const { return fsm.predict(); }

  std::pair<bool, bool> read(const Inst &inst); // <branch, res>

  void monitor();

  double rate() { return double(success_num) / double(branch_num); }

  int32_t brn() { return branch_num; }

  int32_t scn() { return success_num; }

  PredictFSM &distribute(int32_t addr);
};

#endif