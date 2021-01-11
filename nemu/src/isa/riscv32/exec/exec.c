#include "cpu/exec.h"
#include "all-instr.h"

static OpcodeEntry load_table[8] = {
  EXW(lh_lb, 1), EXW(lh_lb, 2), EXW(ld, 4), EMPTY, EXW(ld, 1), EXW(ld, 2), EMPTY, EMPTY
};
static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry store_table[8] = {
  EXW(st, 1), EXW(st, 2), EXW(st, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry regImm_table[8] = {
  EX(addi), EX(slli), EX(slti), EX(sltiu), EX(xori), EX(srliORsrai), EX(ori), EX(andi)
};

static make_EHelper(regImm) {
  decinfo.width = regImm_table[decinfo.isa.instr.funct3].width;
  idex(pc, &regImm_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry regReg_table[8] = {
  EX(addORsub), EX(sll), EX(slt), EX(sltu), EX(xor), EX(srlORsra), EX(or), EX(and)
};

static make_EHelper(regReg) {
  decinfo.width = regReg_table[decinfo.isa.instr.funct3].width;
  idex(pc, &regReg_table[decinfo.isa.instr.funct3]);
}


static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, IDEX(I, regImm), IDEX(U, auipc), EMPTY, EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, IDEX(R, regReg), IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ IDEX(B, conBranch), IDEX(I, jalr), EX(nemu_trap), IDEX(I, jal), IDEX(SYS, sys), EMPTY, EMPTY, EMPTY,
};

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}
