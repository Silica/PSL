#ifdef __GNUC__
public:
#endif
class Environment;
private:
class OpCode
{
public:
	struct MNEMONIC
	{
		enum mnemonic
		{
			NOP = 0,
			PUSH,
			PUSH_INT,
			PUSH_HEX,
			PUSH_FLOAT,
			PUSH_STRING,
			PUSH_NULL,
			POP,
			VARIABLE,

			GLOBAL,
			LOCAL,
			STATIC,
			DECLARATION,
			INSTANCE,

			MEMBER,
			INDEX,

			CALL,
			RETURN,
			BREAK,
			CONTINUE,
			YIELD,
			GOTO,
			SCOPE,
			LOOP,
			IF,
			LABEL,

			MOV,
			SUBSTITUTION,
			SUBSTITUTE,
			ASSIGN,
			ASSIGNMENT,
			ARGUMENT,

			MINUS,		// - neg?
			PLUS,		// +
			DEREF,		// *
			REF,		// &
			NOT,		// !
			COMPL,		// ~
			INC,
			DEC,
			PINC,
			PDEC,

			ADD,
			SUB,
			MUL,
			DIV,
			MOD,
			AND,
			OR,
			XOR,
			SHL,
			SHR,

			SADD,
			SSUB,
			SMUL,
			SDIV,
			SMOD,
			SAND,
			SOR,
			SXOR,
			SSHL,
			SSHR,

			// 比較
			EQ,
			NEQ,
			LE,
			GE,
			LT,
			GT,

			BAND,
			BOR,

			JMP,
			JT,
			JF,
			JR,
			JRT,
			JRF,

			LIST,			// , comma
			PARENTHESES,	// ()

			LABELBEGIN = 0xF0,
			BEGIN,
			END = 0xFF
		};
	};
	struct RC
	{
		enum RETURNCODE
		{
			NONE = 0,
			CALL,
			BREAK,
			CONTINUE,
			RETURN,
			GOTO,
			YIELD,
			END,
			SCOPE,
			IF,
			LOOP,
		};
	};
	virtual RC::RETURNCODE Execute(Environment &env) = 0;
	virtual ~OpCode(){}
	virtual void set(int s){}
	virtual MNEMONIC::mnemonic get()	{return MNEMONIC::NOP;}
	virtual void write(bytecode &b)	{};
	virtual void dump(int d = 0){};
private:
};

#ifdef PSL_WARNING_POP_EMPTY_STACK
	#define PSL_RW_PES 4
#else
	#define PSL_RW_PES 0
#endif
#ifdef PSL_WARNING_STACK_REMAINED
	#define PSL_RW_SR 1
#else
	#define PSL_RW_SR 0
#endif
#ifdef PSL_WARNING_UNDECLARED_IDENTIFIER
	#define PSL_RW_UI 2
#else
	#define PSL_RW_UI 0
#endif
#define PSL_ENVIRONMENT_WARNING (PSL_RW_PES | PSL_RW_SR | PSL_RW_UI)

class Scope;
#ifdef __GNUC__
public:
#endif
class Environment
{
public:
	Environment()	{scope = NULL;warning = PSL_ENVIRONMENT_WARNING;PSLlib::Basic(global);}
	Environment(const Environment &env):global(env.global)	{scope = NULL;warning = env.warning;}
	~Environment()	{delete scope;if(warning&1)if (int ss=stack.size())std::printf("runtime warning : %d stack remained when deleting env\n", ss);}
	rsv getVariable(const string &name)
	{
		Variable *v = scope->getVariable(name);
		if (v)								return v;
		else if (global.get()->exist(name))	return global.get()->child(name);
		if (warning&2)
			std::printf("runtime warning : undeclared identidier %s\n", name.c_str());
		variable x;
		scope->addLocal(name, x);
		return x;
	}
	void addLocal(const string &name, variable &v)		{scope->addLocal(name, v, this);}
	void addGlobal(const string &name, variable &v)		{global.get()->set(name, v);push(v);}
	void addStatic(const string &name, variable &v)		{scope->addStatic(name, v, this);}
	void Declaration(const string &name, variable &v)	{scope->Declaration(name, v, this);}
	void addScope(Scope *s)	{s->set(scope);scope = s;}
	void Jump(int l)	{scope->Jump(l);}
	void RJump(int l)	{scope->RJump(l);}
	void endScope()		{scope = scope->End(*this);	}
	void Run()			{while (scope && scope->Run(*this));}
	void StepExec()		{if (scope) scope->StepExec(*this);}
	void Return()		{scope = scope->Return();}
	void Break()		{scope = scope->Break();}
	void Continue()		{scope = scope->Continue();}
	void Yield()		{scope = scope->Yield();}
	void Goto(const string &label)	{Scope *s = scope->Goto(label);if (s)scope = s;}
	OpCode::MNEMONIC::mnemonic getNext()		{return scope->getNext();}
#ifdef PSL_USE_STL_STACK
	void push(const rsv &v)	{stack.push(v);}
#else
	void push(const rsv &v)	{stack.push_back(v);}
#endif
	rsv pop()
	{
		if (stack.empty())
		{
			if (warning&4)
				std::printf("runtime error : pop empty stack\n");
			return rsv();
		}
#ifdef PSL_USE_STL_STACK
		rsv v = stack.top();
		stack.pop();
#else
		int size = stack.size();
		rsv v = stack[size-1];
		stack.resize(size-1);
#endif
		return v;
	}
	bool Runable()	{return scope;}
protected:
	rsv global;
#ifdef PSL_USE_STL_STACK
	std::stack<rsv> stack;
#else
	rlist stack;
#endif
	Scope *scope;
	unsigned long warning;
};
private:

