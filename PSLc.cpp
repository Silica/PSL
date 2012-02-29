
#include "variable.h"
#include "PSL.h"

#include <stdio.h>
#include <conio.h>

typedef unsigned long dword;

void compile(FILE *exe, const char *filename)
{
	PSL p;
	if (p.LoadScript(filename))
		return;

	string exename = filename;
	exename -= 4;
	exename += ".exe";
	FILE *fp = fopen(exename, "wb");
	if (!fp)
		return;
	fseek(exe, 0, SEEK_END);
	int end = ftell(exe);
	std::vector<unsigned char> byte(end);
	fseek(exe, 0, SEEK_SET);
	fread(&byte[0], 1, end, exe);
	fwrite(&byte[0], 1, end, fp);
	p.WriteCompiledCode(fp);
	dword l = 0xDEADC0DE;
	fwrite(&end, 1, sizeof(dword), fp);
	fwrite(&l, 1, sizeof(dword), fp);

	fclose(fp);
}

bool compiled(FILE *exe)
{
	int i = sizeof(dword);
	fseek(exe, -i, SEEK_END);
	dword l;
	fread(&l, 1, sizeof(dword), exe);
	return l == 0xDEADC0DE;
}

void execute(FILE *exe)
{
	int i = sizeof(dword) * 2;
	fseek(exe, -i, SEEK_END);
	dword l;
	int end = ftell(exe);
	fread(&l, 1, sizeof(dword), exe);
	fseek(exe, l, SEEK_SET);
	PSL p;
	if (p.LoadCompiledCode(exe, end - l))
		return;
	p.Run();
}

int main(int argc, char **argv)
{
	FILE *exe = fopen(argv[0], "rb");
	if (!exe)
		return 1;

	if (!compiled(exe))
	{
		if (argc >= 2)
			compile(exe, argv[1]);
	}
	else
	{
		execute(exe);
		getch();
	}

	fclose(exe);
	return 0;
}
