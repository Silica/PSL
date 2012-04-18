#define GET(n) MNEMONIC::mnemonic get(){return MNEMONIC::n;}
#define CLONE(n) OpCode *clone(){return new n;}
#define EXEC RC::RETURNCODE Execute(Environment &env)
#define WRITE(n,x) void write(bytecode &b){b.push(MNEMONIC::n);x}
class PUSH_INT : public OpCode
{
public:
	PUSH_INT(int i):x(new Variable(i),0){}
	CLONE(PUSH_INT(x.get()->toInt()))
	EXEC
	{
		env.push(x);
		return RC::NONE;
	}
	GET(CONSTANT)
	PSL_DUMP((int d){PSL_PRINTF(("PUSH %d\n", x.get()->toInt()));})
	WRITE(PUSH_INT,int w = x.get()->toInt();b.push(&w, sizeof(w));)
private:
	rsv x;
};
class PUSH_HEX : public OpCode
{
public:
	PUSH_HEX(hex l):x(new Variable(new vHex(l)),0){}
	CLONE(PUSH_HEX(x.get()->toInt()))
	EXEC
	{
		env.push(x);
		return RC::NONE;
	}
	GET(CONSTANT)
	PSL_DUMP((int d){PSL_PRINTF(("PUSH %X\n", x.get()->toInt()));})
	WRITE(PUSH_HEX,hex w = x.get()->toInt();b.push(&w, sizeof(w));)
private:
	rsv x;
};
class PUSH_FLOAT : public OpCode
{
public:
	PUSH_FLOAT(double d):x(new Variable(d),0){}
	CLONE(PUSH_FLOAT(x.get()->toDouble()))
	EXEC
	{
		env.push(x);
		return RC::NONE;
	}
	GET(CONSTANT)
	PSL_DUMP((int d){PSL_PRINTF(("PUSH %f\n", x.get()->toDouble()));})
	WRITE(PUSH_FLOAT,double w = x.get()->toDouble();b.push(&w, sizeof(w));)
private:
	rsv x;
};
class PUSH_STRING : public OpCode
{
public:
	PUSH_STRING(const string &s):x(new Variable(s),0){}
	CLONE(PUSH_STRING(x.get()->toString()))
	EXEC
	{
		env.push(x);
		return RC::NONE;
	}
	GET(CONSTANT)
	PSL_DUMP((int d){PSL_PRINTF(("PUSH %s\n", x.get()->toString().c_str()));})
	WRITE(PUSH_STRING,string w = x.get()->toString();b.push(w.c_str(), w.length()+1);)
private:
	rsv x;
};
class POP : public OpCode
{
public:
	CLONE(POP)
	EXEC
	{
		env.pop();
		return RC::NONE;
	}
	GET(POP)
	PSL_DUMP((int d){PSL_PRINTF(("POP\n"));})
	WRITE(POP,)
};
class PUSH_NULL : public OpCode
{
public:
	CLONE(PUSH_NULL)
	EXEC
	{
		variable v;
		env.push(v);
		return RC::NONE;
	}
	GET(CONSTANT)
	PSL_DUMP((int d){PSL_PRINTF(("PUSH NULL\n"));})
	WRITE(PUSH_NULL,)
};

class SUBSTITUTION : public OpCode
{
public:
	CLONE(SUBSTITUTION)
	EXEC
	{
		variable r = env.pop();
		variable l = env.top();
		l.substitution(r);
		return RC::NONE;
	}
	GET(BINARY)
	PSL_DUMP((int d){PSL_PRINTF(("SUBSTITUTION\n"));})
	WRITE(SUBSTITUTION,)
};
class ASSIGNMENT : public OpCode
{
public:
	CLONE(ASSIGNMENT)
	EXEC
	{
		variable r = env.pop();
		variable l = env.top();
		l.assignment(r);
		return RC::NONE;
	}
	GET(BINARY)
	PSL_DUMP((int d){PSL_PRINTF(("ASSIGNMENT\n"));})
	WRITE(ASSIGNMENT,)
};
class ARGUMENT : public OpCode
{
public:
	CLONE(ARGUMENT)
	EXEC
	{
		variable l = env.pop();
		variable r = env.pop();
		env.push(l.substitution(r));
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("ARGUMENT\n"));})
	WRITE(ARGUMENT,)
};


