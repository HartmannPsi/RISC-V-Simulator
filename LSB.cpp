#include "LSB.hpp"
#include "headers.hpp"
#include "main.hpp"
#include "utility.hpp"

bool LSBuffer::full() const { return buffer.full(); }

bool LSBuffer::empty() const { return buffer.empty(); }

bool LSBuffer::read(const Inst &inst) {
  if (buffer.full()) {
    return false;
  }

  LSInst tmp;

  tmp.serial = inst.serial;

  if (inst.opcode == 0b000'0011) { // ld insts

    tmp.imm = inst.imm;
    reg_depend[inst.rd] = tmp.serial;
    if (reg_depend[inst.rs1] == 0) { // no depend
      tmp.vj = reg[inst.rs1];
    } else {
      tmp.qj = reg_depend[inst.rs1];
      tmp.state = -1;
    }

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

  if (buf.state == 3) { // carry out & broadcast
    int32_t res = 0;

    if (buf.mode == LB) {
      res = signed_extend(uint8_t(mem[buf.vj + buf.imm]), 7);

    } else if (buf.mode == LBU) {
      res = uint8_t(mem[buf.vj + buf.imm]);

    } else if (buf.mode == LH) {
      const uint8_t byte_low = mem[buf.vj + buf.imm],
                    byte_high = mem[buf.vj + buf.imm + 1];
      res = signed_extend((byte_high << 8) | byte_low, 15);

    } else if (buf.mode == LHU) {
      const uint8_t byte_low = mem[buf.vj + buf.imm],
                    byte_high = mem[buf.vj + buf.imm + 1];
      res = (byte_high << 8) | byte_low;

    } else if (buf.mode == LW) {
      res = fetch(buf.vj + buf.imm);

    } else if (buf.mode == SB) {
      mem[buf.vj + buf.imm] = uint8_t(get_bits(res, 7, 0));

    } else if (buf.mode == SH) {
      mem[buf.vj + buf.imm] = uint8_t(get_bits(res, 7, 0));
      mem[buf.vj + buf.imm + 1] = uint8_t(get_bits(res, 15, 8));

    } else if (buf.mode == SW) {
      mem[buf.vj + buf.imm] = uint8_t(get_bits(res, 7, 0));
      mem[buf.vj + buf.imm + 1] = uint8_t(get_bits(res, 15, 8));
      mem[buf.vj + buf.imm + 2] = uint8_t(get_bits(res, 23, 16));
      mem[buf.vj + buf.imm + 3] = uint8_t(get_bits(res, 31, 24));
    }

    rob.submit(buf.serial, res);

  } else if (buf.state >= 0) { // in process
    ++buf.state;
  }
}