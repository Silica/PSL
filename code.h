#define GET(n) MNEMONIC::mnemonic get(){return MNEMONIC::n;}
class PUSH_INT : public OpCode
{
public:
	PUSH_INT(int i):x(new Variable(i),0){}
	OpCode *clone()	{return new PUSH_INT(x.get()->toInt());}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.push(x);
		return RC::NONE;
	}
	GET(CONSTANT)
	PSL_DUMP((int d){PSL_PRINTF(("PUSH %d\n", x.get()->toInt()));})
	void write(bytecode &b){b.push(MNEMONIC::PUSH_INT);int w = x.get()->toInt();b.push(&w, sizeof(w));}
private:
	rsv x;
};
class PUSH_HEX : public OpCode
{
public:
	PUSH_HEX(hex l):x(new Variable(new vHex(l)),0){}
	OpCode *clone()	{return new PUSH_HEX(x.get()->toInt());}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.push(x);
		return RC::NONE;
	}
	GET(CONSTANT)
	PSL_DUMP((int d){PSL_PRINTF(("PUSH %X\n", x.get()->toInt()));})
	void write(bytecode &b){b.push(MNEMONIC::PUSH_HEX);hex w = x.get()->toInt();b.push(&w, sizeof(w));}
private:
	rsv x;
};
class PUSH_FLOAT : public OpCode
{
public:
	PUSH_FLOAT(double d):x(new Variable(d),0){}
	OpCode *clone()	{return new PUSH_FLOAT(x.get()->toDouble());}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.push(x);
		return RC::NONE;
	}
	GET(CONSTANT)
	PSL_DUMP((int d){PSL_PRINTF(("PUSH %f\n", x.get()->toDouble()));})
	void write(bytecode &b){b.push(MNEMONIC::PUSH_FLOAT);double w = x.get()->toDouble();b.push(&w, sizeof(w));}
private:
	rsv x;
};
class PUSH_STRING : public OpCode
{
public:
	PUSH_STRING(string &s):x(new Variable(s),0){}
	OpCode *clone()	{string s = x.get()->toString();return new PUSH_STRING(s);}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.push(x);
		return RC::NONE;
	}
	GET(CONSTANT)
	PSL_DUMP((int d){PSL_PRINTF(("PUSH %s\n", x.get()->toString().c_str()));})
	void write(bytecode &b){b.push(MNEMONIC::PUSH_STRING);string w = x.get()->toString();b.push(w.c_str(), w.length()+1);}
private:
	rsv x;
};
class POP : public OpCode
{
public:
	OpCode *clone()	{return new POP;}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.pop();
		return RC::NONE;
	}
	GET(POP)
	PSL_DUMP((int d){PSL_PRINTF(("POP\n"));})
	void write(bytecode &b){b.push(MNEMONIC::POP);}
};
class PUSH_NULL : public OpCode
{
public:
	OpCode *clone()	{return new PUSH_NULL;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v;
		env.push(v);
		return RC::NONE;
	}
	GET(CONSTANT)
	PSL_DUMP((int d){PSL_PRINTF(("PUSH NULL\n"));})
	void write(bytecode &b){b.push(MNEMONIC::PUSH_NULL);}
};
class POP_VARIABLE : public OpCode
{
public:
	OpCode *clone()	{return new POP_VARIABLE(name);}
	POP_VARIABLE(string &s)	{name = s;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.getVariable(name);
		v = env.pop();
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("POP VARIABLE %s\n", name.c_str()));})
private:
	string name;
};

class SUBSTITUTION : public OpCode
{
public:
	OpCode *clone()	{return new SUBSTITUTION;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable r = env.pop();
		variable l = env.top();
		l.substitution(r);
		return RC::NONE;
	}
	GET(BINARY)
	PSL_DUMP((int d){PSL_PRINTF(("SUBSTITUTION\n"));})
	void write(bytecode &b){b.push(MNEMONIC::SUBSTITUTION);}
};
class ASSIGNMENT : public OpCode
{
public:
	OpCode *clone()	{return new ASSIGNMENT;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable r = env.pop();
		variable l = env.top();
		l.assignment(r);
		return RC::NONE;
	}
	GET(BINARY)
	PSL_DUMP((int d){PSL_PRINTF(("ASSIGNMENT\n"));})
	void write(bytecode &b){b.push(MNEMONIC::ASSIGNMENT);}
};
class ARGUMENT : public OpCode
{
public:
	OpCode *clone()	{return new ARGUMENT;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable l = env.pop();
		variable r = env.pop();
		env.push(l.substitution(r));
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("ARGUMENT\n"));})
	void write(bytecode &b){b.push(MNEMONIC::ARGUMENT);}
};