class PLUS : public OpCode
{
public:
	CLONE(PLUS)
	EXEC
	{
		return RC::NONE;
	}
	GET(PLUS)
	PSL_DUMP((int d){PSL_PRINTF(("PLUS\n"));})
	WRITE(PLUS,)
};
class MINUS : public OpCode
{
public:
	CLONE(MINUS)
	EXEC
	{
		variable v = env.pop();
		env.push(-v);
		return RC::NONE;
	}
	GET(UNARY)
	PSL_DUMP((int d){PSL_PRINTF(("MINUS\n"));})
	WRITE(MINUS,)
};
class NOT : public OpCode
{
public:
	CLONE(NOT)
	EXEC
	{
		variable v = env.pop();
		variable x = !static_cast<bool>(v);
		env.push(x);
		return RC::NONE;
	}
	GET(UNARY)
	PSL_DUMP((int d){PSL_PRINTF(("NOT\n"));})
	WRITE(NOT,)
};
class COMPL : public OpCode
{
public:
	CLONE(COMPL)
	EXEC
	{
		variable v = env.pop();
		variable x(~static_cast<int>(v));
		env.push(x);
		return RC::NONE;
	}
	GET(UNARY)
	PSL_DUMP((int d){PSL_PRINTF(("COMPL\n"));})
	WRITE(COMPL,)
};
class INC : public OpCode	// 後置
{
public:
	CLONE(INC)
	EXEC
	{
		variable v = env.pop();
		variable c = v;	// 値をコピー
		env.push(c);
		v += 1;
		return RC::NONE;
	}
	GET(INC)
	PSL_DUMP((int d){PSL_PRINTF(("INC\n"));})
	WRITE(INC,)
};
class PINC : public OpCode	// 前置
{
public:
	CLONE(PINC)
	EXEC
	{
		variable v = env.top();
		v += 1;
		return RC::NONE;
	}
	GET(UNARY)
	PSL_DUMP((int d){PSL_PRINTF(("PINC\n"));})
	WRITE(PINC,)
};
class DEC : public OpCode	// 後置
{
public:
	CLONE(DEC)
	EXEC
	{
		variable v = env.pop();
		variable c = v;	// 値をコピー
		env.push(c);
		v -= 1;
		return RC::NONE;
	}
	GET(DEC)
	PSL_DUMP((int d){PSL_PRINTF(("DEC\n"));})
	WRITE(DEC,)
};
class PDEC : public OpCode	// 前置
{
public:
	CLONE(PDEC)
	EXEC
	{
		variable v = env.top();
		v -= 1;
		return RC::NONE;
	}
	GET(UNARY)
	PSL_DUMP((int d){PSL_PRINTF(("PDEC\n"));})
	WRITE(PDEC,)
};
class DEREF : public OpCode
{
public:
	CLONE(DEREF)
	EXEC
	{
		variable v = env.pop();
		env.push(*v);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("DEREF\n"));})
	WRITE(DEREF,)
};
class REF : public OpCode
{
public:
	CLONE(REF)
	EXEC
	{
		variable v = env.pop();
		env.push(v.pointer());
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("REF\n"));})
	WRITE(REF,)
};
#define OOC(n,o) class n:public OpCode{public:CLONE(n)EXEC{variable r=env.pop();variable l=env.top();l o r;return RC::NONE;}GET(BINARY)PSL_DUMP((int d){PSL_PRINTF((#n"\n"));})WRITE(n,)}
OOC(SADD,+=);
OOC(SSUB,-=);
OOC(SMUL,*=);
OOC(SDIV,/=);
OOC(SMOD,%=);
OOC(SAND,&=);
OOC(SOR,|=);
OOC(SXOR,^=);
OOC(SSHL,<<=);
OOC(SSHR,>>=);
//OOC(MOV,=);
#undef OOC
#define OOC(n,o) class n:public OpCode{public:CLONE(n)EXEC{variable r=env.pop();variable l=env.pop();variable x(l o r);env.push(x);return RC::NONE;}GET(BINARY)PSL_DUMP((int d){PSL_PRINTF((#n"\n"));})WRITE(n,)}
OOC(ADD,+);
OOC(SUB,-);
OOC(MUL,*);
OOC(DIV,/);
OOC(MOD,%);
OOC(AND,&);
OOC(OR,|);
OOC(XOR,^);
OOC(SHL,<<);
OOC(SHR,>>);
OOC(EQ,==);
OOC(NEQ,!=);
OOC(LE,<=);
OOC(GE,>=);
OOC(LT,<);
OOC(GT,>);
#undef OOC

