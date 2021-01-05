#include "cpu/exec.h"

/* imm-reg instr (type I) */
make_EHelper(addi);
make_EHelper(slti);
make_EHelper(sltiu);
make_EHelper(xori);
make_EHelper(ori);
make_EHelper(andi);
make_EHelper(slli);
make_EHelper(srliORsrai);
/* imm-reg instr (type U) */
make_EHelper(auipc);
make_EHelper(lui);
/* reg-reg instr (type R) */
make_EHelper(addORsub);
make_EHelper(sll);
make_EHelper(slt);
make_EHelper(sltu);
make_EHelper(xor);
make_EHelper(srlORsra);
make_EHelper(or);
make_EHelper(and);

/* conditional branches (type B) */
make_EHelper(conBranch);
/* unconditional jump (type I) */
make_EHelper(jalr);
/* unconditional jump (type J) */
make_EHelper(jal);


make_EHelper(ld);
make_EHelper(lh_lb);
make_EHelper(st);

make_EHelper(inv);
make_EHelper(nemu_trap);
