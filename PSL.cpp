
#include "PSL/PSL.h"

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

	using namespace PSL;
	PSLVM p;
	PSLVM::error e = p.LoadScript(argv[1]);
	if (e)
	{
		if (e == PSLVM::FOPEN_ERROR)
		{
			string input = argv[1];
			for (int i = 2; i < argc; ++i)
			{
				input += ' ';
				input += argv[i];
			}
			input += ';';
			p.LoadString(input);
			variable v = p.Run();
			printf("%s", v.toString().c_str());
			return 0;
		}
		else
		{
			printf(" - compile error\n");
		}
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
		variable v = p.Run(argv[1]);
	}

	#ifdef _WIN32
	printf("end - press any key\n");
	getch();
	#endif
	return 0;
}
