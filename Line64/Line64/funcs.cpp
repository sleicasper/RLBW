#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
LPVOID alloc_u_r(int addr, int size)
{
	int i, j, n;
	LPBYTE p;
	if (p = (LPBYTE)VirtualAlloc((LPVOID *)(addr), size, MEM_TOP_DOWN | MEM_RESERVE, PAGE_READWRITE))
	{
	}
	else
	{
		puts("big alloc");
		printf("last error:%d\n", GetLastError());
	}
	return p;
}

LPVOID alloc_u_c(int addr, int size)
{
	LPBYTE pp;
	if (pp = (LPBYTE)VirtualAlloc((LPVOID*)addr, size, MEM_COMMIT, PAGE_READWRITE))
	{
		DWORD flag;
		if (!VirtualProtect((LPVOID *)(addr), size, PAGE_EXECUTE_READWRITE, &flag))
		{
			printf("addr: %p size: %x  ", addr, size);
			printf("last error:%d\n", GetLastError());
			puts("error c");
			exit(0);
		}
	}
	else
		;
	return pp;

}



bool write_mem(int addr, int filesz, int size, char *buf)
{
	char *p = (char*)addr;
	alloc_u_c(addr, size);
	int i;
	for (i = 0; i<filesz; i++)
		*(p + i) = buf[i];
	for (; i<filesz; i++)
		*(p + i) = 0;
	return true;
}


