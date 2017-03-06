#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include "elf.h"
#include "funcs.h"
#include "symbol.h"
#include "main.h"



Elf32_Ehdr g_ELF_header;
Elf32_Addr g_dym_addr;
Elf32_Addr g_got_addr;
Elf32_Addr g_rel_addr;
Elf32_Addr g_sym_addr;
Elf32_Addr g_str_addr;




void dl_resolv_init32()
{
	Elf32_Shdr t_ELF_Shdr;
	Elf32_Dyn t_ELF_Dyn;
	int d_offset;
	int i;

	init_funcs_table();


	fseek(fp, 0, 0);
	fread(&g_ELF_header, sizeof(Elf32_Ehdr), 1u, fp);
	for (i = 0; i < g_ELF_header.e_shnum; ++i)
	{
		fseek(fp, g_ELF_header.e_shoff + 40 * i, 0);
		fread(&t_ELF_Shdr, sizeof(Elf32_Shdr), 1u, fp);
		if (t_ELF_Shdr.sh_type == SHT_DYNAMIC)		
		{
			g_dym_addr = t_ELF_Shdr.sh_addr;
			d_offset = t_ELF_Shdr.sh_offset;
		}
	}
	for (i = 0; ; ++i)
	{
		fseek(fp, d_offset + sizeof(Elf32_Dyn) * i, 0);
		fread(&t_ELF_Dyn, sizeof(Elf32_Dyn), 1u, fp);
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
			g_sym_addr = t_ELF_Dyn.d_un.d_ptr;
			break;
		case DT_STRTAB:	
			g_str_addr = t_ELF_Dyn.d_un.d_ptr;
			break;
		case DT_NULL:
			sig = 1;
			break;
		}
		if (sig)
			break;
	}
}
void dl_resolv32(int off)
{
	int i_sym, i;
	Elf32_Addr got_tmp;
	Elf32_Addr t_pfunc;
	char *name;

	//get index
	i_sym = (*(Elf32_Rel*)(off + g_rel_addr)).r_info >> 8;
	got_tmp = (*(Elf32_Rel*)(off + g_rel_addr)).r_offset;
	name = (char*)((*(Elf32_Sym*)(g_sym_addr + 16 * i_sym)).st_name + g_str_addr);

	/*
	First search in self defined function table
	*/
	bool found = false;
	for (i = 0; i < funcs.max; ++i)
	{
		if (!strcmp(name, funcs.fitems[i].name))
		{
			t_pfunc = (Elf32_Addr)funcs.fitems[i].pf;
			found = true;
			break;
		}
	}                                             

	/*
	If not found in self defined function table, then search in DLLs
	*/
	if (found == false)
	{
		t_pfunc = (Elf32_Addr)find_in_modules(name);
		if (!t_pfunc)
		{
			printf("%s not found \n", name);
			exit(0);
		}
	}

	//change got table 
	*(Elf32_Addr*)got_tmp = t_pfunc;

	
	__asm {
		mov eax, t_pfunc;
		lea esp, [ebp + 0Ch];
		mov ebp, [ebp];		
		jmp eax;
	}



}
void load32(int argc, char** argv)
{
	int i;
	Elf32_Phdr t_ELF_Phdr;
	Elf32_Addr max = 0, min = 0xffffffff;

	for (i = 0; i < g_ELF_header.e_phnum; ++i)
	{
		fseek(fp, g_ELF_header.e_phoff + 32 * i, 0);
		fread(&t_ELF_Phdr, 0x20u, 1u, fp);
		if (t_ELF_Phdr.p_type == PT_LOAD )		
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
		fseek(fp, g_ELF_header.e_phoff + 32 * i, 0);
		fread(&t_ELF_Phdr, 0x20u, 1u, fp);
		if (t_ELF_Phdr.p_type == PT_LOAD )
		{
			fseek(fp, t_ELF_Phdr.p_offset, 0);
			fread(&buf, 1u, t_ELF_Phdr.p_filesz, fp);
			write_mem(t_ELF_Phdr.p_vaddr, t_ELF_Phdr.p_filesz, t_ELF_Phdr.p_memsz, buf);
		}
	}
	*(Elf32_Addr*)(g_got_addr + 8) = (Elf32_Addr)dl_resolv32;
	Elf32_Addr main_addr;
	main_addr = *(Elf32_Addr*)(g_ELF_header.e_entry+ 24);
	((int(*)(int, char**))main_addr)(argc - 1, argv + 1);
}







