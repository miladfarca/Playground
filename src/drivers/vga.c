#include "../include/vga.h"
#include "../include/hal.h"
#include "../include/memory_layout.h"
#include "../include/utils.h"
#include <stddef.h>
#include <stdint.h>

static uint16_t *vga_buffer = (uint16_t *)MEMORY_VGA_TEXT_BUFFER;
static uint16_t vga_width = 80;
static uint16_t vga_height = 25;
static uint8_t vga_buffer_index_x = 0;
static uint8_t vga_buffer_index_y = 0;
static enum vga_mode mode = VGA_MODE_FREE;

static void update_cursor();
static void move_to_next_row();
static char get_char_at(uint8_t x, uint8_t y);
static void vga_print_char_at(uint8_t x, uint8_t y, char input);

static void update_cursor() {
  uint16_t pos = vga_buffer_index_y * vga_width + vga_buffer_index_x;
  hal_outb(0x3D4, 0x0F);
  hal_outb(0x3D5, (uint8_t)(pos & 0xFF));
  hal_outb(0x3D4, 0x0E);
  hal_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void move_to_next_row() {
  vga_buffer_index_x = 0;
  // check y boundaries
  if (vga_buffer_index_y >= vga_height - 1) {
    // scroll down one row
    uint16_t *start_point = vga_buffer + vga_width;
    uint64_t i;
    uint64_t count = (vga_width * vga_height) - vga_width;
    for (i = 0; i < count; i++) {
      vga_buffer[i] = start_point[i];
    }
    // clear the last row
    for (i = 0; i < vga_width; i++) {
      vga_print_char_at(i, vga_buffer_index_y, '\0');
    }
  } else {
    vga_buffer_index_y++;
  }
  update_cursor();
}

static char get_char_at(uint8_t x, uint8_t y) {
  // convert x,y to linear array position, x + (y * screen width)
  uint64_t linear_position = x + (y * vga_width);
  return vga_get_char_at_index(linear_position);
}

static void vga_print_char_at(uint8_t x, uint8_t y, char input) {
  uint16_t vga_input = (uint16_t)input;
  // convert x,y to linear array position, x + (y * screen width)
  uint64_t linear_position = x + (y * vga_width);
  // set bg color to black, text color to white
  vga_buffer[linear_position] = (0xf << 8) | vga_input;
}

void vga_set_mode(enum vga_mode input) { mode = input; }

void vga_set_cursor_at(uint8_t x, uint8_t y) {
  vga_buffer_index_x = x;
  vga_buffer_index_y = y;
  update_cursor();
}

enum vga_mode vga_get_mode() { return mode; }

struct vga_coordinates vga_get_coordinates() {
  return (struct vga_coordinates){vga_width, vga_height, vga_buffer_index_x,
                                  vga_buffer_index_y, vga_buffer};
}

char vga_get_char_at_index(uint64_t xy) {
  return (char)(vga_buffer[xy] & 0xff);
}

void vga_print_char(char input) {
  // check x boundaries
  if (vga_buffer_index_x > vga_width - 1) {
    // if screen is full scroll down, 1 row
    move_to_next_row();
  }
  vga_print_char_at(vga_buffer_index_x++, vga_buffer_index_y, input);
  update_cursor();
}

void vga_print_s(char *input) {
  uint64_t index = 0;
  while (input[index]) {
    vga_print_char(input[index++]);
  }
}

void vga_print_i(int input) {
  char c[100] = {0};
  utils_itoa(input, c, 10);
  vga_print_s(c);
}

void vga_print_ln(char *input) {
  vga_print_s(input);
  move_to_next_row();
}

void vga_print_back_space() {
  uint8_t limit =
      mode == VGA_MODE_SHELL || mode == VGA_MODE_SHELL_IN_PROGRAM ? 2 : 0;
  if (vga_buffer_index_x > limit) {
    --vga_buffer_index_x;
  } else if (vga_buffer_index_y > 0 && mode == VGA_MODE_FREE) {
    --vga_buffer_index_y;
    vga_buffer_index_x = vga_width - 1;
    while (vga_buffer_index_x > limit &&
           get_char_at(vga_buffer_index_x - 1, vga_buffer_index_y) == '\0') {
      --vga_buffer_index_x;
    }
  }
  vga_print_char_at(vga_buffer_index_x, vga_buffer_index_y, '\0');
  update_cursor();
}

void vga_clear() {
  uint64_t i;
  vga_buffer_index_x = 0;
  vga_buffer_index_y = 0;
  for (i = 0; i < vga_width * vga_height; i++) {
    vga_print_char('\0');
  }
  vga_buffer_index_x = 0;
  vga_buffer_index_y = 0;
}
