#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include "main.h"
#include "elfloader64.h"

char buf[0xfffff];
FILE *fp;
int main(int argc, char** argv)
{
	char magic[5];
	//puts("VERSION 0.79!");

	if (argc == 1)
	{
		puts("<usage>: filename");
		exit(0);
	}
	if ((fp = fopen(argv[1], "rb")) == NULL)
	{
		puts(argv[1]);
		puts("fopen error");
		exit(0);
	}
	fread(magic, 0x5u, 1u, fp);
	if (magic[4] == '\x01')
	{
		puts("only support ELF64");
	}
	else
	{
		dl_resolv_init64();
		load64(argc, argv);
	}
	//puts("\n------------------------------\nRun End !");
	fclose(fp);
	return 0;
}