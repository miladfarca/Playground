/*
  GNU nano 2.7.0 File: LICENSE

MIT License

Copyright (c) 2016 Michael Lazear

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "../../include/assembler.h"
#include "../../include/live.h"
#include "../../include/memory.h"
#include "../../include/memory_layout.h"
#include "../../include/utils.h"
#include "opcodes.h"
#include <stddef.h>
#include <stdint.h>

#define UCHAR_MAX 255

static uint32_t program_length = 0;
static uint32_t current_position;
static uint32_t num_symbols = 0;
static uint32_t unr_symbols = 0;
static uint8_t *output = (uint8_t *)MEMORY_LIVE_BUFFER_START;

static int find_symbol(char *label);

static void add_symbol(char *label) {
  // printf("%08x %s:\n", current_position, label);
  for (int i = 0; i < 32 && i < strlen(label); i++) {
    symtab[num_symbols].name[i] = label[i];
  }
  for (int i = 0; i < unr_symbols; i++) {
    if (strncmp(label, unresolved[i].name, strlen(unresolved[i].name)) == 0) {
      /* Added a previously unresolved symbol, so we need to go back and
      replace the dummy value with the real label position */
      int dummy = unresolved[i].position;
      for (int i = dummy; i < dummy + 4; i += 2) {
        uint16_t word = (output[i + 1] << 8 | output[i]);
        if (word == dummy) {
          output[i] = current_position & 0xFF;
          output[i + 1] = (current_position >> 8) & 0xFF;
        }
      }
    }
  }

  symtab[num_symbols++].position = current_position;
}

static int find_symbol(char *label) {
  for (int i = 0; i < num_symbols; i++)
    if (strcmp(symtab[i].name, label) == 0)
      return symtab[i].position;
  if (strncmp(label, "$$", 2) == 0)
    return 0;
  if (strcmp(label, "$") == 0)
    return current_position;

  unresolved[unr_symbols].position = current_position;

  for (int i = 0; i < 32 && i < strlen(label); i++)
    unresolved[unr_symbols].name[i] = label[i];
  unr_symbols++;
  return current_position;
}

/* functions to output in little-endian */
static void write_byte(uint32_t x) { output[current_position++] = x & 0xFF; }

static void write_word(uint32_t x) {
  output[current_position++] = x & 0xFF;
  output[current_position++] = (x >> 8) & 0xFF;
}

static void write_dword(uint32_t x) {
  output[current_position++] = x & 0xFF;
  output[current_position++] = (x >> 8) & 0xFF;
  output[current_position++] = (x >> 16) & 0xFF;
  output[current_position++] = (x >> 24) & 0xFF;
}

#ifndef __MATH__

static double log2(double s) {
  double i = 0;
  while (s /= 2)
    i++;
  return i;
}

#endif

static long long lass_atoi(char *s) {
  long long num = 0;
  int sign = 1;
  int base = 10;
  char *d = strpbrk(s, "x");
  if (d != NULL) {
    base = 16;
    s = d + 1;
  }

  if (s[0] == '-')
    sign = -1;

  for (int i = 0; i < strlen(s) && s[i] != '\0'; i++) {
    if (isdigit(s[i]))
      num = num * base + (s[i] - '0');
    else if (ishex(s[i]))
      num = num * base + (tolower(s[i]) - 'a') + 10;
  }
  return sign * num;
}

