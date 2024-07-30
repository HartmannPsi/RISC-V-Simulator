#include "LSB.hpp"
#include "headers.hpp"
#include "main.hpp"
#include "utility.hpp"
//#include <sys/types.h>

bool LSBuffer::full() const { return buffer.full(); }

bool LSBuffer::empty() const { return buffer.empty(); }

void LSBuffer::clear_after(int32_t serial) {

  Queue<LSInst, 6> tmp_queue;

  while (!buffer.empty()) {
    const auto &t = buffer.top();
    if (t.serial < serial) {
      tmp_queue.push(t);
    }

    buffer.top() = LSInst();
    buffer.pop();
  }

  while (!tmp_queue.empty()) {
    buffer.push(tmp_queue.top());
    tmp_queue.pop();
  }
}

bool LSBuffer::read(const Inst &inst) {
  if (buffer.full()) {
    return false;
  }

  LSInst tmp;

  tmp.serial = inst.serial;

  if (inst.opcode == 0b000'0011) { // ld insts

    tmp.ready = true;
    tmp.imm = inst.imm;
    if (reg_depend[inst.rs1] == 0) { // no depend
      tmp.vj = reg[inst.rs1];
    } else {
      tmp.qj = reg_depend[inst.rs1];
      tmp.state = -1;
    }
    reg_depend[inst.rd] = tmp.serial;
    reg_depend[0] = 0;

    if (inst.subop1 == 0b000) { // lb
      tmp.mode = LB;
    } else if (inst.subop1 == 0b001) { // lh
      tmp.mode = LH;
    } else if (inst.subop1 == 0b010) { // lw
      tmp.mode = LW;
    } else if (inst.subop1 == 0b100) { // lbu
      tmp.mode = LBU;
    } else { // lhu
      tmp.mode = LHU;
    }

  } else if (inst.opcode == 0b010'0011) { // st insts

    tmp.ready = false;
    tmp.imm = inst.imm;
    if (reg_depend[inst.rs1] == 0) { // no depend
      tmp.vj = reg[inst.rs1];
    } else {
      tmp.qj = reg_depend[inst.rs1];
      tmp.state = -1;
    }
    if (reg_depend[inst.rs2] == 0) { // no depend
      tmp.vk = reg[inst.rs2];
    } else {
      tmp.qk = reg_depend[inst.rs2];
      tmp.state = -1;
    }

    if (inst.subop1 == 0b000) { // sb
      tmp.mode = SB;
    } else if (inst.subop1 == 0b001) { // sh
      tmp.mode = SH;
    } else { // sw
      tmp.mode = SW;
    }
  } else {
    return false;
  }

  buffer.push(tmp);

  return true;
}