class PLUS : public OpCode
{
public:
	OpCode *clone()	{return new PLUS;}
	RC::RETURNCODE Execute(Environment &env)
	{
		return RC::NONE;
	}
	GET(PLUS)
	PSL_DUMP((int d){PSL_PRINTF(("PLUS\n"));})
	void write(bytecode &b){b.push(MNEMONIC::PLUS);}
};
class MINUS : public OpCode
{
public:
	OpCode *clone()	{return new MINUS;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		env.push(-v);
		return RC::NONE;
	}
	GET(UNARY)
	PSL_DUMP((int d){PSL_PRINTF(("MINUS\n"));})
	void write(bytecode &b){b.push(MNEMONIC::MINUS);}
};
class NOT : public OpCode
{
public:
	OpCode *clone()	{return new NOT;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		variable x = !(bool)v;
		env.push(x);
		return RC::NONE;
	}
	GET(UNARY)
	PSL_DUMP((int d){PSL_PRINTF(("NOT\n"));})
	void write(bytecode &b){b.push(MNEMONIC::NOT);}
};
class COMPL : public OpCode
{
public:
	OpCode *clone()	{return new COMPL;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		variable x(~(int)v);
		env.push(x);
		return RC::NONE;
	}
	GET(UNARY)
	PSL_DUMP((int d){PSL_PRINTF(("COMPL\n"));})
	void write(bytecode &b){b.push(MNEMONIC::COMPL);}
};
class INC : public OpCode	// 後置
{
public:
	OpCode *clone()	{return new INC;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		variable c = v;	// 値をコピー
		env.push(c);
		v += 1;
		return RC::NONE;
	}
	GET(INC)
	PSL_DUMP((int d){PSL_PRINTF(("INC\n"));})
	void write(bytecode &b){b.push(MNEMONIC::INC);}
};
class PINC : public OpCode	// 前置
{
public:
	OpCode *clone()	{return new PINC;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.top();
		v += 1;
		return RC::NONE;
	}
	GET(UNARY)
	PSL_DUMP((int d){PSL_PRINTF(("PINC\n"));})
	void write(bytecode &b){b.push(MNEMONIC::PINC);}
};
class DEC : public OpCode	// 後置
{
public:
	OpCode *clone()	{return new DEC;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		variable c = v;	// 値をコピー
		env.push(c);
		v -= 1;
		return RC::NONE;
	}
	GET(DEC)
	PSL_DUMP((int d){PSL_PRINTF(("DEC\n"));})
	void write(bytecode &b){b.push(MNEMONIC::DEC);}
};
class PDEC : public OpCode	// 前置
{
public:
	OpCode *clone()	{return new PDEC;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.top();
		v -= 1;
		return RC::NONE;
	}
	GET(UNARY)
	PSL_DUMP((int d){PSL_PRINTF(("PDEC\n"));})
	void write(bytecode &b){b.push(MNEMONIC::PDEC);}
};
class DEREF : public OpCode
{
public:
	OpCode *clone()	{return new DEREF;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		env.push(*v);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("DEREF\n"));})
	void write(bytecode &b){b.push(MNEMONIC::DEREF);}
};
class REF : public OpCode
{
public:
	OpCode *clone()	{return new REF;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		env.push(v.pointer());
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("REF\n"));})
	void write(bytecode &b){b.push(MNEMONIC::REF);}
};
#define OOC(n,o) class n:public OpCode{public:OpCode*clone(){return new n;}RC::RETURNCODE Execute(Environment&env){variable r=env.pop();variable l=env.top();l o r;return RC::NONE;}MNEMONIC::mnemonic get(){return MNEMONIC::BINARY;}PSL_DUMP((int d){PSL_PRINTF((#n"\n"));})void write(bytecode &b){b.push(MNEMONIC::n);}}
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
#define OOC(n,o) class n:public OpCode{public:OpCode*clone(){return new n;}RC::RETURNCODE Execute(Environment&env){variable r=env.pop();variable l=env.pop();variable x(l o r);env.push(x);return RC::NONE;}MNEMONIC::mnemonic get(){return MNEMONIC::BINARY;}PSL_DUMP((int d){PSL_PRINTF((#n"\n"));})void write(bytecode &b){b.push(MNEMONIC::n);}}
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
	OpCode *clone()	{return new VARIABLE(name);}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.getVariable(name);
		env.push(v);
		return RC::NONE;
	}
	GET(VARIABLE)
	PSL_DUMP((int d){PSL_PRINTF(("VARIABLE %s\n", name.c_str()));})
	void write(bytecode &b){b.push(MNEMONIC::VARIABLE);b.push(name.c_str(), name.length()+1);}
