class Environment;
class OpCode
{
public:
	struct MNEMONIC
	{
		enum mnemonic
		{
			NOP = 0,
			CONSTANT,
			PUSH,
			PUSH_INT,
			PUSH_HEX,
			PUSH_FLOAT,
			PUSH_STRING,
			PUSH_NULL,
			PUSH_CODE,
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

			UNARY,
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

			BINARY,
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

			// ��r
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
#ifdef PSL_WARNING_DECLARED_IDENTIFIER_ALREADY_EXIST
	#define PSL_RW_DIAE 8
#else
	#define PSL_RW_DIAE 0
#endif
#define PSL_ENVIRONMENT_WARNING (PSL_RW_PES | PSL_RW_SR | PSL_RW_UI | PSL_RW_DIAE)

class Scope;
class Environment
{
	const static unsigned long WARNING = PSL_ENVIRONMENT_WARNING;
	void warning(const int n, const string &s = "")
	{
		if (WARNING & (1 << n))
		{
			switch (n)
			{
			case 0:PSL_PRINTF(("runtime warning : %s stack remained when deleting env\n", s.c_str()));break;
			case 1:PSL_PRINTF(("runtime warning : undeclared identidier %s\n", s.c_str()));break;
			case 2:PSL_PRINTF(("runtime error : pop empty stack\n"));break;
			case 3:PSL_PRINTF(("runtime warning : declared identidier %s already exist, over write\n", s.c_str()));break;
			}
		}
	}
public:
	Environment()	{scope = NULL;PSLlib::Basic(global);}
	Environment(const Environment &env):global(env.global)	{scope = NULL;}
	~Environment()	{delete scope;if (int ss=stack.size())warning(0, ss);}
	rsv getVariable(const string &name)
	{
		Variable *v = scope->getVariable(name);
		if (v)								return v;
		else if (global.get()->exist(name))	return global.get()->child(name);
		warning(1, name);
		variable x;
		scope->addLocal(name, x);
		return x;
	}
	void addLocal(const string &name, variable &v)		{if (!scope->addLocal(name, v, this))warning(3, name);}
	void addGlobal(const string &name, variable &v)		{if (!global.get()->set(name, v))warning(3, name);push(v);}
	void addStatic(const string &name, variable &v)		{scope->addStatic(name, v, this);}
	void Declaration(const string &name, variable &v)	{if (!scope->Declaration(name, v, this))warning(3, name);}
	void addScope(Scope *s)	{s->set(scope);scope = s;}
	void Jump(int l)	{scope->Jump(l);}
	void RJump(int l)	{scope->RJump(l);}
	void endScope()		{scope = scope->End(*this);	}
	void Run()			{while (scope && scope->Run(*this));}
	#ifdef PSL_DEBUG
	void StepExec()		{if (scope) scope->StepExec(*this);}
	#endif
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
			warning(2);
			return rsv();
		}
#ifdef PSL_USE_STL_STACK
		rsv v = stack.top();
		stack.pop();
#else
		int size = stack.size();
		rsv v = stack[size-1];
		#ifdef PSL_POPSTACK_NULL
		const static rsv null;
		stack[size-1] = null;
		#endif
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
};

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
	#ifdef PSL_DEBUG
	void StepExec(Environment &env, size_t &line)
	{
		if (line < code.size())
		{
			for (table::iterator it = label.begin(); it != label.end(); ++it)
			{
				if ((unsigned)it->second.get()->toInt() == line)
					PSL_PRINTF(("%s:\n", it->first.c_str()));
			}
			PSL_PRINTF(("exec: "));
			code[line]->dump(1);
			if (code[line++]->Execute(env) == OpCode::RC::YIELD)	// �X�^�b�N�s������h���ׂɁA�ЂƂ܂�yield�p��NULL��n�����Ƃɂ���
				env.push(rsv());
		}
		else
		{
			PSL_PRINTF(("exec: END\n"));
			env.endScope();
		}
	}
	#endif
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
	void push(OpCode *c)
	{
		#ifdef PSL_OPTIMIZE_IN_COMPILE
		if (optimize(c))
		#endif
		code.push_back(c);
	}
	void push(Code *c)
	{
		for (size_t i = 0; i < c->code.size(); ++i)
			code.push_back(c->code[i]);
		c->code.resize(0);
	}
	size_t length()	{return code.size();}
	Code *inc()	{++rc;return this;}
	PSL_DUMP((){
		for (size_t i = 0; i < code.size(); ++i)
			code[i]->dump();
//		for (std::map<string,int>::iterator it = label.begin(); it != label.end(); ++it)
//			PSL_PRINTF(("%s:%d\n", it->first.c_str(), it->second));
		for (table::iterator it = label.begin(); it != label.end(); ++it)
			PSL_PRINTF(("%s:%d\n", it->first.c_str(), it->second.get()->toInt()));
	})
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
		b.push(OpCode::MNEMONIC::END);	// END�͊֐���������Ȃ����[�v�ɂ�����̂ł���ς肱����
	};
private:
	PSL_VECTOR<OpCode*> code;
//	std::map<string,int> label;	// ������Ɨe�ʂƂĂ��������
	table label;
	int rc;
	bool optimize(OpCode *c)
	{
		static Environment optimizer;
		OpCode::MNEMONIC::mnemonic cn = c->get();
		if (cn == OpCode::MNEMONIC::PLUS)
		{
			delete c;
			return false;
		}
		if (cn == OpCode::MNEMONIC::POP)
		{
			int s = code.size();
			if (s >= 1)
			{
				OpCode::MNEMONIC::mnemonic n = code[s-1]->get();
				#ifdef PSL_OPTIMIZE_IMMEDIATELY_POP
				if (n == OpCode::MNEMONIC::CONSTANT || n == OpCode::MNEMONIC::VARIABLE)
				{
					if (s < 2 || code[s-2]->get() != OpCode::MNEMONIC::JR)
					{
						delete c;
						delete code[s-1];
						code.resize(s-1);
						return false;
					}
				}
				#endif
				#ifdef PSL_OPTIMIZE_SUFFIX_INCREMENT
				if (n == OpCode::MNEMONIC::INC)
				{
					delete code[s-1];
					code[s-1] = new PINC();
					return true;
				}
				if (n == OpCode::MNEMONIC::DEC)
				{
					delete code[s-1];
					code[s-1] = new PDEC();
					return true;
				}
				#endif
			}
		}
		#ifdef PSL_OPTIMIZE_CONSTANT_CALCULATION
		if (cn == OpCode::MNEMONIC::UNARY || cn == OpCode::MNEMONIC::INC || cn == OpCode::MNEMONIC::DEC)
		{
			int s = code.size();
			if (s >= 1)
			{
				if (code[s-1]->get() == OpCode::MNEMONIC::CONSTANT)
				{
					code[s-1]->Execute(optimizer);
					variable v = optimizer.pop();
					optimizer.push(v);
					c->Execute(optimizer);
					variable a = optimizer.pop();
					v = a;
					delete c;
					return false;
				}
			}
		}
		if (cn == OpCode::MNEMONIC::BINARY)
		{
			int s = code.size();
			if (s >= 2)
			{
				if ((code[s-1]->get() == OpCode::MNEMONIC::CONSTANT) && (code[s-2]->get() == OpCode::MNEMONIC::CONSTANT))
				{
					code[s-2]->Execute(optimizer);
					code[s-1]->Execute(optimizer);
					variable r = optimizer.pop();
					variable l = optimizer.pop();
					optimizer.push(l);
					optimizer.push(r);
					c->Execute(optimizer);
					variable a = optimizer.pop();
					l = a;
					delete c;
					delete code[s-1];
					code.resize(s-1);
					return false;
				}
			}
		}
		#endif
		return true;
	}
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
	#ifdef PSL_DEBUG
	virtual void StepExec(Environment &env)	{return code->StepExec(env, line);}
	#endif
	virtual OpCode::MNEMONIC::mnemonic getNext()	{return code->get(line);}
	virtual Scope *Return() = 0;
	virtual Scope *Break() = 0;
	virtual Scope *Continue() = 0;
	virtual Scope *Yield()	{return this;}	// ���ĉ������邱�ƂȂ��ˁH
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
