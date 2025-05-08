#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include"./types.h"

registers_t regs = {};
uint8_t rom[ROM_SIZE] = {};
flags_t flags = {};

void clear_flags(uint8_t *reg) {
  *reg = 0;
}

void set_flag(uint8_t *reg, uint8_t bit, bool sr) {
  if(sr){
    *reg |= (sr << bit);
  }else{
    *reg &= ~(sr << bit);
  }
}

/** Accepts address <A1> and address <A2> such as in add <A1>, <A2> 
 *  Returns bitmask for the flags
 * */
uint8_t add_uint8(uint8_t *where, uint8_t *what, uint8_t *reg_pair) {
  uint8_t mask = 0;
  uint16_t result = (uint16_t) *where + (uint16_t) *what;
  set_flag(&mask, sf_half_carry, ((*where & 0x0F) + (*what & 0x0F)) > 0x0F);
  if(result > 0xFF) {
    set_flag(&mask, sf_carry, 1);
  }

  bool signA = *where   & 0x80;
  bool signN = *what    & 0x80;
  bool signR = result   & 0x80;
  bool overflow = (signA == signN) && (signR != signA);
  set_flag(&mask, sf_parity, overflow);
  set_flag(&mask, sf_sign, (signR) != 0);
  set_flag(&mask, sf_zero, ((uint8_t) result ) == 0);
  printf("[DEBUG] 16bit result: %d 8bit result: %d\n\n", result, (uint8_t) result);
  printf("[DEBUG] Carry: %d\n", (mask >> sf_carry ) & 1);
  printf("[DEBUG] Add/Sub: %d\n", (mask >> sf_add_sub ) & 1);
  printf("[DEBUG] Parity: %d\n", (mask >> sf_parity ) & 1);
  printf("[DEBUG] Half-C: %d\n", (mask >> sf_half_carry ) & 1);
  printf("[DEBUG] Zero: %d\n", (mask >> sf_zero ) & 1);
  printf("[DEBUG] Sign: %d\n", (mask >> sf_sign ) & 1);

  *reg_pair = mask;
  *where = (uint8_t) result;
  return mask;
}



//TODO make macro
void SWAP(uint8_t *a, uint8_t *b) {
  uint8_t temp =  *a;
  *a = *b;
  *b = temp;
}

void state_dump() {
  printf("   A       F       B       C       D       E   \n");
  printf("0x%04X  0x%04X  0x%04X  0x%04X  0x%04X  0x%04X \n",
      regs.A, regs.F, regs.B, regs.C, regs.D, regs.E
      );
  printf("   A'      F'      B'      C'      D'      E'   \n");
  printf("0x%04X  0x%04X  0x%04X  0x%04X  0x%04X  0x%04X  \n",
      regs.AA, regs.FA, regs.BA, regs.CA, regs.DA, regs.EA
      );
  printf(" PC - 0x%04X\n", regs.PC);
}

void rom_dump() {
  int index = 0;
  while(index < ROM_SIZE){
    printf("0x%02X\n", rom[index]);
    index ++;
  }
}

int main(int argc, char** argv) {

  if(argc < 2){
    return 1;
  }

  FILE *fptr;

  printf("File: %s\n", argv[1]);
  fptr = fopen(argv[1], "r");

  if(fptr == NULL){
    return 1;
  }

  int curr = 0;
  while (curr < ROM_SIZE){
    int c = fgetc(fptr);
    if(c == EOF){
      break;
    }

    rom[curr++] = (uint8_t) c;
  }

  rom_dump();

  while(!flags.halt && regs.PC < ROM_SIZE){
    int op = rom[regs.PC++];

    switch(op) {
      case op_inc_a: regs.A ++; break;
      case op_jp_nn: regs.PC = rom[regs.PC++] | (rom[regs.PC++] << 8); break;
      case op_ld_a_n: regs.A = rom[regs.PC++]; break;
      case op_ld_b_n: regs.B = rom[regs.PC++]; break;
      case op_ld_c_n: regs.C = rom[regs.PC++]; break;
      case op_ld_d_n: regs.D = rom[regs.PC++]; break;
      case op_ld_e_n: regs.E = rom[regs.PC++]; break;
      case op_ld_h_n: regs.H = rom[regs.PC++]; break;
      case op_ld_l_n: regs.L = rom[regs.PC++]; break;
      case op_halt: flags.halt = true; break;
      case op_ex_af: 
        SWAP(&regs.A, &regs.AA);
        SWAP(&regs.F, &regs.FA);
      break;
      case op_exx: 
       SWAP(&regs.B, &regs.BA);
       SWAP(&regs.C, &regs.CA);
       SWAP(&regs.D, &regs.DA);
       SWAP(&regs.E, &regs.EA);
       SWAP(&regs.H, &regs.HA);
       SWAP(&regs.L, &regs.LA);
      break;
      case op_add_a_n: 
        add_uint8(&regs.A, &rom[regs.PC++], &regs.F);
      break;
      case op_sub_b: printf("TODO\n");break;
      case op_nop: break;
    }
    state_dump();
    while(getchar() != '\n');
  }

  fclose(fptr);

  return 0;
}