private:
	string name;
};

class BAND : public OpCode
{
public:
	OpCode *clone()	{return new BAND;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable r = env.pop();
		variable l = env.pop();
		variable v((bool)l && (bool)r);
		env.push(v);
		return RC::NONE;
	}
	GET(BINARY)
	PSL_DUMP((int d){PSL_PRINTF(("BAND\n"));})
	void write(bytecode &b){b.push(MNEMONIC::BAND);}
};
class BOR : public OpCode
{
public:
	OpCode *clone()	{return new BOR;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable r = env.pop();
		variable l = env.pop();
		variable v((bool)l || (bool)r);
		env.push(v);
		return RC::NONE;
	}
	GET(BINARY)
	PSL_DUMP((int d){PSL_PRINTF(("BOR\n"));})
	void write(bytecode &b){b.push(MNEMONIC::BOR);}
};



class PUSH_CODE : public OpCode
{
public:
	PUSH_CODE(const rsv &v):x(v){}
	OpCode *clone()	{return new PUSH_CODE(x);}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = x;		// 一旦受け取り
		variable c = v;		// コピー作成
		env.push(c);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("PUSH CODE\n"));if (!d){x.get()->dump();PSL_PRINTF(("CODE END\n"));}})
	void write(bytecode &b){b.push(MNEMONIC::PUSH_CODE);x.get()->write("", b);}
private:
	rsv x;
};
class CLOSURE : public OpCode
{
public:
	OpCode *clone()	{return new CLOSURE;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.top();
		v = env.getLocal();	// ローカル変数受け取り
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("CLOSURE\n"));})
	void write(bytecode &b){b.push(MNEMONIC::CLOSURE);}
};


class JMP : public OpCode
{
public:
	JMP(int i)	{j = i;}
	OpCode *clone()	{return new JMP(j);}
	void set(int s){j = s;}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.Jump(j);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("JMP %d\n", j));})
	void write(bytecode &b){b.push(MNEMONIC::JMP);b.push(&j, sizeof(j));}
private:
	int j;
};
class JT : public OpCode
{
public:
	JT(int i)	{j = i;}
	OpCode *clone()	{return new JT(j);}
	void set(int s){j = s;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		if ((bool)v)
			env.Jump(j);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("JT %d\n", j));})
	void write(bytecode &b){b.push(MNEMONIC::JT);b.push(&j, sizeof(j));}
private:
	int j;
};
class JF : public OpCode
{
public:
	JF(int i)	{j = i;}
	OpCode *clone()	{return new JF(j);}
	void set(int s){j = s;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		if (!v)
			env.Jump(j);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("JF %d\n", j));})
	void write(bytecode &b){b.push(MNEMONIC::JF);b.push(&j, sizeof(j));}
private:
	int j;
};
class JR : public OpCode		// 相対ジャンプ、行は実行時には次を指していることに注意
{
public:
	JR(int i)	{j = i;}
	OpCode *clone()	{return new JR(j);}
	void set(int s){j = s;}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.RJump(j);
		return RC::NONE;
	}
	GET(JR)
	PSL_DUMP((int d){PSL_PRINTF(("JR %d\n", j));})
	void write(bytecode &b){b.push(MNEMONIC::JR);b.push(&j, sizeof(j));}
private:
	int j;
};
class JRT : public OpCode
{
public:
	JRT(int i)	{j = i;}
	OpCode *clone()	{return new JRT(j);}
	void set(int s){j = s;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		if ((bool)v)
			env.RJump(j);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("JRT %d\n", j));})
	void write(bytecode &b){b.push(MNEMONIC::JRT);b.push(&j, sizeof(j));}
