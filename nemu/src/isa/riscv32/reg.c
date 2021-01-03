#include "nemu.h"

const char *regsl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  //printf("reg_display:\n");
  int index;
  for (int i = 0; i < 4; i ++) {
    for (int j = 0; j < 8; j ++) {
      index = i * 8 + j;
      printf("%4s: %08x ", regsl[index], reg_l(index));
    }
    printf("\n");
  }
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  //when s == "0", return $0.
  if (strcmp(s, "0") == 0) {
    *success = true;
    return reg_l(0);
  } 

  int index;
  bool find = false;
  for (index = 1; index < 32; index++) {
    if (strcmp(s, regsl[index]) == 0) {
      find = true;
      *success = true;
      return reg_l(index);
    }
  }

  if (!find) *success = false;
  return 0;
}