static isa *find_instruction(char *s, int op1, int op2) {
  for (int i = 0; i < sizeof(x86) / sizeof(isa); i++) {
    if (strcmp(x86[i].name, s) == 0) {
      if (op1 & sreg) {
        if ((x86[i].op1 == op1) &&
            ((x86[i].op2 == op2) || (x86[i].op2 == none)))
          return &x86[i];
      } else {
        if (((x86[i].op1 & op1) == x86[i].op1) &&
            ((x86[i].op2 == op2) || (x86[i].op2 == none)))
          return &x86[i];
        if ((x86[i].op1 == none) && (x86[i].op2 == none))
          return &x86[i];
      }
    }
  }
  /* Second loop - if we have an imm8 or rel8, but there is no designated
  imm8/rel8 function, use the 32 bit version. This simply saves some output
  space,
  and copies the format that NASM uses */
  for (int i = 0; i < sizeof(x86) / sizeof(isa); i++)
    if (strcmp(x86[i].name, s) == 0) {
      if (x86[i].primary == 0xe5)
        return &x86[i]; // TODO in/out workaround, change this hard-coded fix.
      else if (((x86[i].op1 & op1) == x86[i].op1) &&
               ((x86[i].op2 & ((op2 & (imm8 | rel8)) ? 0x6C0 : op2)) ||
                (x86[i].op2 == none)))
        return &x86[i];
    }
  return NULL;
}

/* classify() returns the size of operands and immediates */
static int classify(char *s) {
  if (isdigit(*s)) {
    long long num = lass_atoi(s);
    if (abs(num) > UCHAR_MAX)
      return imm32;
    else
      return imm8;
  }
  for (int i = 0; i < sizeof(registers) / sizeof(syntax); i++)
    if (strcmp(registers[i].name, s) == 0)
      return registers[i].code | r32;
  for (int i = 0; i < sizeof(half) / sizeof(syntax); i++)
    if (strcmp(half[i].name, s) == 0)
      return half[i].code | r8;
  for (int i = 0; i < sizeof(segments) / sizeof(syntax); i++)
    if (strcmp(segments[i].name, s) == 0) {
      return segments[i].code | sreg;
    }

  return -1;
}

typedef struct __instruction_s {
  int prefix;
  int operand;
  int modrm;
  int sib;
  int disp;
} instruction_s;

static instruction_s *calc_sib(char *line, int op1) {
  int scale = -1;
  int index = -1;
  int base = -1;
  char *plus = strchr(line, '+');
  char *minus = strchr(line, '-');
  char *star = strchr(line, '*');

  int c = 0;
  int disp = 0;
  int mod = 0;

  instruction_s *ret = memory_alloc(sizeof(instruction_s));
  memset(ret, 0, sizeof(instruction_s));

  /* We haven't been provided the first operand by parse_line.
  This should mean that the displacement IS the first operand:
  mov [eax+8], ebx 	etc, etc
  */
  if (op1 == 0) {
    char *op = strchr(line, ',');
    if (op != NULL) {
      // printf("%s\n", op);
      while (*op == ' ' || *op == ',')
        op++;
      op1 = classify(op);
      if (op1 & (imm8 | imm32))
        disp = lass_atoi(op);
    }
  }

  /* No multiplication symbol, so we assume that no SIB byte is required */
  if (star == NULL) {
    scale = 0;

    char *pch = strtok(line, " [+-],");
    while (pch) {
      if (isdigit(*pch)) {
        disp = lass_atoi(pch);
      } else {
        if (c == 0)
          index = classify(pch) & ~(rm32 | rm8);
        if (c == 1)
          base = classify(pch);
        c++;
      }
      pch = strtok(NULL, " [+-],*");
    }

    if (disp) {
      disp *= (minus) ? -1 : 1;
      if (disp > UCHAR_MAX)
        ret->modrm = MODRM(MOD_FOUR, (op1 & ~(r8 | r32)), index);
      else
        ret->modrm = MODRM(MOD_ONE, (op1 & ~(r8 | r32)), index);
      ret->disp = disp;
    } else {
      ret->modrm = MODRM((index == EBP) ? MOD_ONE : MOD_ZERO,
                         (op1 & ~(r8 | r32)), index);
      ret->disp = 0;
    }
    return ret;
  }

  /* We need to calculate an SIB byte
  MOD = 00b, 01b, 10b and R/M = 100b */
  char *pch = strtok(line, " [+],*");
  int n;
  while (pch) {
    if (isdigit(*pch)) {
      n = lass_atoi(pch);

      if ((plus < star) && (pch < plus)) {
        disp = n;
      }
      if ((plus < star) && (pch > plus)) {
        scale = n;
      }
    } else {
      if (c == 0)
        index = classify(pch);
      if (c == 1)
        base = classify(pch);
      c++;
    }

    pch = strtok(NULL, " [+],*");
  }

  /* R/M bits equals 4 always. Mod is determined by SIB addressing mode:
      00 = [reg32 + eax*n]
      01 = [disp + reg8 + eax*n]
      10 = [disp + reg32 + eax*n]
      00 = [disp + eax*n], base field = 101
  */
  if (index && base) {
    if (index & r32)
      mod = 2;
    else
      mod = 1;
    index &= ~(r8 | r32);
    base &= ~(r8 | r32);
  }
  if (base == -1) {
    base = 5;
    mod = 0;
  }

  scale = log2(scale);

  ret->modrm = MODRM(mod, op1, 4);
  ret->sib = MODRM(scale, index, base);
  ret->disp = disp;
  return ret;
}