private:
	int j;
};
class JRF : public OpCode
{
public:
	JRF(int i)	{j = i;}
	OpCode *clone()	{return new JRF(j);}
	void set(int s){j = s;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		if (!v)
			env.RJump(j);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("JRF %d\n", j));})
	void write(bytecode &b){b.push(MNEMONIC::JRF);b.push(&j, sizeof(j));}
private:
	int j;
};

class LIST : public OpCode
{
public:
	OpCode *clone()	{return new LIST;}
	RC::RETURNCODE Execute(Environment &env)
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
	void write(bytecode &b){b.push(MNEMONIC::LIST);}
};

class PARENTHESES : public OpCode
{
public:
	OpCode *clone()	{return new PARENTHESES;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		variable r(variable::RARRAY);
		r.push(v);
		env.push(r);
		return RC::NONE;
	}
	GET(PARENTHESES)
	PSL_DUMP((int d){PSL_PRINTF(("PARENTHESES\n"));})
	void write(bytecode &b){b.push(MNEMONIC::PARENTHESES);}
};

class CALL : public OpCode
{
public:
	OpCode *clone()	{return new CALL;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable r = env.pop();
		variable v = env.pop();
		env.push(r);
		#ifdef PSL_OPTIMIZE_TAILCALL
		MNEMONIC::mnemonic n = env.getNext();
		if (n == MNEMONIC::RETURN)	env.Return();	// 末尾最適化
		else if (n == MNEMONIC::END)env.endScope();
		#endif
		v.prepare(env);
		return RC::CALL;
	}
	PSL_DUMP((int d){PSL_PRINTF(("CALL\n"));})
	void write(bytecode &b){b.push(MNEMONIC::CALL);}
};

class RETURN : public OpCode
{
public:
	OpCode *clone()	{return new RETURN;}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.Return();
		return RC::RETURN;
	}
	GET(RETURN)
	PSL_DUMP((int d){PSL_PRINTF(("RETURN\n"));})
	void write(bytecode &b){b.push(MNEMONIC::RETURN);}
};
class BREAK : public OpCode
{
public:
	OpCode *clone()	{return new BREAK;}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.Break();
		return RC::BREAK;
	}
	PSL_DUMP((int d){PSL_PRINTF(("BREAK\n"));})
	void write(bytecode &b){b.push(MNEMONIC::BREAK);}
};
class CONTINUE : public OpCode
{
public:
	OpCode *clone()	{return new CONTINUE;}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.Continue();
		return RC::CONTINUE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("CONTINUE\n"));})
	void write(bytecode &b){b.push(MNEMONIC::CONTINUE);}
};
class YIELD : public OpCode
{
public:
	OpCode *clone()	{return new YIELD;}
	RC::RETURNCODE Execute(Environment &env)
	{
//		env.Yield();	// ってすることなくね？
		return RC::YIELD;
	}
	PSL_DUMP((int d){PSL_PRINTF(("YIELD\n"));})
	void write(bytecode &b){b.push(MNEMONIC::YIELD);}
};
class GOTO : public OpCode
{
public:
	GOTO(string &s)	{label = s;}
	OpCode *clone()	{return new GOTO(label);}
	RC::RETURNCODE Execute(Environment &env)
	{
		env.Goto(label);
		return RC::GOTO;
	}
	PSL_DUMP((int d){PSL_PRINTF(("GOTO %s\n", label.c_str()));})
	void write(bytecode &b){b.push(MNEMONIC::VARIABLE);b.push(label.c_str(), label.length()+1);}
private:
	string label;
};

class SCOPE : public OpCode
{
public:
	SCOPE(Code *c)	{statement = c->inc();}
	OpCode *clone()	{return new SCOPE(statement);}
	~SCOPE()	{statement->finalize();}
	RC::RETURNCODE Execute(Environment &env)
	{
		Scope *s = new AnonymousScope(statement);
		env.addScope(s);
		return RC::SCOPE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("SCOPE\n"));if (!d){statement->dump();PSL_PRINTF(("SCOPE END\n"));}})
	void write(bytecode &b){b.push(MNEMONIC::SCOPE);statement->write(b);}