class VARIABLE : public OpCode
{
public:
	VARIABLE(string &s)	{name = s;}
	CLONE(VARIABLE(name))
	EXEC
	{
		variable v = env.getVariable(name);
		env.push(v);
		return RC::NONE;
	}
	GET(VARIABLE)
	PSL_DUMP((int d){PSL_PRINTF(("VARIABLE %s\n", name.c_str()));})
	WRITE(VARIABLE,b.push(name.c_str(), name.length()+1);)
private:
	string name;
};

class BAND : public OpCode
{
public:
	CLONE(BAND)
	EXEC
	{
		variable r = env.pop();
		variable l = env.pop();
		variable v(static_cast<bool>(l) && static_cast<bool>(r));
		env.push(v);
		return RC::NONE;
	}
	GET(BINARY)
	PSL_DUMP((int d){PSL_PRINTF(("BAND\n"));})
	WRITE(BAND,)
};
class BOR : public OpCode
{
public:
	CLONE(BOR)
	EXEC
	{
		variable r = env.pop();
		variable l = env.pop();
		variable v(static_cast<bool>(l) || static_cast<bool>(r));
		env.push(v);
		return RC::NONE;
	}
	GET(BINARY)
	PSL_DUMP((int d){PSL_PRINTF(("BOR\n"));})
	WRITE(BOR,)
};



class PUSH_CODE : public OpCode
{
public:
	PUSH_CODE(const rsv &v):x(v){}
	CLONE(PUSH_CODE(x))
	EXEC
	{
		variable v = x;		// 一旦受け取り
		variable c = v;		// コピー作成
		env.push(c);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("PUSH CODE\n"));if (!d){x.get()->dump();PSL_PRINTF(("CODE END\n"));}})
	WRITE(PUSH_CODE,x.get()->write("", b);)
private:
	rsv x;
};
class CLOSURE : public OpCode
{
public:
	CLONE(CLOSURE)
	EXEC
	{
		variable v = env.top();
		#ifdef PSL_CLOSURE_REFERENCE
		env.setLocal(v);
		#else
		v = env.getLocal();	// ローカル変数受け取り
		#endif
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("CLOSURE\n"));})
	WRITE(CLOSURE,)
};


class JMP : public OpCode
{
public:
	JMP(int i)	{j = i;}
	CLONE(JMP(j))
	void set(int s){j = s;}
	EXEC
	{
		env.Jump(j);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("JMP %d\n", j));})
	WRITE(JMP,b.push(&j, sizeof(j));)
private:
	int j;
};
class JT : public OpCode
{
public:
	JT(int i)	{j = i;}
	CLONE(JT(j))
	void set(int s){j = s;}
	EXEC
	{
		variable v = env.pop();
		if (static_cast<bool>(v))
			env.Jump(j);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("JT %d\n", j));})
	WRITE(JT,b.push(&j, sizeof(j));)
private:
	int j;
};
class JF : public OpCode
{
public:
	JF(int i)	{j = i;}
	CLONE(JF(j))
	void set(int s){j = s;}
	EXEC
	{
		variable v = env.pop();
		if (!v)
			env.Jump(j);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("JF %d\n", j));})
	WRITE(JF,b.push(&j, sizeof(j));)
private:
	int j;
};
class JR : public OpCode		// 相対ジャンプ、行は実行時には次を指していることに注意
{
public:
	JR(int i)	{j = i;}
	CLONE(JR(j))
	void set(int s){j = s;}
	EXEC
	{
		env.RJump(j);
		return RC::NONE;
	}
	GET(JR)
	PSL_DUMP((int d){PSL_PRINTF(("JR %d\n", j));})
	WRITE(JR,b.push(&j, sizeof(j));)
private:
	int j;
};
class JRT : public OpCode
{
public:
	JRT(int i)	{j = i;}
	CLONE(JRT(j))
	void set(int s){j = s;}
	EXEC
	{
		variable v = env.pop();
		if (static_cast<bool>(v))
			env.RJump(j);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("JRT %d\n", j));})
	WRITE(JRT,b.push(&j, sizeof(j));)