static void parse_line(char *line, int pass_no) {
  // if (pass_no > 1)
  // printf("%08x %-20s\n", current_position, line);
  char *displacement = NULL;
  char *disp = strpbrk(line, "[");

  if (disp)
    displacement = strdup(disp);

  // split line by spaces and commas
  char *pch = strtok(line, " ,\t");

  // store instruction
  char *inst;

  uint64_t syn[2] = {0, 0};

  // number of items in line
  int count = 0;

  // store value of immediates
  int immediate = 0;
  int imm = 0;

  // default to MOD 11b
  int mode = MOD_REG;
  int m = -1;
  int rev = 0;
  int sib = 0;

  if (*pch == 'd') {
    char next = *(pch + 1);
    pch = strtok(NULL, ", '\"");
    while (pch) {
      switch (next) {
      case 'b': {
        if (isdigit(*pch)) {
          write_byte(lass_atoi(pch));
        } else {
          while (*pch)
            write_byte(*pch++);
        }
        break;
      }
      case 'w':
        write_word(lass_atoi(pch));
        break;
      case 'd':
        write_dword(lass_atoi(pch));
        break;
      default:
        // printf("Unrecognized symbol d%c\n", next);
        return;
      }
      pch = strtok(NULL, ", '\"");
    }
    return;
  }

  while (pch && *pch != ';') {
    if (pch == disp && disp) {
      // Do something
      instruction_s *a = calc_sib(displacement, syn[0]);
      m = a->modrm;
      sib = a->sib;
      immediate = a->disp;
      mode = (m >> 6);

      imm = 1;

      syn[1] |= (r32);
      if (syn[0] == 0) {
        syn[0] = ((m >> 3) & 0x7) | r32;
        rev = 1;
      }
      break;
    }

    if (count == 0) {
      inst = strdup(pch);
    } else {
      int n = classify(pch);

      if (n & (imm8 | imm32)) {
        imm = 1;
        immediate = lass_atoi(pch);
      }
      // label maybe?
      if (n == -1) {

        immediate = find_symbol(pch);
        imm = 1;
        n = rel8;
      }
      syn[count - 1] = n;
    }
    count++;
    // get next token
    pch = strtok(NULL, " ,\t");
  }

  isa *instruction = find_instruction(inst, syn[0], syn[1]);

  int start = current_position;

  if (instruction) {
    int o = instruction->primary;

    // Get any prefixes taken care of
    if (instruction->secondary)
      write_byte(instruction->secondary);
    if (instruction->prefix)
      write_byte(0x0F);

    // Two operands
    if (syn[1]) {

      if ((syn[0] & (r32 | r8 | sreg)) && (syn[1] & (r32 | r8 | sreg)))
        m = (m == -1) ? MODRM(mode, syn[0] &= ~(r32 | r8 | sreg),
                              syn[1] &= ~(r32 | r8 | sreg))
                      : m;
      else if ((syn[0] & (r32 | r8)) &&
               (syn[1] & (imm8 | imm32 | rel8 | rel32)))
        m = (m == -1)
                ? MODRM(mode, instruction->extension, syn[0] &= ~(r32 | r8))
                : m;

      if ((syn[0] & (imm8 | imm32 | rel8 | rel32)))
        m = MODRM(mode, instruction->extension, 0);

      // switch d flag
      if (rev)
        o ^= 0x2;

      // Macro value for alternate encodings
      if (o == 0xe7 ||
          o == 0xe5) // TODO in/out workaround, change this hard-coded fix.
        write_byte(o);
      else if (instruction->extension == 0xDEAD)
        write_byte(((o & ~0x7) | (syn[0] & ~(r32 | r8))));
      else
        write_word(m << 8 | o);

    } else {
      // One operand?
      if (instruction->extension == 0xDEAD && m == -1) {
        write_byte(((o & ~0x7) | (syn[0] & ~(r32 | r8 | sreg))));
      } else {
        if (!syn[0])
          write_byte(o);
        // Immediate, relative, none or ss/fs/gs/etc
        else if (syn[0] & 0x6C0 || (syn[0] & sreg))
          write_byte(o);
        else if (syn[0] != none) {
          m = (m == -1) ? MODRM(mode, instruction->extension,
                                syn[0] & ~(r32 | r8 | sreg))
                        : m;
          write_word(m << 8 | o);
        }
      }
    }

    /* If there's an SIB byte, write it now */
    if (sib) {
      write_byte(sib);
    }

    if (imm) {
      if (o == 0xe7 ||
          o == 0xe5) { // TODO in/out workaround, change this hard-coded fix.
        write_byte(immediate & 0xFF);
      } else {
        int sw =
            (instruction->op2 == none) ? instruction->op1 : instruction->op2;
        switch (sw) {
        case imm8:
          write_byte(immediate & 0xFF);
          break;
        case imm32:
          write_dword(immediate);
          break;
        case rel8:
          // Calculate relative offset
          write_byte(immediate - current_position - (current_position - start));
          break;
        case rel32:
          write_dword(immediate - current_position -
                      (current_position - start));
          break;
        default:
          // SIB/Displacement
          if (mode == 1)
            write_byte(immediate);
          else
            write_dword(immediate);
        }
      }
    }
  } else {
    // printf("instruction: %s not recognized?\n", inst);
  }
}