protected:
	Code *statement;
};
class LOOP : public OpCode
{
public:
	LOOP(Code *c, int l)	{statement = c->inc();cline = l;}
	OpCode *clone()	{return new LOOP(statement, cline);}
	~LOOP()	{statement->finalize();}
	RC::RETURNCODE Execute(Environment &env)
	{
		Scope *s = new LoopScope(statement, cline);
		env.addScope(s);
		return RC::LOOP;
	}
	PSL_DUMP((int d){PSL_PRINTF(("LOOP\n"));if (!d){statement->dump();PSL_PRINTF(("LOOP END\n"));}})
	void write(bytecode &b){b.push(MNEMONIC::LOOP);b.push(&cline, sizeof(cline));statement->write(b);}
private:
	Code *statement;
	int cline;	// continue line
};
#ifdef PSL_DEBUG
class IF : public SCOPE
{
public:
	IF(Code *c):SCOPE(c){}
	OpCode *clone()	{return new IF(statement);}
	PSL_DUMP((int d){PSL_PRINTF(("IF\n"));if (!d){statement->dump();PSL_PRINTF(("IF END\n"));}})
	void write(bytecode &b){b.push(MNEMONIC::IF);statement->write(b);}
};
#endif

class LOCAL : public OpCode
{
public:
	LOCAL(string &s)	{name = s;}
	OpCode *clone()	{return new LOCAL(name);}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		env.addLocal(name, v);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("LOCAL %s\n", name.c_str()));})
	void write(bytecode &b){b.push(MNEMONIC::LOCAL);b.push(name.c_str(), name.length()+1);}
private:
	string name;
};
class GLOBAL : public OpCode
{
public:
	GLOBAL(string &s)	{name = s;}
	OpCode *clone()	{return new GLOBAL(name);}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		env.addGlobal(name, v);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("GLOBAL %s\n", name.c_str()));})
	void write(bytecode &b){b.push(MNEMONIC::GLOBAL);b.push(name.c_str(), name.length()+1);}
private:
	string name;
};
class STATIC : public OpCode
{
public:
	STATIC(string &s)	{name = s;}
	OpCode *clone()	{return new STATIC(name);}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		env.addStatic(name, v);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("STATIC %s\n", name.c_str()));})
	void write(bytecode &b){b.push(MNEMONIC::STATIC);b.push(name.c_str(), name.length()+1);}
private:
	string name;
};

class DECLARATION : public OpCode
{
public:
	DECLARATION(string &s)	{name = s;}
	OpCode *clone()	{return new DECLARATION(name);}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		env.Declaration(name, v);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("DECLARATION %s\n", name.c_str()));})
	void write(bytecode &b){b.push(MNEMONIC::DECLARATION);b.push(name.c_str(), name.length()+1);}
private:
	string name;
};

class INSTANCE : public OpCode
{
public:
	OpCode *clone()	{return new INSTANCE;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		v.prepareInstance(env);
		return RC::CALL;
	}
	PSL_DUMP((int d){PSL_PRINTF(("INSTANCE\n"));})
	void write(bytecode &b){b.push(MNEMONIC::INSTANCE);}
};
/*
labelをコード中に埋め込むという案もあった
class LABEL : public OpCode
{
public:
	LABEL(string &s)	{name = s;}
	RC::RETURNCODE Execute(Environment &env)
	{
		// なにもしない
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("LABEL %s\n", name.c_str()));})
	void write(bytecode &b){b.push(MNEMONIC::LABEL);b.push(name.c_str(), name.length()+1);}
private:
	string name;
};
*/


class MEMBER : public OpCode
{
public:
	MEMBER(string &s)	{name = s;}
	OpCode *clone()	{return new MEMBER(name);}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable v = env.pop();
		variable x = v[name];
		env.push(x);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("MEMBER %s\n", name.c_str()));})
	void write(bytecode &b){b.push(MNEMONIC::MEMBER);b.push(name.c_str(), name.length()+1);}
private:
	string name;
};
class INDEX : public OpCode
{
public:
	OpCode *clone()	{return new INDEX;}
	RC::RETURNCODE Execute(Environment &env)
	{
		variable i = env.pop();
		variable v = env.pop();
		env.push(v[i]);
		return RC::NONE;
	}
	PSL_DUMP((int d){PSL_PRINTF(("INDEX\n"));})
	void write(bytecode &b){b.push(MNEMONIC::INDEX);}
};
#undef GET
