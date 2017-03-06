#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include "elf.h"
#include "symbol.h"



funcs_table funcs;
hmlist g_Module_list;
LPVOID find_in_modules(char* funcname)
{
	int i;
	LPVOID pf = NULL;
	char t_name_buf[103] = "___";
	if (!funcname)
		return NULL;
	if (strlen(funcname)>99)
		return NULL;
	strcat(t_name_buf, funcname);
	for (i = 0; i<g_Module_list.max; i++)
	{
		int j;
		bool found = false;
		for (j = 3; j>0; j--)
		{
			pf = GetProcAddress(g_Module_list.hms[i], t_name_buf + j);
			if (pf)
			{
				found = true;
				break;
			}
		}
		if (found == true)
			break;
	}
	return pf;
}
unsigned int alarm(unsigned int seconds)
{
	puts("alarm was not supported");
	exit(0);
}
unsigned int sleep(unsigned int seconds)
{
	Elf32_Addr t_pfunc = (Elf32_Addr)find_in_modules("sleep");
	((unsigned int(*)(unsigned int seconds)) t_pfunc)(seconds * 1000);
	return 0;
}


void init_funcs_table()
{
	g_Module_list.max = 0;
	if (g_Module_list.hms[g_Module_list.max] = LoadLibrary(L"msvcrt.dll"))
		g_Module_list.max++;
	//if (g_Module_list.hms[g_Module_list.max] = LoadLibrary(L"msvcrt90.dll"))
		//g_Module_list.max++;
	//printf("max:%d\n", g_Module_list.max);
	funcs.max = 0;
	strcpy(funcs.fitems[funcs.max].name, "alarm");
	funcs.fitems[funcs.max++].pf = (LPVOID*)alarm;
	strcpy(funcs.fitems[funcs.max].name, "sleep");
	funcs.fitems[funcs.max++].pf = (LPVOID*)sleep;



}



