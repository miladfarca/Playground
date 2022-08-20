#include "../../include/live.h"
#include "../../include/assembler.h"
#include "../../include/idt.h"
#include "../../include/shell.h"
#include "../../include/vga.h"

void live_draw_screen(int number_emitted_bytes) {
  struct vga_coordinates coordinates = vga_get_coordinates();
  uint16_t vga_width = coordinates.vga_width;
  int16_t vga_height = coordinates.vga_height;
  int16_t half_screen = (vga_width / 2) - 1;

  vga_clear();

#define LIVE_DISPLAY_REGISTER(name, x, y)                                      \
  vga_set_cursor_at(x, y);                                                     \
  vga_print_s(#name ": ");                                                     \
  vga_print_i(LIVE_REGISTER_##name);
  LIVE_DISPLAY_REGISTER(EAX, 1, 1)
  LIVE_DISPLAY_REGISTER(ECX, half_screen, 1)
  LIVE_DISPLAY_REGISTER(EDX, 1, 3)
  LIVE_DISPLAY_REGISTER(EBX, half_screen, 3)
  LIVE_DISPLAY_REGISTER(ESP, 1, 5)
  LIVE_DISPLAY_REGISTER(EBP, half_screen, 5)
  LIVE_DISPLAY_REGISTER(ESI, 1, 7)
  LIVE_DISPLAY_REGISTER(EDI, half_screen, 7)
  LIVE_DISPLAY_REGISTER(SS, 1, 9)
  LIVE_DISPLAY_REGISTER(CS, half_screen, 9)
  LIVE_DISPLAY_REGISTER(DS, 1, 11)
  LIVE_DISPLAY_REGISTER(ES, half_screen, 11)
  LIVE_DISPLAY_REGISTER(FS, 1, 13)
  LIVE_DISPLAY_REGISTER(GS, half_screen, 13)
#undef LIVE_DISPLAY_REGISTER

  // show number of emitted bytes.
  vga_set_cursor_at(0, vga_height - 5);
  vga_print_s("Emitted ");
  vga_print_i(number_emitted_bytes);
  vga_print_ln(" bytes.");

  vga_set_cursor_at(0, vga_height - 3);
  vga_print_s("Enter an instruction and press ENTER, press ESC to exit:");

  vga_set_cursor_at(0, vga_height - 1);
  shell_init();
}

void live_exec() {
  char buffer[SHELL_MAX_INPUT_SIZE] = {0};
  shell_get_command(buffer);
  int number_emitted_bytes = assembler_exec(buffer);
  live_draw_screen(number_emitted_bytes);
}

void live_init() {
  vga_set_mode(VGA_MODE_SHELL_IN_PROGRAM);
  shell_set_return_callback(live_exec);
  live_draw_screen(0);
}
