#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include "elf.h"
#include "funcs.h"
#include "symbol.h"
#include "main.h"



Elf64_Ehdr g_ELF_header;
Elf64_Addr g_dym_addr;
Elf64_Addr g_got_addr;
Elf64_Addr g_rel_addr;
Elf64_Addr g_dsym_addr;
Elf64_Addr g_str_addr;
extern "C" Elf64_Addr __fastcall dl_resolv64(int off);
extern "C" __int64 hook_dl_resolv64();
extern "C" int hook_main(Elf64_Addr main_addr, int argc, char **argv);





Elf64_Addr __fastcall dl_resolv64(int off)
{
	int i_sym, i;
	Elf64_Addr got_tmp;
	Elf64_Addr t_pfunc;
	char *name;

	i_sym = ELF64_R_SYM((*(Elf64_Rela*)(off*sizeof(Elf64_Rela) + g_rel_addr)).r_info);
	name = (char*)((*(Elf64_Sym*)(g_dsym_addr + sizeof(Elf64_Sym) * i_sym)).st_name + g_str_addr);
	got_tmp = (*(Elf64_Rela*)(off*sizeof(Elf64_Rela) + g_rel_addr)).r_offset;
	/*
	First search in self defined function table
	*/
	bool found = false;
	for (i = 0; i < funcs.max; ++i)
	{
		if (!strcmp(name, funcs.fitems[i].name))
		{
			t_pfunc = (Elf64_Addr)funcs.fitems[i].pf;
			found = true;
			break;
		}
	}                                            

	 /*
	 If not found in self defined function table, then search in DLLs
	 */
	if (found == false)
	{
		t_pfunc = (Elf64_Addr)find_in_modules(name);
		if (!t_pfunc)
		{
			printf("%s not found \n", name);
			exit(0);
		}
	}

	/*
	In amd64 we have to prepare args for the call, so we don't place the function address into got,
	just leave the orginal addresses.
	(Elf64_Addr*)got_tmp = t_pfunc;
	*/

	return t_pfunc;


}


void dl_resolv_init64()
{
	Elf64_Shdr t_ELF_Shdr;
	Elf64_Dyn t_ELF_Dyn;
	int d_offset;
	int i;

	init_funcs_table();


	fseek(fp, 0, 0);
	fread(&g_ELF_header, sizeof(Elf64_Ehdr), 1u, fp);
	for (i = 0; i < g_ELF_header.e_shnum; ++i)
	{
		fseek(fp, g_ELF_header.e_shoff + sizeof(Elf64_Shdr) * i, 0);
		fread(&t_ELF_Shdr, sizeof(Elf64_Shdr), 1u, fp);
		if (t_ELF_Shdr.sh_type == SHT_DYNAMIC)		
		{
			g_dym_addr = t_ELF_Shdr.sh_addr;
			d_offset = t_ELF_Shdr.sh_offset;
		}
	}
	for (i = 0; ; ++i)
	{
		fseek(fp, d_offset + sizeof(Elf64_Dyn) * i, 0);
		fread(&t_ELF_Dyn, sizeof(Elf64_Dyn), 1u, fp);
		int sig = 0;
		switch (t_ELF_Dyn.d_tag)
		{
		case DT_JMPREL:	
			g_rel_addr = t_ELF_Dyn.d_un.d_ptr;
			break;
		case DT_PLTGOT:	
			g_got_addr = t_ELF_Dyn.d_un.d_ptr;
			break;
		case DT_SYMTAB:	
			g_dsym_addr = t_ELF_Dyn.d_un.d_ptr;
			break;
		case DT_STRTAB:	
			g_str_addr = t_ELF_Dyn.d_un.d_ptr;
			break;
		case  DT_NULL:
			sig = 1;
			break;
		}
		if (sig)
			break;
	}
}



void load64(int argc, char** argv)
{
	int i;
	Elf64_Phdr t_ELF_Phdr;
	Elf64_Addr max = 0, min = 0xffffffff;


	for (i = 0; i < g_ELF_header.e_phnum; ++i)
	{
		fseek(fp, g_ELF_header.e_phoff + sizeof(Elf64_Phdr) * i, 0);
		fread(&t_ELF_Phdr, sizeof(Elf64_Phdr), 1u, fp);
		if (t_ELF_Phdr.p_type == PT_LOAD)		
		{
			if (max < t_ELF_Phdr.p_vaddr + t_ELF_Phdr.p_memsz)
				max = t_ELF_Phdr.p_vaddr + t_ELF_Phdr.p_memsz;
			if (min > t_ELF_Phdr.p_vaddr)
				min = t_ELF_Phdr.p_vaddr;
		}
	}
	alloc_u_r(min, max - min);


	for (i = 0; i < g_ELF_header.e_phnum; ++i)
	{
		fseek(fp, g_ELF_header.e_phoff + sizeof(Elf64_Phdr) * i, 0);
		fread(&t_ELF_Phdr, sizeof(Elf64_Phdr), 1u, fp);
		if (t_ELF_Phdr.p_type == PT_LOAD)
		{
			fseek(fp, t_ELF_Phdr.p_offset, 0);
			fread(&buf, 1u, t_ELF_Phdr.p_filesz, fp);
			write_mem(t_ELF_Phdr.p_vaddr, t_ELF_Phdr.p_filesz, t_ELF_Phdr.p_memsz, buf);
		}
	}
	*(Elf64_Addr*)(g_got_addr + 2*sizeof(Elf64_Addr)) = (Elf64_Addr)hook_dl_resolv64;
	Elf64_Addr main_addr;
	main_addr = *(__u32 *)(g_ELF_header.e_entry + 0x20);
	//scanf("%d", &i);
	hook_main(main_addr, argc - 1, argv + 1);
//	((int(*)(int, char**))main_addr)(argc - 1, argv + 1);
}