private:
	int j;
};
class JRF : public OpCode
{
public:
	JRF(int i)	{j = i;}
	CLONE(JRF(j))
	void set(int s){j = s;}
	EXEC
	{
		variable v = env.pop();
		if (!v)
			env.RJump(j);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("JRF %d\n", j));})
	WRITE(JRF,b.push(&j, sizeof(j));)
private:
	int j;
};

class LIST : public OpCode
{
public:
	CLONE(LIST)
	EXEC
	{
		variable r = env.pop();
		variable l = env.pop();
		if (l.type(variable::RARRAY))
		{
			l.push(r);
			env.push(l);
		}
		else
		{
			variable ref(variable::RARRAY);
			ref.push(l);
			ref.push(r);
			env.push(ref);
		}
		return RC::NONE;
	}
	GET(LIST)
	PSL_DUMP((int d){PSL_PRINTF(("LIST\n"));})
	WRITE(LIST,)
};

class PARENTHESES : public OpCode
{
public:
	CLONE(PARENTHESES)
	EXEC
	{
		variable v = env.pop();
		variable r(variable::RARRAY);
		r.push(v);
		env.push(r);
		return RC::NONE;
	}
	GET(PARENTHESES)
	PSL_DUMP((int d){PSL_PRINTF(("PARENTHESES\n"));})
	WRITE(PARENTHESES,)
};

class CALL : public OpCode
{
public:
	CLONE(CALL)
	EXEC
	{
		variable r = env.pop();
		variable v = env.pop();
		rsv z;
		Type t = v.type();
		if (t == CMETHOD || t == METHOD)
			z = v[1];
/*		メソッドはthisを束縛しない為、メソッド呼び出しで末尾最適化がかかると
		スコープに割り付けられたローカルオブジェクト(this)が破棄されながらメソッドが呼び出されてしまう
		メソッドスコープはthisを束縛するので次のスコープを作ってしまえば問題ない
		その為ここで一時的にthisを保持しておく */
		env.push(r);
		#ifdef PSL_OPTIMIZE_TAILCALL
		MNEMONIC::mnemonic n = env.getNext();
		if (n == MNEMONIC::RETURN)	env.Return();
		else if (n == MNEMONIC::END)env.endScope();
		#endif
		v.prepare(env);
		return RC::CALL;
	}
	PSL_DUMP((int d){PSL_PRINTF(("CALL\n"));})
	WRITE(CALL,)
};

class RETURN : public OpCode
{
public:
	CLONE(RETURN)
	EXEC
	{
		env.Return();
		return RC::RETURN;
	}
	GET(RETURN)
	PSL_DUMP((int d){PSL_PRINTF(("RETURN\n"));})
	WRITE(RETURN,)
};
class BREAK : public OpCode
{
public:
	CLONE(BREAK)
	EXEC
	{
		env.Break();
		return RC::BREAK;
	}
	PSL_DUMP((int d){PSL_PRINTF(("BREAK\n"));})
	WRITE(BREAK,)
};
class CONTINUE : public OpCode
{
public:
	CLONE(CONTINUE)
	EXEC
	{
		env.Continue();
		return RC::CONTINUE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("CONTINUE\n"));})
	WRITE(CONTINUE,)
};
class YIELD : public OpCode
{
public:
	CLONE(YIELD)
	EXEC
	{
//		env.Yield();	// ってすることなくね？
		return RC::YIELD;
	}
	PSL_DUMP((int d){PSL_PRINTF(("YIELD\n"));})
	WRITE(YIELD,)
};
class GOTO : public OpCode
{
public:
	GOTO(string &s)	{label = s;}
	CLONE(GOTO(label))
	EXEC
	{
		env.Goto(label);
		return RC::GOTO;
	}
	PSL_DUMP((int d){PSL_PRINTF(("GOTO %s\n", label.c_str()));})
	WRITE(VARIABLE,b.push(label.c_str(), label.length()+1);)
private:
	string label;
};

