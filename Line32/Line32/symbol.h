#pragma once



typedef struct {
	char name[20];
	LPVOID *pf;
}fitem;
typedef struct {
	int max;
	fitem fitems[40];
}funcs_table;


typedef struct {
	int max;
	HMODULE hms[20];
}hmlist;


extern funcs_table funcs;
extern hmlist g_Module_list;

void init_funcs_table();
LPVOID find_in_modules(char* funcname);
