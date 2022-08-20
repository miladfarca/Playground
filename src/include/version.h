#ifndef _VERSION_H
#define _VERSION_H

#include "vga.h"

#define VERSION_STRING "0.5-beta"
#define VERSION_PRINT_PLAYGROUND()                                             \
  vga_print_ln(                                                                \
      "             _____  _                                             _ "); \
  vga_print_ln("            |  __ \\| |                                      " \
               "     | |");                                                    \
  vga_print_ln(                                                                \
      "            | |__) | | __ _ _   _  __ _ _ __ ___  _   _ _ __   __| |"); \
  vga_print_ln("            |  ___/| |/ _` | | | |/ _` | '__/ _ \\| | | | '_ " \
               "\\ / _` |");                                                   \
  vga_print_ln(                                                                \
      "            | |    | | (_| | |_| | (_| | | | (_) | |_| | | | | (_| |"); \
  vga_print_ln("            |_|    |_|\\__,_|\\__, |\\__, |_|  \\___/ "        \
               "\\__,_|_| |_|\\__,_|");                                        \
  vga_print_ln(                                                                \
      "                             __/ | __/ |                            "); \
  vga_print_ln(                                                                \
      "                            |___/ |___/                             ");

#endif
