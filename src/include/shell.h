#ifndef _SHELL_H
#define _SHELL_H

#define SHELL_MAX_INPUT_SIZE 500

void shell_set_return_callback(void (*callback)(void));
void shell_init();
void shell_reset();
void shell_restart();
void shell_get_command(char *buffer);
void shell_parse_command();

#endif
