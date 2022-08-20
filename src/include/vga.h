#ifndef _VGA_H
#define _VGA_H

#include <stdint.h>

enum vga_mode {
  VGA_MODE_FREE,            // free roaming on page.
  VGA_MODE_SHELL,           // shell command input.
  VGA_MODE_SHELL_IN_PROGRAM // VGA_MODE_SHELL, but do not auto start shell, let
                            // the new program do it.
};

struct vga_coordinates {
  uint16_t vga_width;
  uint16_t vga_height;
  uint16_t current_x;
  uint16_t current_y;
  uint16_t *vga_buffer;
};

void vga_set_mode(enum vga_mode input);
void vga_set_cursor_at(uint8_t x, uint8_t y);
enum vga_mode vga_get_mode();
struct vga_coordinates vga_get_coordinates();
char vga_get_char_at_index(uint64_t xy);
void vga_print_char(char input);
void vga_print_s(char *input);
void vga_print_i(int input);
void vga_print_ln(char *input);
void vga_print_back_space();
void vga_clear();

#endif
