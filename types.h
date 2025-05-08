#include<stdbool.h>

#define ROM_SIZE 256

typedef struct flags {
  bool halt;
} flags_t;

typedef struct registers {
  uint8_t A;
  uint8_t AA;
  uint8_t F;
  uint8_t FA;

  uint8_t B;
  uint8_t BA;
  uint8_t C;
  uint8_t CA;

  uint8_t D;
  uint8_t DA;
  uint8_t E;
  uint8_t EA;

  uint8_t H;
  uint8_t HA;
  uint8_t L;
  uint8_t LA;

  uint16_t IX;
  uint16_t IY;
  uint16_t SP;
  uint16_t PC;
} registers_t;

enum status_flags {
  sf_carry = 0,
  sf_add_sub = 1,
  sf_parity = 2,
  sf_half_carry = 4,
  sf_zero = 6,
  sf_sign = 7,
};

enum op_codes {
  op_ld_a_n  = 0x3E,
  op_ld_b_n  = 0x06,
  op_ld_d_n  = 0x16,
  op_add_a_n = 0xC6,
  op_sub_b   = 0x90,
  op_ld_c_n  = 0x0E,
  op_ld_e_n  = 0x1E,
  op_ld_h_n  = 0x26,
  op_ld_l_n  = 0x2E, 
  op_ex_af   = 0x08,
  op_exx     = 0xD9,
  op_inc_a   = 0x3C,
  op_jp_nn   = 0xC3,
  op_halt    = 0x76,
  op_nop     = 0x00,
};
