
class Parser
{
public:
	Parser(Tokenizer *to)
	{
		t = to;
		error = 0;
	}
	void Parse(variable &v)
	{
		while (t->checkNext())
			ParseStatement(v, v);
		if (error)
			std::printf("%d error in compile %s\n", error, t->getFile().c_str());
//			std::printf("%d error%s in compile %s\n", error, error == 1 ? "" : "s", t->getFile().c_str());
	}
	int getErrorNum()	{return error;}
private:
	enum ErrorID
	{
		_ERROR = 1,
		TINA,
		TINSC,
		TINLAG,
		TINIA,
		NOTT,
		UKT,
		TINCOT,
		TINAE,
		WARNING,
		IIAE,
		BINC,
	};
	void Error(ErrorID n, int op = 0, const char *str = "", int line = -1)
	{
		if (line < 0)
			line = t->getLine();
		if (n < WARNING)
		{
			++error;
			std::printf("error %s %d: ", t->getFile().c_str(), line);
		}
		else
		{
			std::printf("warning %s %d: ", t->getFile().c_str(), line);
		}
		switch (n)
		{
		case TINA:	std::printf("There is no %c after %s\n", op, str);break;
		case TINSC:	std::printf("There is no %s-statement condition\n", str);break;
		case TINLAG:std::printf("There is no label after goto\n");break;
		case NOTT:	std::printf("not term : %c\n", op);break;
		case UKT:	std::printf("unknown token\n");break;
		case TINIA:	std::printf("There is no identifier after %s\n", str);break;
		case TINCOT:std::printf("There is no : on ternary operation\n");break;
		case TINAE:	std::printf("There is no %c at expression end\n", op);break;
		case IIAE:	std::printf("'%s' is already exsit\n", str);break;
		case BINC:	std::printf("block not closed from %d\n", op);break;
		default:	std::printf("unknown error\n");
		}
	}
	void ParseDangling(variable &g, variable &c)
	{
		if (t->checkNext() == '{'/*'}'*/)
		{
			t->getNext();
			ParseBlock(g, c);
		}
		else
		{
			ParseStatement(g, c);
		}
	}
	void ParseIf(variable &g, variable &c)
	{
		variable v;
		if (t->getNext() != '('/*')'*/)
			Error(TINA, '('/*')'*/, "if");
		else
			ParseExpression(v, /*'('*/')');
		int l = v.codelength();
		variable::Variable::OpCode *oc = NULL;
		if (l)
		{
			oc = new variable::Variable::JF(l);
			v.pushcode(oc);	// 取り敢えず足す
		}
		else
		{
			Error(TINSC, 0, "if");
		}
		ParseDangling(g, v);
		if (t->checkNext() == Tokenizer::IDENTIFIER && t->nstr == "else")
		{
			t->getNext();
			if (oc)
				oc->set(v.codelength()+1);
			oc = new variable::Variable::JMP(0);
			v.pushcode(oc);
			ParseDangling(g, v);
			oc->set(v.codelength());
		}
		else
		{
			if (oc)
				oc->set(v.codelength());
		}
		if (variable::Variable::Code *x = v.getcode())
		{
			#ifdef PSL_DEBUG
			c.pushcode(new variable::Variable::IF(x));
			#else
			c.pushcode(new variable::Variable::SCOPE(x));
			#endif
		}
	}
	void ParseFor(variable &g, variable &c)
	{
		variable v;
		int l = 0;
		if (t->getNext() != '('/*')'*/)
			Error(TINA, '('/*')'*/, "for");
		if (t->checkNext() != ';')
		{
			ParseExpression(v, ';');
			l = v.codelength();
			if (l)
			{
				v.pushcode(new variable::Variable::POP);
				++l;
			}
		}
		else
		{
			t->getNext();
			// 初期化式が空
		}
		variable::Variable::OpCode *oc = NULL;
		if (t->checkNext() != ';')
		{
			ParseExpression(v, ';');
			if (v.codelength() - l)
			{
				oc = new variable::Variable::JF(v.codelength());
				v.pushcode(oc);
			}
		}
		else
		{
			t->getNext();	// 条件式が空
		}
		variable x;
		if (t->checkNext() != /*'('*/')')
			ParseExpression(x, /*'('*/')');
		ParseDangling(g, v);
		int cline = v.codelength();
		if (x.codelength())
		{
			v.getcode()->push(x.getcode());
			v.pushcode(new variable::Variable::POP);
		}
		v.pushcode(new variable::Variable::JMP(l));
		if (oc)
			oc->set(v.codelength());
		if (t->checkNext() == Tokenizer::IDENTIFIER && t->nstr == "else")
		{
			t->getNext();
			ParseDangling(g, v);
		}
		if (variable::Variable::Code *x = v.getcode())
			c.pushcode(new variable::Variable::LOOP(x, cline));
	}
	void ParseWhile(variable &g, variable &c)
	{
		variable v;
		if (t->getNext() != '('/*')'*/)
			Error(TINA, '('/*')'*/, "while");
		else
			ParseExpression(v, /*'('*/')');
		int l = v.codelength();
		variable::Variable::OpCode *oc = NULL;
		if (l)
		{
			oc = new variable::Variable::JF(l);
			v.pushcode(oc);	// 取り敢えず足す
		}
		else
			Error(TINSC, 0, "while");

		ParseDangling(g, v);

		v.pushcode(new variable::Variable::JMP(0));
		if (oc)
			oc->set(v.codelength());
		if (t->checkNext() == Tokenizer::IDENTIFIER && t->nstr == "else")
		{
			t->getNext();
			ParseDangling(g, v);
		}
		if (variable::Variable::Code *x = v.getcode())
			c.pushcode(new variable::Variable::LOOP(x, 0));
	}
	void ParseStatement(variable &g, variable &c)
	{
		int n = t->checkNext();
		if (n == ';')
		{
			t->getNext();
			return;	// 空行
		}
		#ifdef PSL_DEBUG
		c.pushlabel(t->getFile() + " " + t->getLine());
		#endif
		if (n == '{'/*'}'*/)
		{
			t->getNext();
			// 無名スコープ
			variable v;
			ParseBlock(g, v);
			if (v.codelength())
				c.pushcode(new variable::Variable::SCOPE(v.getcode()));
			return;
		}
		if (n == Tokenizer::IDENTIFIER)
		{
			if (t->nstr == "global" || t->nstr == "static" || t->nstr == "local" || t->nstr == "yield")
			{
				ParseExpression(c, ';');
				c.pushcode(new variable::Variable::POP);
				return;
			}
			t->getNext();
			if (t->nstr == "if")
			{
				ParseIf(g, c);
				return;
			}
			if (t->nstr == "for")
			{
				ParseFor(g, c);
				return;
			}
			if (t->nstr == "while")
			{
				ParseWhile(g, c);
				return;
			}
			if (t->nstr == "return")
			{
				if (t->checkNext() == ';')
					c.pushcode(new variable::Variable::PUSH_NULL);
				else
					ParseExpression(c, ';');
				c.pushcode(new variable::Variable::RETURN);
				return;
			}
			if (t->nstr == "continue")
			{
				if (t->checkNext() != ';')	Error(TINA, ';', "continue");
				else	t->getNext();
				c.pushcode(new variable::Variable::CONTINUE);
				return;
			}
			if (t->nstr == "break")
			{
				if (t->checkNext() != ';')	Error(TINA, ';', "break");
				else	t->getNext();
				c.pushcode(new variable::Variable::BREAK);
				return;
			}
			if (t->nstr == "goto")
			{
				if (t->checkNext() != Tokenizer::IDENTIFIER)
					Error(TINLAG);
				else
				{
					t->getNext();
					string label = t->nstr;
					if (t->checkNext() != ';')
						Error(TINA, ';', "goto");
					else
						t->getNext();
					c.pushcode(new variable::Variable::GOTO(label));
				}
				return;
			}

			string name = t->nstr;
			int line = t->getLine();
			n = t->checkNext();
			if (n == ':')	// label
			{
				t->getNext();
				c.pushlabel(name);
				return;
			}
			if (n == '{'/*'}'*/)	// クラス定義
			{
				t->getNext();
				if (g.exist(name))
					Error(IIAE, 0, name, line);
				variable m = g[name];
				if (!m.getcode())
					m.pushcode(new variable::Variable::POP);
				ParseBlock(m, m);
				return;
			}
			if (n == '('/*')'*/)
			{
				variable arg;
				t->getNext();
				n = t->checkNext();
				if (n != /*'('*/')')
				{
					ParseExpression(arg, /*'('*/')');
				}
				else
				{
					t->getNext();
					arg.pushcode(new variable::Variable::PUSH_NULL);
				}
				n = t->checkNext();
				if (n == '{'/*'}'*/ || n == Tokenizer::IDENTIFIER)	// 関数定義
				{
					if (g.exist(name))
						Error(IIAE, 0, name, line);
					g[name] = arg;
					variable m = g[name];
					g[name].pushcode(new variable::Variable::ARGUMENT);
					if (n == '{'/*'}'*/)
					{
						t->getNext();
						ParseBlock(m, m);
					}
					else if (n == Tokenizer::IDENTIFIER)
						ParseStatement(m, m);
					return;
				}
				else	// 関数呼び出し
				{
					c.pushcode(new variable::Variable::VARIABLE(name));
					c.getcode()->push(arg.getcode());
					c.pushcode(new variable::Variable::CALL);
				}
			}
			else if (n == Tokenizer::IDENTIFIER)
			{
				t->getNext();
				c.pushcode(new variable::Variable::VARIABLE(name));
				c.pushcode(new variable::Variable::INSTANCE);
				c.pushcode(new variable::Variable::DECLARATION(t->nstr));
			}
			else
			{
				c.pushcode(new variable::Variable::VARIABLE(name));
			}
			getSuffOp(c);
			ParseExpression(c, ';', true);
			c.pushcode(new variable::Variable::POP);
		}
		else
		{
			ParseExpression(c, ';');
			c.pushcode(new variable::Variable::POP);
		}
	}
	void ParseBlock(variable &g, variable &c)
	{
		int line = t->getLine();
		int n;
		while ((n = t->checkNext()) != 0)
		{
			if (n == /*'{'*/'}')
				break;
			ParseStatement(g, c);
		}
		if (!n)
			Error(BINC, line);
		t->getNext();
	}
	void getSuffOp(variable &c)
	{
		while (int n = t->checkNext())
		{
			if (n == Tokenizer::INC)
			{
				t->getNext();
				c.pushcode(new variable::Variable::INC);
			}
			else if (n == Tokenizer::DEC)
			{
				t->getNext();
				c.pushcode(new variable::Variable::DEC);
			}
			else if (n == '('/*')'*/)
			{
				t->getNext();
				if (t->checkNext() != /*'('*/')')
				{
					ParseExpression(c, /*'('*/')');
				}
				else
				{
					t->getNext();	// 引数無し
					c.pushcode(new variable::Variable::PUSH_NULL);
				}
				c.pushcode(new variable::Variable::CALL);
			}
			else if (n == '[')
			{
				t->getNext();
				ParseExpression(c, ']');
				c.pushcode(new variable::Variable::INDEX);
			}
			else if (n == '.')
			{
				t->getNext();
				if (t->checkNext() == Tokenizer::IDENTIFIER)
				{
					t->getNext();
					c.pushcode(new variable::Variable::MEMBER(t->nstr));
				}
				else
				{
					Error(TINIA, 0, "member access (.)");
				}
			}
			else
				break;
		}
	}
	#define PRE_OP(m,o) if(n==m){t->getNext();getTerm(c);c.pushcode(new variable::Variable::o);}
	#define TERM(m,o) if(n==m){t->getNext();c.pushcode(new variable::Variable::o);getSuffOp(c);}
	void getTerm(variable &c)
	{
		int n = t->checkNext();
		if (n == '('/*')'*/)
		{
			t->getNext();
			variable v;
			if (t->checkNext() == /*'('*/')')	// 即閉じは確実に無名関数
			{
				t->getNext();
				v.pushcode(new variable::Variable::PUSH_NULL);
			}
			else
			{
				ParseExpression(v, /*'('*/')');
			}
			if (t->checkNext() == '{'/*'}'*/)
			{
				t->getNext();
				v.pushcode(new variable::Variable::ARGUMENT);
				ParseBlock(v, v);
				if (v.codelength())
					c.pushcode(new variable::Variable::PUSH_CODE(v.getcode()));
			}
			else	// 式内の括弧
			{
				if (v.codelength())
				{
					if (c.codelength())
						c.getcode()->push(v.getcode());
					else
						c = v;
					c.pushcode(new variable::Variable::PARENTHESES);
				}
			}
			getSuffOp(c);
		}
		else PRE_OP('+', PLUS)
		else PRE_OP('-', MINUS)
		else PRE_OP('!', NOT)
		else PRE_OP('~', COMPL)
		else PRE_OP('*', DEREF)
		else PRE_OP('&', REF)
		else PRE_OP(Tokenizer::INC, PINC)
		else PRE_OP(Tokenizer::DEC, PDEC)
		else TERM(Tokenizer::IDENTIFIER, VARIABLE(t->nstr))
		else TERM(Tokenizer::INT, PUSH_INT(t->nint))
		else TERM(Tokenizer::HEX, PUSH_HEX(t->nint))
		else TERM(Tokenizer::NUMBER, PUSH_FLOAT(t->nnum))
		else TERM(Tokenizer::STRING, PUSH_STRING(t->nstr))
		else
		{
			if (n < 0)	Error(UKT);
			else		Error(NOTT, n);
			t->getNext();
			return;
		}
	}
	#undef PRE_OP
	#undef TERM
	#define EXP0(m,e,o) if(n==m){t->getNext();get##e(c);c.pushcode(new variable::Variable::o);}
	#define EXP(m,e,o) EXP0(m,exp##e,o)
	void getexp1(variable &c, bool l = false)
	{
		if (!l)
			getTerm(c);
		while (int n = t->checkNext())
		{
			EXP0('*', Term, MUL)
			else EXP0('/', Term, DIV)
			else EXP0('%', Term, MOD)
			else
				break;
		}
	}
	void getexp2(variable &c, bool l = false)
	{
		getexp1(c, l);
		while (int n = t->checkNext())
		{
			EXP('+', 1, ADD)
			else EXP('-', 1, SUB)
			else
				break;
		}
	}
	void getexp3(variable &c, bool l = false)
	{
		getexp2(c, l);
		while (int n = t->checkNext())
		{
			EXP(Tokenizer::SHL, 2, SHL)
			else EXP(Tokenizer::SHR, 2, SHR)
			else
				break;
		}
	}
	void getexp4(variable &c, bool l = false)
	{
		getexp3(c, l);
		while (int n = t->checkNext())
		{
			EXP('&', 3, AND)
			else
				break;
		}
	}
	void getexp5(variable &c, bool l = false)
	{
		getexp4(c, l);
		while (int n = t->checkNext())
		{
			EXP('^', 4, XOR)
			else
				break;
		}
	}
	void getexp6(variable &c, bool l = false)
	{
		getexp5(c, l);
		while (int n = t->checkNext())
		{
			EXP('|', 5, OR)
			else
				break;
		}
	}
	void getexp7(variable &c, bool l = false)
	{
		getexp6(c, l);
		while (int n = t->checkNext())
		{
			EXP('<', 6, LT)
			else EXP(Tokenizer::LE, 6, LE)
			else EXP('>', 6, GT)
			else EXP(Tokenizer::GE, 6, GE)
			else
				break;
		}
	}
	void getexp8(variable &c, bool l = false)
	{
		getexp7(c, l);
		while (int n = t->checkNext())
		{
			EXP(Tokenizer::EQ, 7, EQ)
			else EXP(Tokenizer::NE, 7, NEQ)
			else
				break;
		}
	}
	void getexp9(variable &c, bool l = false)
	{
		getexp8(c, l);
		while (int n = t->checkNext())
		{
			#if defined(PSL_OPTIMIZE_BOOL_AND) && defined(PSL_OPTIMIZE_IN_COMPILE)
			if (n==Tokenizer::BAND)
			{
				t->getNext();
				variable::Variable::OpCode *oc = new variable::Variable::JRF(0);
				c.pushcode(oc);
				int b = c.codelength();
				getexp8(c);
				oc->set(c.codelength()+1 - b);
				c.pushcode(new variable::Variable::JR(1));
				c.pushcode(new variable::Variable::PUSH_NULL);
			}
			#else
			EXP(Tokenizer::BAND, 8, BAND)
			#endif
			else
				break;
		}
	}
	void getexp10(variable &c, bool l = false)
	{
		getexp9(c, l);
		while (int n = t->checkNext())
		{
			#if defined(PSL_OPTIMIZE_BOOL_AND) && defined(PSL_OPTIMIZE_IN_COMPILE)
			if (n==Tokenizer::BOR)
			{
				t->getNext();
				variable::Variable::OpCode *oc = new variable::Variable::JRT(0);
				c.pushcode(oc);
				int b = c.codelength();
				getexp9(c);
				oc->set(c.codelength()+1 - b);
				c.pushcode(new variable::Variable::JR(1));
				c.pushcode(new variable::Variable::PUSH_INT(1));
			}
			#else
			EXP(Tokenizer::BOR, 9, BOR)
			#endif
			else
				break;
		}
	}
	void getexp11(variable &c, bool l = false)
	{
		if (!l && t->checkNext() == Tokenizer::IDENTIFIER)
		{
			bool _global = false;
			bool _static = false;
			bool _local = false;
			if (t->nstr == "global")
				_global = true;
			else if (t->nstr == "static")
				_static = true;
			else if (t->nstr == "local")
				_local = true;
			if (_global || _static || _local)
//			if ((_global = (t->nstr == "global")) || (_static = (t->nstr == "static")) || (_local = (t->nstr == "local")))	// 警告出るねん
			{
				string scope = t->nstr;
				t->getNext();
				if (t->checkNext() == Tokenizer::IDENTIFIER)
				{
					t->getNext();
					string name = t->nstr;
					if (t->checkNext() == Tokenizer::IDENTIFIER)
					{
						t->getNext();
						l = true;
						c.pushcode(new variable::Variable::VARIABLE(name));
						c.pushcode(new variable::Variable::INSTANCE);
						if (_global)		c.pushcode(new variable::Variable::GLOBAL(t->nstr));
						else if (_static)	c.pushcode(new variable::Variable::STATIC(t->nstr));
						else if (_local)	c.pushcode(new variable::Variable::LOCAL(t->nstr));
					}
					else
					{
						l = true;
						c.pushcode(new variable::Variable::PUSH_NULL());
						if (_global)		c.pushcode(new variable::Variable::GLOBAL(name));
						else if (_static)	c.pushcode(new variable::Variable::STATIC(name));
						else if (_local)	c.pushcode(new variable::Variable::LOCAL(name));
					}
				}
				else
				{
					Error(TINIA, 0, scope);
				}
			}
			else
			{
				t->getNext();
				string name = t->nstr;
				if (t->checkNext() == Tokenizer::IDENTIFIER)
				{
					t->getNext();
					l = true;
					c.pushcode(new variable::Variable::VARIABLE(name));
					c.pushcode(new variable::Variable::INSTANCE);
					c.pushcode(new variable::Variable::DECLARATION(t->nstr));
				}
				else
				{
					l = true;
					c.pushcode(new variable::Variable::VARIABLE(name));
				}
			}
			if (l)
				getSuffOp(c);
		}
		getexp10(c, l);
		if (t->checkNext() == '?')
		{
			t->getNext();
			variable::Variable::OpCode *oc = new variable::Variable::JRF(0);
			c.pushcode(oc);
			int b = c.codelength();
			getexp11(c);
			oc->set(c.codelength()+1 - b);
			oc = new variable::Variable::JR(0);
			c.pushcode(oc);
			b = c.codelength();
			if (t->checkNext() != ':')	Error(TINCOT);
			else						t->getNext();
			getexp11(c);
			oc->set(c.codelength() - b);
		}
	}
	void getexp12(variable &c, bool l = false)
	{
		getexp11(c, l);
		while (int n = t->checkNext())
		{
			EXP(',', 11, LIST)
			else
				break;
		}
	}
	void getexp13(variable &c, bool l = false)
	{
		if (!l && t->checkNext() == Tokenizer::IDENTIFIER && t->nstr == "yield")
		{
			t->getNext();
			if (t->checkNext() == ';')
				c.pushcode(new variable::Variable::PUSH_NULL);
			else
				getexp13(c);
			c.pushcode(new variable::Variable::YIELD);
		}
		else
			getexp12(c, l);
	}
	void getexp14(variable &c, bool l = false)
	{
		getexp13(c, l);
		while (int n = t->checkNext())
		{
			EXP('=', 13, SUBSTITUTION)
			else EXP(Tokenizer::ASSIGN, 14, ASSIGNMENT)
			else EXP(Tokenizer::SADD, 14, SADD)
			else EXP(Tokenizer::SSUB, 14, SSUB)
			else EXP(Tokenizer::SMUL, 14, SMUL)
			else EXP(Tokenizer::SDIV, 14, SDIV)
			else EXP(Tokenizer::SMOD, 14, SMOD)
			else EXP(Tokenizer::SAND, 14, SAND)
			else EXP(Tokenizer::SOR, 14, SOR)
			else EXP(Tokenizer::SXOR, 14, SXOR)
			else EXP(Tokenizer::SSHL, 14, SSHL)
			else EXP(Tokenizer::SSHR, 14, SSHR)
			else
				break;
		}
	}
	void ParseExpression(variable &c, char e, bool l = false)
	{
		getexp14(c, l);
		if (t->checkNext() != e)
			Error(TINAE, e, "", t->getPrevLine());
		else
			t->getNext();
	}
private:
	Tokenizer *t;
	int error;
};
