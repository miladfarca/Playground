#include "../include/shell.h"
#include "../include/builtins.h"
#include "../include/utils.h"
#include "../include/vga.h"
#include <stddef.h>
#include <stdint.h>

static uint64_t last_cursor_xy = 0;
static void (*return_callback)(void) = NULL;

void shell_set_return_callback(void (*callback)(void)) {
  return_callback = callback;
}

void shell_init() {
  vga_print_s("# ");
  // save the current cursor linear position.
  struct vga_coordinates coordinates = vga_get_coordinates();
  last_cursor_xy =
      coordinates.current_x + (coordinates.current_y * coordinates.vga_width);
}

void shell_reset() {
  return_callback = NULL;
  vga_set_mode(VGA_MODE_SHELL);
}

void shell_restart() {
  shell_reset();
  vga_clear();
  vga_set_cursor_at(0, 0);
  shell_init();
}

void shell_get_command(char *buffer) {
  uint16_t buffer_index = 0;
  while (vga_get_char_at_index(last_cursor_xy) != '\0') {
    buffer[buffer_index++] = vga_get_char_at_index(last_cursor_xy++);
  }
}

void shell_parse_command() {

  if (return_callback) {
    (*return_callback)();
  } else {
    char buffer[SHELL_MAX_INPUT_SIZE] = {0};
    shell_get_command(buffer);

    vga_print_ln("");

    if (strcmp(buffer, "help") == 0) {
      builtins_show_help();
    } else if (strcmp(buffer, "party") == 0) {
      builtins_party();
    } else if (strcmp(buffer, "live") == 0) {
      builtins_live();
    } else if (strcmp(buffer, "time") == 0) {
      builtins_time();
    } else if (strcmp(buffer, "random") == 0) {
      builtins_random();
    } else if (strcmp(buffer, "beep") == 0) {
      builtins_beep();
    } else {
      vga_print_s("Unknown command!");
      vga_print_ln("");
    }

    if (vga_get_mode() != VGA_MODE_SHELL_IN_PROGRAM)
      shell_init();
  }
}
