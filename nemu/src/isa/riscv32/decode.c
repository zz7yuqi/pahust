#include "cpu/decode.h"
#include "rtl/rtl.h"

// decode operand helper
#define make_DopHelper(name) void concat(decode_op_, name) (Operand *op, uint32_t val, bool load_val)

static inline make_DopHelper(i) {
  op->type = OP_TYPE_IMM;
  op->imm = val;
  rtl_li(&op->val, op->imm);

  print_Dop(op->str, OP_STR_SIZE, "%d", op->imm);
}

static inline make_DopHelper(r) {
  op->type = OP_TYPE_REG;
  op->reg = val;
  if (load_val) {
    rtl_lr(&op->val, op->reg, 4);
  }

  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg, 4));
}

/* type R:
 * dest: rd
 * src1: rs1
 * src2: rs2
 */
make_DHelper(R) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}
/* type I:
 * dest: rd
 * src1: rs1
 * src2: imm[11:0]
 */


make_DHelper(I) {
  uint32_t tempImm = decinfo.isa.instr.simm11_0;
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, (int32_t)(tempImm << 20) >> 20, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x",  decinfo.isa.instr.rs1);
  print_Dop(id_src2->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.simm11_0);
}

/* type U:
 * src: imm[31:12] << 12
 * dest: rd(instr[11:7])
 */
make_DHelper(U) {
  decode_op_i(id_src, decinfo.isa.instr.imm31_12 << 12, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.imm31_12);
}

/* type B:
 * src: rs1
 * src2: rs2
 * if $rs1 ==(!=) $rs2 , BRANCH 
 */
make_DHelper(B) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);

  uint32_t tempImm = (decinfo.isa.instr.simm12 << 12) 
                    | (decinfo.isa.instr.imm11 << 11)
                    | (decinfo.isa.instr.imm10_5 << 5)
                    | (decinfo.isa.instr.imm4_1 << 1);
  int32_t tempImmSExt = (int32_t)(tempImm << 19) >> 19;
  rtl_add(&decinfo.jmp_pc, &tempImmSExt, &cpu.pc);
  
  print_Dop(id_dest->str, OP_STR_SIZE, "0x%x", t0);
}

/* type J:
 * dest-addr: imm+pc
 */
make_DHelper(J) {
  uint32_t tempImm =  (decinfo.isa.instr.simm20 << 20) 
        | (decinfo.isa.instr.imm19_12 << 12) 
        | (decinfo.isa.instr.imm11_ << 11) 
        | (decinfo.isa.instr.imm10_1 << 1);   
  int32_t tempSExtImm = (int32_t)(tempImm << 11) >> 11;

  decode_op_i(id_src , tempSExtImm, true);
  rtl_add(&decinfo.jmp_pc, &tempSExtImm, &cpu.pc);

  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
  
  
  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", t0);
}

make_DHelper(ld) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, decinfo.isa.instr.simm11_0, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

/* type S:
 * src: rs1+imm
 * dest: rs2
 * mov rs2->val to *(rs1+imm) 
 */
make_DHelper(st) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  int32_t simm = (decinfo.isa.instr.simm11_5 << 5) | decinfo.isa.instr.imm4_0;
  decode_op_i(id_src2, simm, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rs2, true);
}

