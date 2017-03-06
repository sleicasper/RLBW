========================================================================
    Line64 
========================================================================

Line64 is designed for running ELF executable in Windows environment.
Unlike the previous Edition of Line which is Line32, Line64 is running amd64 opcode and most of the function call is fastcall convention, so Line64 can't use ELF lazy binding. It must keep got orignal.
And there are some problem for loading default ELF64 executable's address which is 0x400000 in windows, so if you want to use this ELFloader, you must set the base address of ELF executable up to 0x800000 or some other address that won't cause windows VirtualAlloc fail. When you compile the ELF executables, you must set the -fno-stack-protector flag, because the FS segment register in windows different from it in linux.
A sample makefile looks like below.
Line64 still can not load FULL RELRO executables now.
Line64 can not load statically linked executable since linux syscalls are different from windows syscalls.
The project source code is tested on vs2015, it is working well. And I write a test program to test this project. There are many C functions, I can't test all of them, so I just test some normal C functions, these functions work pretty good, and some aren't working, e.g., alarm.The Line64 aren't working with float number, there must be some difference when passing a float parameter or return a float parameter between linux ELF and windows PE.
The source code of the tested ELF executable can be found below.
If you have any suggestions, you can contact me at 4pplecn@gmail.com
I hope you can fork and star this repo, and do better job.

Makefile
/* 
all:
	gcc testlib.c -o testlib64 -Wl,-Ttext-segment=0x800000 -fno-stack-protector
clean:
	rm testlib64
*/

source code
/*
int func(a, b, c)
{
	printf("a: %d\n", a);
	printf("b: %d\n", b);
	printf("c: %d\n", c);
}
int main(int argc, char** argv)
{
	int i,j;
	char buf[100];
	printf("argc: %d\n", argc);
	for(i=0; i<argc; i++)
		puts(argv[i]);
	func(17, 19, 23);
	puts("hello from puts");
	write(1, "hello from write\n", 0x11);
	printf("hello from printf");
	puts("input i,i,s");
	scanf("%d %d %s", &i, &j, buf);
	printf("test: scanf printf: %d %d %s\n", i, j, buf);
	puts("test read input s");
	read(0, buf, 0x40);
	printf("echo read:%s\n", buf);
	puts("testing");
	puts("testing");
	puts("testing");
	puts("testing");
	printf("start multil args testing: %d %d %d %lld %lld %lld %x %x %x %x\n", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	puts("testing");
	puts("testing");
	puts("testing");
	puts("testing");
	puts("scanf s testing");
	scanf("%s", buf);
	printf("echo scanf: %s\n", buf);
	sleep(3);
}
*/
