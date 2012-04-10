// ���\�R���p�C��
// �C���^�v���^(���ꎩ�g)�̌��Ƀo�C�g�R�[�h�𖄂ߍ���
#define private public // �ǂ�����p�v���O�����Ȃ̂��A�����Astd::vector�g���Ɨe�ʑ�������̂Łc
#include "PSL.h"
#undef private

#include <stdio.h>
#include <conio.h>

typedef unsigned long dword;

void compile(FILE *exe, const char *filename)
{
	PSL p;
	if (p.LoadScript(filename))
		return;

	string exename = filename;
	int period = exename.rfind('.');
	if (period > 0)
		exename /= period;
	exename += ".exe";
	FILE *fp = fopen(exename, "wb");
	if (!fp)
		return;
	fseek(exe, 0, SEEK_END);
	int end = ftell(exe);
	variable::vector<unsigned char> byte(end);
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

void execute(FILE *exe, variable &arg)
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
	p.Run(arg);
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
		variable arg;
		for (int i = 0; i < argc; i++)
			arg[i] = argv[i];
		execute(exe, arg);
		getch();
	}

	fclose(exe);
	return 0;
}
