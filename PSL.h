#ifndef PSL_H
#define PSL_H

#include "variable.h"

class PSL : private variable::environment
{
public:
	PSL() : variable::Variable::Environment(1){}
	enum error
	{
		NONE = 0,
		FOPEN_ERROR,
		PARSE_ERROR,
		NOT_COMPILED_CODE,
	};
	error LoadScript(const char *filename)
	{
		if (!LoadCompiledCode(filename))
			return NONE;
		variable::Tokenizer *t = variable::Tokenizer::New(filename);
		if (!t)
			return FOPEN_ERROR;

		bool e = parse(t);
		delete t;

		if (e)
			return PARSE_ERROR;
		return NONE;
	}
	error WriteCompiledCode(std::FILE *fp)
	{
		variable::Variable::bytecode b;
		global.get()->write("", b);
		unsigned long l = 0xBCDEF01A;
		fwrite(&l, 1, sizeof(l), fp);
		fwrite(b.get(), 1, b.size(), fp);
		return NONE;
	}
	error WriteCompiledCode(const char *filename)
	{
		using namespace std;
		FILE *fp = fopen(filename, "wb");
		if (!fp)
			return FOPEN_ERROR;
		WriteCompiledCode(fp);
		fclose(fp);
		return NONE;
	}
	error LoadCompiledCode(std::FILE *fp, unsigned long size)
	{
		variable cc = global;
		unsigned long l;
		fread(&l, 1, sizeof(l), fp);
		if (l != 0xBCDEF01A)
			return NOT_COMPILED_CODE;
		size -= sizeof(l);
		variable::Variable::bytecode b(size);
		fread(b.get(), 1, size, fp);
//		variable::PSLlib::Basic(global);
		variable::Variable::bcreader::read(b, cc);
		cc.prepare(*this);
		return NONE;
	}
	error LoadCompiledCode(const char *filename)
	{
		using namespace std;
		FILE *fp = fopen(filename, "rb");
		if (!fp)
			return FOPEN_ERROR;
		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		error e = LoadCompiledCode(fp, size);
		fclose(fp);
		return e;
	}
	variable::rsv Run(const variable &arg = 0)
	{
		if (!Runable())
			return variable::rsv();
		#ifdef PSL_DEBUG
		if (!init)
		#endif
		push(arg);
		variable::Variable::Environment::Run();
//		PSL_PRINTF(("stack:%d\n", stack.size()));
		return pop();
	}
	#ifdef PSL_DEBUG
	PSL()	{init = false;}
	variable::rsv StepExec()
	{
		if (!Runable())
		{
			if (stack.size())
				return pop();
			return variable(1);
		}
		if (!init)
		{
			init = true;
			push(variable::rsv());
		}
		variable::Variable::Environment::StepExec();
		return variable::rsv();
	}
	#endif
	variable::rsv get(const string &name)
	{
		return global.get()->child(name);
	}
	operator variable::Variable::Environment&()
	{
		return *this;
	}
private:
	bool parse(variable::Tokenizer *t)
	{
		variable::Parser p(t);
//		variable::PSLlib::Basic(global);

		variable g = global;
		p.Parse(g);
		if (p.getErrorNum())
			return true;

//		PSL_PRINTF(("stack:%d\n", stack.size()));
//		g.dump();

		global.get()->prepare(*this);
		return false;
	}
	#ifdef PSL_DEBUG
	bool init;
	#endif
};

#endif