class Code
{
public:
	Code()	{rc = 1;}
	~Code()
	{
		for (size_t i = 0; i < code.size(); ++i)
			delete code[i];
	}
	void finalize()
	{
		if (!--rc)
			delete this;
	}
	bool Run(Environment &env, size_t &line)
	{
		while (line < code.size())
		{
			switch (code[line++]->Execute(env))
			{
			case OpCode::RC::YIELD:
				return false;
			case OpCode::RC::NONE:
				break;
			default:
				return true;
			}
		}
		env.endScope();
		return true;
	}
	void StepExec(Environment &env, size_t &line)
	{
		if (line < code.size())
		{
			for (table::iterator it = label.begin(); it != label.end(); ++it)
			{
				if ((unsigned)it->second.get()->toInt() == line)
					std::printf("%s:\n", it->first.c_str());
			}
			std::printf("exec: ");
			code[line]->dump(1);
			code[line++]->Execute(env);
		}
		else
		{
			std::printf("exec: END\n");
			env.endScope();
		}
	}
	OpCode::MNEMONIC::mnemonic get(size_t line)
	{
		if (line < code.size())	return code[line]->get();
		else					return OpCode::MNEMONIC::END;
	}
	bool Goto(const string &s, size_t &line)
	{
		if (!label.count(s))
			return false;
		line = label[s].get()->toInt();
		return true;
	}
//	void pushlabel(const string &s)			{label[s] = code.size();}
	void pushlabel(const string &s)			{variable v = (int)code.size();label[s] = v;}
	void pushlabel(const string &s, int l)	{variable v = l;label[s] = v;}
	void push(OpCode *c)			{code.push_back(c);}
	void push(Code *c)
	{
		for (size_t i = 0; i < c->code.size(); ++i)
			code.push_back(c->code[i]);
		c->code.resize(0);
	}
	size_t length()	{return code.size();}
	Code *inc()	{++rc;return this;}
	void dump()
	{
		for (size_t i = 0; i < code.size(); ++i)
			code[i]->dump();
//		for (std::map<string,int>::iterator it = label.begin(); it != label.end(); ++it)
//			std::printf("%s:%d\n", it->first.c_str(), it->second);
		for (table::iterator it = label.begin(); it != label.end(); ++it)
			std::printf("%s:%d\n", it->first.c_str(), it->second.get()->toInt());
	}
	void write(bytecode &b)
	{
		int size = code.size();
		for (int i = 0; i < size; ++i)
			code[i]->write(b);
		if (label.size())
		{
			b.push(OpCode::MNEMONIC::LABELBEGIN);
			for (table::iterator it = label.begin(); it != label.end(); ++it)
			{
				int x = it->second.get()->toInt();
				b.push(&x, sizeof(x));
				b.push(it->first.c_str(), it->first.length()+1);
			}
			b.push(OpCode::MNEMONIC::END);
		}
		b.push(OpCode::MNEMONIC::END);	// ENDは関数だけじゃなくループにもあるのでやっぱりここに
	};
private:
	#ifdef PSL_USE_STL_VECTOR
	std::vector<OpCode*> code;
	#else
	vector<OpCode*> code;
	#endif
//	std::map<string,int> label;	// これやると容量とても増えるんだ
	table label;
	int rc;
};

class Scope
{
public:
	enum Type
	{
		NONE,
		ANONYMOUS,
		LOOP,
		FUNCTION,
		CONSTRUCTOR,
		METHOD,
	};
	Scope(Code *c)			{code = c->inc();line = 0;owner = NULL;}
	virtual ~Scope()		{code->finalize();delete owner;}
	virtual Type getType()	{return NONE;}
	virtual Variable *getVariable(const string &name)
	{
		if (local.get()->exist(name))	return local.get()->child(name);
		else						return owner->getVariable(name);
	}
	void set(Scope *s)
	{
		if (!owner)		owner = s;
		else			owner->set(s);
	}
	void Jump(int l)					{line = l;}
	void RJump(int l)					{line += l;}
	virtual bool Run(Environment &env)	{return code->Run(env, line);}
	virtual void StepExec(Environment &env)	{return code->StepExec(env, line);}
	virtual OpCode::MNEMONIC::mnemonic getNext()	{return code->get(line);}
	virtual Scope *Return() = 0;
	virtual Scope *Break() = 0;
	virtual Scope *Continue() = 0;
	virtual Scope *Yield()	{return this;}	// って何もすることなくね？
	virtual Scope *Goto(const string &label)
	{
		if (code->Goto(label, line))
			return this;
		if (!owner)
			return NULL;
		Scope *s = owner->Goto(label);
		if (s)
		{
			owner = NULL;
			delete this;
			return s;
		}
		return NULL;
	}
	virtual Scope *End(Environment &env)
	{
		Scope *s = owner;
		owner = NULL;
		delete this;
		return s;
	}
	bool addLocal(const string &name, variable &v, Environment *env = NULL)
	{
		bool r = local.get()->set(name, v);
		if (env)	env->push(v);
		return r;
	}
	virtual bool addStatic(const string &name, variable &v, Environment *env)
	{
		if (owner)		return owner->addStatic(name, v, env);
		else if (env)	env->push(v);
		return false;
	}
	virtual bool Declaration(const string &name, variable &v, Environment *env)
	{
		bool r = local.get()->set(name, v);
		if (env)	env->push(v);
		return r;
	}
protected:
	rsv local;
	Scope *owner;
	Code *code;
	size_t line;
};

#include "scope.h"
#include "code.h"