static void pass(char *buffer, int sz, int pass_no) {
  char **lines = memory_alloc(sz);

  *lines = buffer;
  void *start = lines;

  for (int i = 0; i < sz; i++) {
    if (buffer[i] == '\n') {
      buffer[i] = '\0';
      char *new = buffer + i + 1;
      if (*new)
        *(lines++) = new;
    }
  }

  for (lines = start; *lines; lines++) {
    if (**lines != ';' && **lines) { // Remove comment-only lines
      if (strchr(*lines, ':')) {
        char *c = strchr(*lines, ':');
        *c = '\0'; // remove the semi colon
        add_symbol(*lines);
      } else
        parse_line(*lines, pass_no);
    }
  }

  // free(start);
}

int assembler_exec(char *input) {

  current_position = 0;
  int sz = strlen(input);

  // We only need 1 pass.
  pass(input, sz, 1);

  output[current_position] = 0xc3;

  // Restore all register values from last run.
  LIVE_RESTORABLE_REGISTERS(LIVE_RESTORE_REGISTERS)

  // Don't use a function pointer and run a `call` manually to make sure
  // compiler doesn't overwrite any registers.
  asm("call " STRINGER(MEMORY_LIVE_BUFFER_START));

  // Save current register values.
  LIVE_REGISTERS(LIVE_SAVE_REGISTERS)

  return current_position; // return number of bytes emitted.
}
