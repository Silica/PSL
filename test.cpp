
#include "PSL.h"

#include <stdio.h>
#ifdef _WIN32
#include <conio.h>
#else
#define getch getchar
#endif
#include <time.h>


int main(int argc, char **argv)
{
	if (argc < 2)
		return 0;

	PSL p;
	if (p.LoadScript(argv[1]))
//	if (p.LoadCompiledCode(argv[1]))
	{
		printf(" - compile error\n");
	}
	else
	{
		printf(" - compiled\n");

	if (argc > 2)
		p.WriteCompiledCode(argv[2]);

#ifdef PSL_DEBUG
		variable r;
		while (!(r = p.StepExec())) if (getch() == 'q')break;
#endif
	clock_t start,end;
	start = clock();
		variable v = p.Run(argv[1]);
	end = clock();
	printf("PSL:%.2fsec\n",(double)(end-start)/CLOCKS_PER_SEC);
//		printf(" - dump returned:\n");
//		v.dump();
	}

	printf("end - press any key\n");
	getch();
	return 0;
}
