#ifndef _SYSTEM_H
#define _SYSTEM_H

struct system_time {
  char seconds;
  char minutes;
  char hours;
  char *am_pm;
};

struct system_time system_get_time();

#endif
