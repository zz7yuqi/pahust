#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include "common.h"

#define PC_START (0x80000000u + IMAGE_START)

enum {
  STVEC = 0, SEPC, SSTATUS, SCAUSE
};

typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  rtlreg_t csr[4];

  vaddr_t pc;

} CPU_state;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 32);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)

static inline const char* reg_name(int index, int width) {
  extern const char* regsl[];
  assert(index >= 0 && index < 32);
  return regsl[index];
}

#endif
