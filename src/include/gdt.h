#ifndef _GDT_H
#define _GDT_H

void gdt_set();
void _gdt_set_gdtr();
void _gdt_reload_segments();

#endif
