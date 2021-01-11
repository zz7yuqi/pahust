#include "cpu/exec.h"

extern void raise_intr(uint32_t, vaddr_t);
int32_t readcsr(int i){
    switch(i){
        case 0x105:
            return cpu.csr[STVEC];
        case 0x142:
            return cpu.csr[SCAUSE];
        case 0x100:
            return cpu.csr[SSTATUS];
        case 0x141:
            //printf("%x\n", decinfo.isa.sepc);
            //printf("pc%x\n", decinfo.seq_pc-4);
            return cpu.csr[SEPC];
        default:
            assert(0 && "Unfinished readcsr");
    }
}

void writecsr(int i, int32_t val){
    switch(i){
        case 0x105:
            cpu.csr[STVEC] = val;
            break;
        case 0x142:
            cpu.csr[SCAUSE] = val;
            break;
        case 0x100:
            cpu.csr[SSTATUS] = val;
            break;
        case 0x141:
            cpu.csr[SEPC] = val;
            break;
        default:
            assert(0 && "Unfinished readcsr");
    }
}

make_EHelper(sys){
    Instr instr = decinfo.isa.instr;
    switch(instr.funct3){
        //ecall
        case 0b0:
            if((instr.val & ~(0x7f))==0){
                raise_intr(reg_l(17), decinfo.seq_pc-4);
            }
            else if(instr.val == 0x10200073){
                decinfo.jmp_pc = cpu.csr[SEPC] + 4;
                decinfo_set_jmp(true);
            }
            else{
                assert(0 && "system code unfinish");
            }
            break;
        // csrrw
        case 0b001:
            s0 = readcsr(instr.csr);
            writecsr(instr.csr, id_src->val);
            rtl_sr(id_dest->reg, &s0, 4);
            break;
        case 0b010:
            s0 = readcsr(instr.csr);
            writecsr(instr.csr, s0 | id_src->val);
            
            rtl_sr(id_dest->reg, &s0, 4);
            //printf("reg:%d\n", reg_l(id_dest->reg));
            break;
        default:
            assert(0 && "Unfinished system op");
    }
}