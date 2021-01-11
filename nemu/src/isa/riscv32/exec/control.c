#include "cpu/exec.h"

/* unconditional jumps (type J) */
make_EHelper(jal) { /* interpret_rtl_jal(vaddr_t *pc) */
   t0 = cpu.pc + 4;
   rtl_sr(id_dest->reg, &t0, 4);
   //rtl_add(&decinfo.jmp_pc, &id_src->val, &cpu.pc);
   interpret_rtl_j(decinfo.jmp_pc); 
   print_asm_template2(jal);
   printf("0x%x\n", cpu.pc);
    // s0 = decinfo.seq_pc;
    // s1 = 4;
    // rtl_sr(id_dest->reg, &s0, 4);
    // rtl_sub(&s0, &s0, &s1);
    // rtl_add(&(decinfo.jmp_pc), &s0, &id_src->val);
    // //printf("pc:%x\n", decinfo.jmp_pc);
    // decinfo_set_jmp(true);
    // print_asm_template2(jal);
}
/* unconditional jumps (type I) */
make_EHelper(jalr) {
    t0 = cpu.pc + 4;
    rtl_add(&decinfo.jmp_pc, &id_src->val, &id_src2->val);    
    rtl_sr(id_dest->reg, &t0, 4);
    interpret_rtl_j(decinfo.jmp_pc); /* interpret_rtl_jr(&decinfo.jmp_pc); */

    print_asm_template2(jalr);   
}

/* conditional branches (type B) */
static uint32_t br_table[8] = {
    RELOP_EQ, RELOP_NE, RELOP_FALSE, RELOP_FALSE, RELOP_LT, RELOP_GE, RELOP_LTU, RELOP_GEU
};
make_EHelper(conBranch) {
    rtl_jrelop(br_table[decinfo.isa.instr.funct3],
     &id_src->val, &id_src2->val, decinfo.jmp_pc);
    
    switch (br_table[decinfo.isa.instr.funct3]) {
        case RELOP_EQ: print_asm_template3(beq); break;
        case RELOP_NE: print_asm_template3(bne); break;
        case RELOP_LT: print_asm_template3(blt); break;
        case RELOP_GE: print_asm_template3(bge); break;
        case RELOP_LTU: print_asm_template3(bltu); break;
        case RELOP_GEU: print_asm_template3(bgeu); break;
        default: assert(0);
    }
}