class SCOPE : public OpCode
{
public:
	SCOPE(Code *c)	{statement = c->inc();}
	CLONE(SCOPE(statement))
	~SCOPE()	{statement->finalize();}
	EXEC
	{
		Scope *s = new AnonymousScope(statement);
		env.addScope(s);
		return RC::SCOPE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("SCOPE\n"));if (!d){statement->dump();PSL_PRINTF(("SCOPE END\n"));}})
	WRITE(SCOPE,statement->write(b);)
protected:
	Code *statement;
};
class LOOP : public OpCode
{
public:
	LOOP(Code *c, int l)	{statement = c->inc();cline = l;}
	CLONE(LOOP(statement, cline))
	~LOOP()	{statement->finalize();}
	EXEC
	{
		Scope *s = new LoopScope(statement, cline);
		env.addScope(s);
		return RC::LOOP;
	}
	PSL_DUMP((int d){PSL_PRINTF(("LOOP\n"));if (!d){statement->dump();PSL_PRINTF(("LOOP END\n"));}})
	WRITE(LOOP,b.push(&cline, sizeof(cline));statement->write(b);)
private:
	Code *statement;
	int cline;	// continue line
};
#ifdef PSL_DEBUG
class IF : public SCOPE
{
public:
	IF(Code *c):SCOPE(c){}
	CLONE(IF(statement))
	PSL_DUMP((int d){PSL_PRINTF(("IF\n"));if (!d){statement->dump();PSL_PRINTF(("IF END\n"));}})
	WRITE(IF,statement->write(b);)
};
#endif

class LOCAL : public OpCode
{
public:
	LOCAL(string &s)	{name = s;}
	CLONE(LOCAL(name))
	EXEC
	{
		variable v = env.pop();
		env.addLocal(name, v);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("LOCAL %s\n", name.c_str()));})
	WRITE(LOCAL,b.push(name.c_str(), name.length()+1);)
private:
	string name;
};
class GLOBAL : public OpCode
{
public:
	GLOBAL(string &s)	{name = s;}
	CLONE(GLOBAL(name))
	EXEC
	{
		variable v = env.pop();
		env.addGlobal(name, v);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("GLOBAL %s\n", name.c_str()));})
	WRITE(GLOBAL,b.push(name.c_str(), name.length()+1);)
private:
	string name;
};
class STATIC : public OpCode
{
public:
	STATIC(string &s)	{name = s;}
	CLONE(STATIC(name))
	EXEC
	{
		variable v = env.pop();
		env.addStatic(name, v);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("STATIC %s\n", name.c_str()));})
	WRITE(STATIC,b.push(name.c_str(), name.length()+1);)
private:
	string name;
};

class DECLARATION : public OpCode
{
public:
	DECLARATION(string &s)	{name = s;}
	CLONE(DECLARATION(name))
	EXEC
	{
		variable v = env.pop();
		env.Declaration(name, v);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("DECLARATION %s\n", name.c_str()));})
	WRITE(DECLARATION,b.push(name.c_str(), name.length()+1);)
private:
	string name;
};

class INSTANCE : public OpCode
{
public:
	CLONE(INSTANCE)
	EXEC
	{
		variable v = env.pop();
		v.prepareInstance(env);
		return RC::CALL;
	}
	PSL_DUMP((int d){PSL_PRINTF(("INSTANCE\n"));})
	WRITE(INSTANCE,)
};

class MEMBER : public OpCode
{
public:
	MEMBER(string &s)	{name = s;}
	CLONE(MEMBER(name))
	EXEC
	{
		variable v = env.pop();
		env.push(v[name]);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("MEMBER %s\n", name.c_str()));})
	WRITE(MEMBER,b.push(name.c_str(), name.length()+1);)
private:
	string name;
};
class INDEX : public OpCode
{
public:
	CLONE(INDEX)
	EXEC
	{
		variable i = env.pop();
		variable v = env.pop();
		env.push(v[i]);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("INDEX\n"));})
	WRITE(INDEX,)
};
class LOCALINDEX : public OpCode
{
public:
	LOCALINDEX(int i)	{x = i;}
	CLONE(LOCALINDEX(x))
	EXEC
	{
		env.push(env.getLocalIndex(x));
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("LOCALINDEX:%d\n", x));})
	WRITE(LOCALINDEX,b.push(&x, sizeof(x));)
private:
	int x;
};
#undef GET
#undef CLONE
#undef EXEC
#undef WRITE
