// See LICENSE for license details.

#include "rocc.h"
#include "mmu.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <iostream>

class matrix16_rocc_t : public rocc_t
{
 public:
  const char* name() { return "matrix16_rocc"; }

  reg_t custom0 (rocc_insn_t insn, reg_t xs1, reg_t xs2)
  {
    int32_t total = 0;
    switch (insn.funct) {
      case 0: {
        m_length = xs1;
		// std::cerr << "Set Length = " << m_length << '\n';
        break;
      }
      case 1: {
        m_v_step = xs1; 
		// std::cerr << "Set V-step = " << m_v_step << '\n';
		break;
      }
      case 2: {
        reg_t xs1_p = xs1;
        reg_t xs2_p = xs2;
        for (reg_t i = 0; i < m_length; i++) {
          int32_t a_val = p->get_mmu()->load_int32(xs1_p); xs1_p += sizeof(int32_t);
          int32_t b_val = p->get_mmu()->load_int32(xs2_p); xs2_p += (m_v_step * sizeof(int32_t));
		  // // std::cout << std::hex << std::setw(8) << std::setfill('0') << a_val << "*"
		  // // 			<< std::hex << std::setw(8) << std::setfill('0') << b_val << '+'
		  // // 			<< std::hex << std::setw(8) << std::setfill('0') << total << '=';
		  // int64_t product = static_cast<int64_t>(a_val) * b_val;
		  // int32_t adjust  = (product & 0x8000) >> 15;
          // total = total + (product >> 16) + adjust;
		  // // std::cout << std::hex << std::setw(8) << std::setfill('0') << total << "," 
		  // // 			<< std::hex << std::setw(16) << std::setfill('0') << product << "\n";
		  
		  int32_t  A = (a_val >> 16),    C = (b_val >> 16);
		  uint32_t B = (a_val & 0xFFFF), D = (b_val & 0xFFFF);
	
		  int32_t  AC    = A*C;
		  int32_t  AD_CB = A*D + C*B;
		  uint32_t BD    = B*D;
	
		  int32_t product_hi = AC + (AD_CB >> 16);
	
		  // Handle carry from lower 32 bits to upper part of result.
		  uint32_t ad_cb_temp = AD_CB << 16;
		  uint32_t product_lo = BD + ad_cb_temp;
		  if (product_lo < BD)
			product_hi++;

		  uint32_t product_lo_tmp = product_lo;
		  product_lo -= 0x8000;
		  product_lo -= (uint32_t)product_hi >> 31;
		  if (product_lo > product_lo_tmp)
			product_hi--;
	
		  // Discard the lowest 16 bits. Note that this is not exactly the same
		  // as dividing by 0x10000. For example if product = -1, result will
		  // also be -1 and not 0. This is compensated by adding +1 to the result
		  // and compensating this in turn in the rounding above.
		  int32_t result = (product_hi << 16) | (product_lo >> 16);
		  result += 1;

		  total = total + result;
        }
		// std::cout << "Calc: " << std::hex << total << '\n';
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
