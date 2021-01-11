#include "rtl/rtl.h"
#include "isa/reg.h"
#include "cpu/exec.h"

void raise_intr(uint32_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //decinfo.isa.sepc = epc;
  cpu.csr[SEPC] = epc;
  //printf("epc%x\n", epc);
  //decinfo.isa.scause = NO;
  cpu.csr[SCAUSE] = NO;
  //decinfo.jmp_pc = decinfo.isa.stvec;
  decinfo.jmp_pc = cpu.csr[STVEC];
  decinfo_set_jmp(true);
}

bool isa_query_intr(void) {
  return false;
}
