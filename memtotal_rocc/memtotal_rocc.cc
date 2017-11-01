// See LICENSE for license details.

#include "rocc.h"
#include "mmu.h"
#include <cstring>

class memtotal_rocc_t : public rocc_t
{
 public:
  const char* name() { return "memtotal_rocc"; }

  reg_t custom0(rocc_insn_t insn, reg_t xs1, reg_t xs2)
  {
    reg_t total = 0;
    switch (insn.funct)
    {
    case 0:
      c0_reg_lengthM = xs1;
      break;
    case 1:
      c0_reg_lengthK = xs1;
      break;
    case 2: {
      reg_t total_lo = 0, total_hi = 0;
      reg_t xs1_p = xs1, xs2_p = xs2;
      for (reg_t i = 0; i < c0_reg_lengthM; i++) {
        total_lo += p->get_mmu()->load_int32(xs1_p) * p->get_mmu()->load_int32(xs2_p);
        xs1_p += sizeof(int32_t);
        xs2_p += c0_reg_lengthK * sizeof(int32_t);
      }
      xs1_p = xs1;
      xs2_p = xs2 + sizeof(int32_t);
      for (reg_t i = 0; i < c0_reg_lengthM; i++) {
        total_hi += p->get_mmu()->load_int32(xs1_p) * p->get_mmu()->load_int32(xs2_p);
        xs1 += sizeof(int32_t);
        xs2 += c0_reg_lengthK * sizeof(int32_t);
      }
      total = ((total_hi & 0x0ffffffffUL) << 32) | (total_lo & 0x0ffffffffUL);
      break;
    }
    }
    return total;
  }

  memtotal_rocc_t()
  {
    c0_reg_lengthM = 0;
    c0_reg_lengthK = 0;
  }

 private:
  static const int num_acc = 4;
  reg_t c0_reg_lengthM;
  reg_t c0_reg_lengthK;
};

REGISTER_EXTENSION(memtotal_rocc, []() { return new memtotal_rocc_t; })
