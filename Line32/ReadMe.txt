========================================================================
    Line32
========================================================================

Line32 is designed for running ELF executable with 32 bits in Windows environment.
Line32 is running under 32 bit x86 cpu, and its function call convention is pretty samilar while an executable running in windows environment or linux environment. They just put their arguments into stack, so it's very easy to call a windows function in ELF text segment.
Line32 treat long long unfriendly, it's a little problem when call printf to print the long long integer. 
A sample makefile looks like below.
Line32 still can not load FULL RELRO executables now.
Line32 can not load statically linked executable since linux syscalls are different from windows syscalls.
The project source code is tested on vs2015, it is working well. And I write a test program to test this project. There are many C functions, I can't test all of them, so I just test some normal C functions, these functions work pretty good, and some aren't working, e.g., alarm.The Line32 aren't working with float number, there must be some difference when passing a float parameter or return a float parameter between linux ELF and windows PE.
The source code of the tested ELF executable can be found below.
If you have any suggestions, you can contact me at 4pplecn@gmail.com
I hope you can fork and star this repo, and do better job.

Makefile
/* 
all:	
	gcc testlib.c -o testlib -m32
clean:
	rm testlib
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
	printf("start multil args testing: %d %d %d %lld %lld %lld %x %x %x %x\n", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);  //unfriendly print
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
