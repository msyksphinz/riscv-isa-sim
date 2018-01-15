// See LICENSE for license details.

#include "rocc.h"
#include "mmu.h"
#include <cstring>
#include <iostream>

class memtotal_rocc_t : public rocc_t
{
 public:
  const char* name() { return "memtotal_rocc"; }

  reg_t custom0 (rocc_insn_t insn, reg_t xs1, reg_t xs2)
  {
    reg_t total = 0;
    switch (insn.funct) {
      case 0: {
        reg_t xs1_p = xs1;
        for (reg_t i = 0; i < xs2; i++) {
          total += p->get_mmu()->load_int64(xs1_p);
          xs1_p += sizeof(int64_t);
        }
        break;
      }
    }
    return total;
  }

  memtotal_rocc_t()
  {
  }

 private:
};

REGISTER_EXTENSION(memtotal_rocc, []() { return new memtotal_rocc_t; })
