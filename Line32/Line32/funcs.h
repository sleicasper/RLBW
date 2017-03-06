#pragma once

LPVOID alloc_u_r(int addr, int size);
LPVOID alloc_u_c(int addr, int size);
bool write_mem(int addr, int filesz, int size, char *buf);