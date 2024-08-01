#include "RS.hpp"
#include "headers.hpp"
#include "main.hpp"

ReservationStation::ReservationStation() {
  buffer[0].name = ALURS1;
  buffer[1].name = ALURS2;
  buffer[2].name = ALURS3;
}

bool ReservationStation::full() const {
  for (int i = 0; i != 3; ++i) {
    if (!buffer[i].busy) {
      return false;
    }
  }
  return true;
}

bool ReservationStation::empty() const {
  for (int i = 0; i != 3; ++i) {
    if (buffer[i].busy) {
      return false;
    }
  }
  return true;
}

int ReservationStation::free() const {
  for (int i = 0; i != 3; ++i) {
    if (!buffer[i].busy) {
      return i;
    }
  }
  return -1;
}

void ReservationStation::clear() {
  for (int i = 0; i != 3; ++i) {
    buffer[i].clear();
  }
}

bool ReservationStation::read(const Inst &inst) {
  if (full()) {
    return false;
  }

  auto &buf = buffer[free()];
  buf.busy = true;
  buf.qj = buf.qk = 0;
  buf.vj = buf.vk = buf.imm = 0;
  buf.serial = inst.serial;

  if (inst.opcode == 0b011'0111) { // lui
    buf.imm = inst.imm;
    buf.vj = 0;
    reg_depend[inst.rd] = buf.serial;
    reg_depend[0] = 0;
    buf.mode = ADDI;

  } else if (inst.opcode == 0b001'0111) { // auipc
    buf.imm = inst.imm;
    buf.vj = 0;
    reg_depend[inst.rd] = buf.serial;
    reg_depend[0] = 0;
    buf.mode = ADDI;

  } else if (inst.opcode == 0b110'0011) { // branch's
    buf.imm = inst.imm;
    if (reg_depend[inst.rs1] == 0) { // no depend
      buf.vj = reg[inst.rs1];
    } else {
      buf.qj = reg_depend[inst.rs1];
    }
    if (reg_depend[inst.rs2] == 0) { // no depend
      buf.vk = reg[inst.rs2];
    } else {
      buf.qk = reg_depend[inst.rs2];
    }

    if (inst.subop1 == 0b000) { // beq
      buf.mode = BEQ;
    } else if (inst.subop1 == 0b001) { // bne
      buf.mode = BNE;
    } else if (inst.subop1 == 0b100) { // blt
      buf.mode = BLT;
    } else if (inst.subop1 == 0b101) { // bge
      buf.mode = BGE;
    } else if (inst.subop1 == 0b110) { // bltu
      buf.mode = BLTU;
    } else { // bgeu
      buf.mode = BGEU;
    }

  } else if (inst.opcode == 0b001'0011) { // imm operations
    buf.imm = inst.imm;
    if (reg_depend[inst.rs1] == 0) { // no depend
      buf.vj = reg[inst.rs1];
    } else {
      buf.qj = reg_depend[inst.rs1];
    }
    reg_depend[inst.rd] = buf.serial;
    reg_depend[0] = 0;

    if (inst.subop1 == 0b000) { // addi
      buf.mode = ADDI;
    } else if (inst.subop1 == 0b010) { // slti
      buf.mode = SLTI;
    } else if (inst.subop1 == 0b011) { // sltiu
      buf.mode = SLTIU;
    } else if (inst.subop1 == 0b100) { // xori
      buf.mode = XORI;
    } else if (inst.subop1 == 0b110) { // ori
      buf.mode = ORI;
    } else if (inst.subop1 == 0b111) { // andi
      buf.mode = ANDI;
    } else if (inst.subop1 == 0b001) { // slli
      buf.mode = SLLI;
    } else if (inst.subop2 == 0b0) { // srli
      buf.mode = SRLI;
    } else { // srai
      buf.mode = SRAI;
    }

  } else if (inst.opcode == 0b011'0011) { // reg operations
    if (reg_depend[inst.rs1] == 0) {      // no depend
      buf.vj = reg[inst.rs1];
    } else {
      buf.qj = reg_depend[inst.rs1];
    }
    if (reg_depend[inst.rs2] == 0) { // no depend
      buf.vk = reg[inst.rs2];
    } else {
      buf.qk = reg_depend[inst.rs2];
    }
    reg_depend[inst.rd] = buf.serial;
    reg_depend[0] = 0;

    if (inst.subop1 == 0b000) { // add / sub
      if (inst.subop2 == 0b0) { // add
        buf.mode = ADD;
      } else { // sub
        buf.mode = SUB;
      }

    } else if (inst.subop1 == 0b010) { // slt
      buf.mode = SLT;
    } else if (inst.subop1 == 0b011) { // sltu
      buf.mode = SLTU;
    } else if (inst.subop1 == 0b100) { // xor
      buf.mode = XOR;
    } else if (inst.subop1 == 0b101) { // srl / sra
      if (inst.subop2 == 0b0) {        // srl
        buf.mode = SRL;
      } else { // sra
        buf.mode = SRA;
      }

    } else if (inst.subop1 == 0b110) { // or
      buf.mode = OR;
    } else { // and
      buf.mode = AND;
    }

  } else if (inst.opcode == 0b110'1111) { // jal
    reg_depend[inst.rd] = buf.serial;
    reg_depend[0] = 0;
    buf.vj = 0;
    buf.imm = inst.imm;
    buf.mode = ADDI;

  } else if (inst.opcode == 0b110'0111) { // jalr
    buf.imm = inst.imm;
    if (reg_depend[inst.rs1] == 0) { // no depend
      buf.vj = reg[inst.rs1];
    } else {
      buf.qj = reg_depend[inst.rs1];
    }
    reg_depend[inst.rd] = buf.serial;
    reg_depend[0] = 0;

    buf.mode = JALR;

  } else {
    buf.serial = 0;
    buf.busy = false;
    return false;
  }

  return true;
}

