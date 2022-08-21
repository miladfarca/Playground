#include "../include/builtins.h"
#include "../include/live.h"
#include "../include/system.h"
#include "../include/utils.h"
#include "../include/version.h"
#include "../include/vga.h"
#include <stddef.h>
#include <stdint.h>

static char *bultin_list[] = {
    "beep:   Turn Motherboard speakers on for one second."
    "live:   Live x86 assembler.",
    "party:  Write random characters to the VGA memory buffer.",
    "random: Print a random 32-bit signed integer.",
    "time:   Show CMOS time (Hour:Minute:Second)."};

void builtins_show_help() {
  vga_print_s("Playground version ");
  vga_print_s(VERSION_STRING);
  vga_print_ln(".");
  vga_print_ln("");
  int size = (sizeof(bultin_list) / sizeof(intptr_t));
  for (int i = 0; i < size; i++) {
    vga_print_ln(bultin_list[i]);
  }
}

void builtins_party() {
  struct vga_coordinates coordinates = vga_get_coordinates();
  uint16_t *vga_buffer = coordinates.vga_buffer;

  while (1) {
    for (int i = 0; i < coordinates.vga_width * coordinates.vga_height; i++) {
      int32_t r = rand();
      *(vga_buffer++) = (uint16_t)r;
    }
    vga_buffer = coordinates.vga_buffer;
    sleep(1);
  }
}

void builtins_live() { live_init(); }

void builtins_time() {
  struct system_time time = system_get_time();
  vga_print_i(time.hours);
  vga_print_s(":");
  vga_print_i(time.minutes);
  vga_print_s(":");
  vga_print_i(time.seconds);
  if (time.am_pm != NULL) {
    vga_print_s(" ");
    vga_print_s(time.am_pm);
  }
  vga_print_ln("");
}

void builtins_random() {
  vga_print_i(rand());
  vga_print_ln("");
}

void builtins_beep() {
  system_turn_speaker_on(1000);
  sleep(1);
  system_turn_speaker_off();
}
