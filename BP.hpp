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
};

class BranchPredictor {
  PredictFSM fsm;
  int32_t branch_num = 0, success_num = 0;
  Queue<std::pair<int32_t, bool>, 6> brq;

public:
  BranchPredictor() {}

  ~BranchPredictor() {}

  bool predict() const { return fsm.predict(); }

  bool read(const Inst &inst);

  void monitor();
};

#endif