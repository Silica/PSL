#ifndef PSL_H
#define PSL_H

#include "variable.h"
#include <clocale>

class PSL : private variable::Environment
{
	typedef variable::rsv rsv;
public:
	enum error
	{
		NONE = 0,
		FOPEN_ERROR,
		PARSE_ERROR,
		NOT_COMPILED_CODE,
	};
	error LoadString(const string &str)
	{
		variable::Tokenizer t(str, "LoadString", 0);
		return parse(&t) ? PARSE_ERROR : NONE;
	}
	error LoadScript(const string &filename)
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
	error WriteCompiledCode(const string &filename)
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
	error LoadCompiledCode(const string &filename)
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
		variable::Environment::Run();
		return pop();
	}
	PSL() : variable::Environment(1)
	#ifndef PSL_DEBUG
	{std::setlocale(LC_ALL, "");}
	#else
	{std::setlocale(LC_ALL, "");init = false;}
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
		variable::Environment::StepExec();
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
	class addf
	{
		PSL *p;
	public:
		addf(PSL *x){p = x;}
		template<class F>addf &operator()(const string &s, F f)
		{
			p->addFunction(s, f);
			return *this;
		}
	};
	template<class C>class addc
	{
		rsv g;
		rsv r;
	public:
		addc(rsv &gl, variable &v){g = gl;r = v;}
		template<class M>addc &operator()(const string &s, M m)
		{
			variable c = r;
			variable v(variable::Method<C>(), m);
			c.set(s, v);
			return *this;
		}
		addc &instance(const string &s, C *p)
		{
			variable c = r;
			variable i = c.instance();
			i.push(p);
			variable n = g;
			n.set(s, i);
			return *this;
		}
	};
public:
	template<class F>addf addFunction(const string &s, F f)
	{
		variable g = global;
		variable v(variable::Function(), f);
		g.set(s, v);
		return addf(this);
	}
	template<class C>addc<C> addClass(const string &s)
	{
		variable g = global;
		variable v;
		g.set(s, v);
		return addc<C>(global, v);
	}
	template<class C>addc<C> addInstance(const string &classname, const string &s, C *p)
	{
		variable g = global;
		variable v = g[classname];
		variable i = v.instance();
		i.push(p);
		g.set(s, i);
		return addc<C>(global, v);
	}
};

#endif
