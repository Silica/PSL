#ifndef PSL_H
#define PSL_H

#include "variable.h"

class PSL : private variable::Environment
{
	typedef variable::rsv rsv;
public:
	#ifndef PSL_DEBUG
	PSL() : variable::Variable::Environment(1){}
	#endif
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
		variable cc;
		unsigned long l;
		fread(&l, 1, sizeof(l), fp);
		if (l != 0xBCDEF01A)
			return NOT_COMPILED_CODE;
		size -= sizeof(l);
		variable::Variable::bytecode b(size);
		fread(b.get(), 1, size, fp);
		variable::Variable::bcreader::read(b, cc);
		variable g = global;
		g.gset(cc);
		g.prepare(*this);
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
	rsv Run(const variable &arg = 0)
	{
		if (!Runable())
			return rsv();
		#ifdef PSL_DEBUG
		if (!init)
		#endif
		push(arg);
		variable::Variable::Environment::Run();
		return pop();
	}
	#ifdef PSL_DEBUG
	PSL() : variable::Variable::Environment(1){init = false;}
	rsv StepExec()
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
			push(rsv());
		}
		variable::Variable::Environment::StepExec();
		return rsv();
	}
	#endif
	rsv get(const string &name)							{return global.get()->child(name);}
	void add(const string &name, const variable &v)		{global.get()->set(name, v);}
	void add(const string &name, variable::function f)	{global.get()->set(name, variable(f));}
	void add(const string &name, variable::method f)	{global.get()->set(name, variable(f));}
	#ifndef PSL_SHARED_GLOBAL
	operator variable::Variable::Environment&()			{return *this;}
	#endif
private:
	bool parse(variable::Tokenizer *t)
	{
		variable::Parser p(t);

		variable g;
		p.Parse(g);
		if (p.getErrorNum())
			return true;

		variable gl = global;
		gl.gset(g);
		gl.prepare(*this);
		return false;
	}
	#ifdef PSL_DEBUG
	bool init;
	#endif
	#include "binder.h"
};

#endif