void ReservationStation::execute() {

  for (int iter = 0; iter != 3; ++iter) {
    auto &buf = buffer[iter];
    const bool flag = (buf.qj != 0 || buf.qk != 0);

    if (buf.qj != 0 && cdb.src() == buf.qj) {
      buf.vj = cdb.val();
      buf.qj = 0;
    }
    if (buf.qk != 0 && cdb.src() == buf.qk) {
      buf.vk = cdb.val();
      buf.qk = 0;
    }
    if (flag || !buf.busy) {
      continue;
    }
    // do operations

    int32_t res = 0;
    bool branch = false;

    if (buf.mode == ADDI) {
      res = buf.imm + buf.vj;

    } else if (buf.mode == BEQ) {
      if (buf.vj == buf.vk) {
        branch = true;
        res = 1;
      }

    } else if (buf.mode == BNE) {
      if (buf.vj != buf.vk) {
        branch = true;
        res = 1;
      }

    } else if (buf.mode == BLT) {
      if (buf.vj < buf.vk) {
        branch = true;
        res = 1;
      }

    } else if (buf.mode == BGE) {
      if (buf.vj >= buf.vk) {
        branch = true;
        res = 1;
      }

    } else if (buf.mode == BLTU) {
      if (uint32_t(buf.vj) < uint32_t(buf.vk)) {
        branch = true;
        res = 1;
      }

    } else if (buf.mode == BGEU) {
      if (uint32_t(buf.vj) >= uint32_t(buf.vk)) {
        branch = true;
        res = 1;
      }

    } else if (buf.mode == SLTI) {
      res = ((buf.vj < buf.imm) ? 1 : 0);

    } else if (buf.mode == SLTIU) {
      res = ((uint32_t(buf.vj) < uint32_t(buf.imm)) ? 1 : 0);

    } else if (buf.mode == XORI) {
      res = buf.vj ^ buf.imm;

    } else if (buf.mode == ORI) {
      res = buf.vj | buf.imm;

    } else if (buf.mode == ANDI) {
      res = buf.vj & buf.imm;

    } else if (buf.mode == SLLI) {
      res = buf.vj << buf.imm;

    } else if (buf.mode == SRLI) {
      res = uint32_t(buf.vj) >> uint32_t(buf.imm);

    } else if (buf.mode == SRAI) {
      res = buf.vj >> buf.imm;

    } else if (buf.mode == ADD) {
      res = buf.vj + buf.vk;

    } else if (buf.mode == SUB) {
      res = buf.vj - buf.vk;

    } else if (buf.mode == SLL) {
      res = buf.vj << buf.vk;

    } else if (buf.mode == SLT) {
      res = ((buf.vj < buf.vk) ? 1 : 0);

    } else if (buf.mode == SLTU) {
      res = ((uint32_t(buf.vj) < uint32_t(buf.vk)) ? 1 : 0);

    } else if (buf.mode == XOR) {
      res = buf.vj ^ buf.vk;

    } else if (buf.mode == SRL) {
      res = uint32_t(buf.vj) >> uint32_t(buf.vk);

    } else if (buf.mode == SRA) {
      res = buf.vj >> buf.vk;

    } else if (buf.mode == OR) {
      res = buf.vj | buf.vk;

    } else if (buf.mode == AND) {
      res = buf.vj & buf.vk;
    } else { // jalr
      res = pc + 4;

      if (nxt_pc_src != 2) {
        nxt_pc = (buf.vj + buf.imm) & (~1);
        nxt_pc_src = 1;
      }
    }

    update(buf.serial, res);
    rob.submit(buf.serial, res);

    buf.busy = false;
  }
}

void ReservationStation::update(int32_t src, int32_t res) {
  for (int iter = 0; iter != 3; ++iter) {
    auto &buf = buffer[iter];
    if (!buf.busy) {
      continue;
    }
    if (buf.qj == src) {
      buf.vj = res;
      buf.qj = 0;
    }
    if (buf.qk == src) {
      buf.vk = res;
      buf.qk = 0;
    }
  }
}

void ReservationStation::print() {
  std::cout << "RS STATE:\n";
  for (int iter = 0; iter != 3; ++iter) {
    auto &buf = buffer[iter];

    if (!buf.busy) {
      std::cout << "NONE\n";
      continue;
    }

    std::cout << "MODE: " << buf.mode << " SER: " << buf.serial << "  DEP: ";

    if (buf.qj == 0 && buf.qk == 0) {
      std::cout << "NONE\n";

    } else {
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