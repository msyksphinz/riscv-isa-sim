// See LICENSE for license details.

#include "rocc.h"
#include "mmu.h"
#include <cstring>
#include <iostream>

class matrix16_rocc_t : public rocc_t
{
 public:
  const char* name() { return "matrix16_rocc"; }

  reg_t custom0 (rocc_insn_t insn, reg_t xs1, reg_t xs2)
  {
    reg_t total = 0;
    switch (insn.funct) {
      case 0: {
        m_length = xs1;
        break;
      }
      case 1: {
        m_v_step = xs1; break;
      }
      case 2: {
        reg_t xs1_p = xs1;
        reg_t xs2_p = xs2;
        for (reg_t i = 0; i < m_length; i++) {
          int16_t a_val = p->get_mmu()->load_int16(xs1_p); xs1_p += sizeof(int16_t);
          int16_t b_val = p->get_mmu()->load_int16(xs2_p); xs2_p += (m_v_step * sizeof(int16_t));
          total = total + ((a_val * b_val) >> 16);
        }
        break;
      }
    }
    return total;
  }

  matrix16_rocc_t()
  {
  }

 private:
  uint64_t m_length;
  uint64_t m_v_step;
};

REGISTER_EXTENSION(matrix16_rocc, []() { return new matrix16_rocc_t; })