void LSBuffer::execute() {
  if (buffer.empty()) {
    return;
  }

  for (int i = 0; i != 6; ++i) {
    auto &buf = buffer.arr[i];

    if (buf.mode == NONE) {
      continue;
    }
    const bool flag = (buf.qj != 0 || buf.qk != 0);

    if (!buf.ready && cdb.active() && cdb.src() == buf.serial) {
      buf.ready = true;
    }

    if (buf.qj != 0 && cdb.src() == buf.qj) {
      buf.vj = cdb.val();
      buf.qj = 0;
    }
    if (buf.qk != 0 && cdb.src() == buf.qk) {
      buf.vk = cdb.val();
      buf.qk = 0;
    }

    if (flag) {
      if (buf.qj == 0 && buf.qk == 0) {
        buf.state = 0; // prepared
      }
    }
  }

  auto &buf = buffer.top();
  const bool flag = (buf.qj != 0 || buf.qk != 0);

  if (buf.qj != 0 && cdb.src() == buf.qj) {
    buf.vj = cdb.val();
    buf.qj = 0;
  }
  if (buf.qk != 0 && cdb.src() == buf.qk) {
    buf.vk = cdb.val();
    buf.qk = 0;
  }
  if (flag) {
    if (buf.qj == 0 && buf.qk == 0) {
      buf.state = 0; // prepared
    }
    return;
  }

  if (!buf.ready && cdb.active() && cdb.src() == buf.serial) {
    buf.ready = true;
  }

  if (!buf.ready) {
    return;
  }

  // if (buf.serial == 0xc5) {
  //   std::cout << "SER: c5"
  //             << " LOAD FROM 0x" << std::hex << buf.vj + buf.imm << '\n';
  // }

  if (buf.state == 3) { // carry out & broadcast
    int32_t res = 0;

    if (buf.mode == LB) {
      res = signed_extend(uint32_t(mem[buf.vj + buf.imm]), 7);
      rs.update(buf.serial, res);
      rob.submit(buf.serial, res);

    } else if (buf.mode == LBU) {
      res = uint32_t(mem[buf.vj + buf.imm]);
      rs.update(buf.serial, res);
      rob.submit(buf.serial, res);

    } else if (buf.mode == LH) {
      const uint32_t byte_low = mem[buf.vj + buf.imm],
                     byte_high = mem[buf.vj + buf.imm + 1];
      res = signed_extend((byte_high << 8) | byte_low, 15);
      rs.update(buf.serial, res);
      rob.submit(buf.serial, res);

    } else if (buf.mode == LHU) {
      const uint32_t byte_low = mem[buf.vj + buf.imm],
                     byte_high = mem[buf.vj + buf.imm + 1];
      res = (byte_high << 8) | byte_low;
      rob.submit(buf.serial, res);

    } else if (buf.mode == LW) {
      res = fetch(buf.vj + buf.imm);

      // if (buf.vj + buf.imm == 0x1394) {
      //   std::cout << "LOAD " << res << " FROM 0x1394\n";
      // }

      rob.submit(buf.serial, res);

    } else if (buf.mode == SB) {
      // std::cout << "SB: " << get_bits(buf.vk, 7, 0);
      mem[buf.vj + buf.imm] = uint8_t(get_bits(buf.vk, 7, 0));
      // std::cout << " -> " << uint32_t(mem[buf.vj + buf.imm]) << '\n';

    } else if (buf.mode == SH) {
      mem[buf.vj + buf.imm] = uint8_t(get_bits(buf.vk, 7, 0));
      mem[buf.vj + buf.imm + 1] = uint8_t(get_bits(buf.vk, 15, 8));

    } else if (buf.mode == SW) {
      // std::cout << "SW: " << get_bits(buf.vk, 31, 0);
      mem[buf.vj + buf.imm] = uint8_t(get_bits(buf.vk, 7, 0));
      mem[buf.vj + buf.imm + 1] = uint8_t(get_bits(buf.vk, 15, 8));
      mem[buf.vj + buf.imm + 2] = uint8_t(get_bits(buf.vk, 23, 16));
      mem[buf.vj + buf.imm + 3] = uint8_t(get_bits(buf.vk, 31, 24));

      // if (buf.vj + buf.imm == 0x1394) {
      //   std::cout << "STORE " << buf.vk << " TO 0x1394\n";
      // }
      //  std::cout << " -> " << uint32_t(mem[buf.vj + buf.imm]) << ' '
      //            << uint32_t(mem[buf.vj + buf.imm + 1]) << ' '
      //            << uint32_t(mem[buf.vj + buf.imm + 2]) << ' '
      //            << uint32_t(mem[buf.vj + buf.imm + 3]) << '\n';
    }

    buf.clear();
    buffer.pop();

  } else if (buf.state >= 0) { // in process
    ++buf.state;
  }
}

// void LSBuffer::update(int32_t src, int32_t res) {}

void LSBuffer::print() {
  std::cout << "LSB STATE:\n";

  for (int i = 0; i != 6; ++i) {
    auto &buf = buffer.arr[i];
    if (buf.mode == NONE) {
      std::cout << "NONE\n";
      continue;
    }

    std::cout << "MODE: " << buf.mode << " SER: " << buf.serial;

    if (buf.qj == 0 && buf.qk == 0) {
      std::cout << " STATE: ";
      if (buf.ready) {
        std::cout << int32_t(buf.state) << '\n';
      } else {
        std::cout << "NOT READY\n";
      }

    } else {
      std::cout << " DEP: ";
      if (buf.qj != 0) {
        std::cout << buf.qj << ' ';
      }
      if (buf.qk != 0) {
        std::cout << buf.qk;
      }
      std::cout << '\n';
    }
  }

  std::cout << std::endl;
}

void LSBuffer::print_first() {
  std::cout << "LSB FRONT STATE:\n";

  auto &buf = buffer.top();
  if (buf.mode == NONE) {
    std::cout << "NONE\n";
    return;
  }

  std::cout << "MODE: " << buf.mode << " SER: " << buf.serial;

  if (buf.qj == 0 && buf.qk == 0) {
    std::cout << " STATE: ";
    if (buf.ready) {
      std::cout << int32_t(buf.state) << '\n';
    } else {
      std::cout << "NOT READY\n";
    }

  } else {
    std::cout << " DEP: ";
    if (buf.qj != 0) {
      std::cout << buf.qj << ' ';
    }
    if (buf.qk != 0) {
      std::cout << buf.qk;
    }
    std::cout << '\n';
  }

  std::cout << std::endl;
}