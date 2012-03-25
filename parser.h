
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
		string arg = "arg";
		v.pushcode(new Variable::VARIABLE(arg));
		v.pushcode(new Variable::ARGUMENT);
		while (t->checkNext())
			ParseStatement(v, v);
		if (error)
			PSL_PRINTF(("%d error in compile %s\n", error, t->getFile().c_str()));
//			PSL_PRINTF(("%d error%s in compile %s\n", error, error == 1 ? "" : "s", t->getFile().c_str()));
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
			PSL_PRINTF(("error %s %d: ", t->getFile().c_str(), line));
		}
		else
		{
			PSL_PRINTF(("warning %s %d: ", t->getFile().c_str(), line));
		}
		switch (n)
		{
		case TINA:	PSL_PRINTF(("There is no %c after %s\n", op, str));break;
		case TINSC:	PSL_PRINTF(("There is no %s-statement condition\n", str));break;
		case TINLAG:PSL_PRINTF(("There is no label after goto\n"));break;
		case NOTT:	PSL_PRINTF(("not term : %c\n", op));break;
		case UKT:	PSL_PRINTF(("unknown token\n"));break;
		case TINIA:	PSL_PRINTF(("There is no identifier after %s\n", str));break;
		case TINCOT:PSL_PRINTF(("There is no : on ternary operation\n"));break;
		case TINAE:	PSL_PRINTF(("There is no %c at expression end\n", op));break;
		case IIAE:	PSL_PRINTF(("'%s' is already exsit\n", str));break;
		case BINC:	PSL_PRINTF(("block not closed from %d\n", op));break;
		default:	PSL_PRINTF(("unknown error\n"));
		}
	}
	enum Declare
	{
		DECLARE_NONE = 0,
		DECLARE_GLOBAL,
		DECLARE_STATIC,
		DECLARE_LOCAL,
	};
	void Declaration(variable &c, Declare d, string &name)
	{
		if (d == DECLARE_GLOBAL)		c.pushcode(new Variable::GLOBAL(name));
		else if (d == DECLARE_STATIC)	c.pushcode(new Variable::STATIC(name));
		else if (d == DECLARE_LOCAL)	c.pushcode(new Variable::LOCAL(name));
		else							c.pushcode(new Variable::DECLARATION(name));
	}
	void ParseDangling(variable &g, variable &c)
	{
		if (t->getNextIf('{'/*'}'*/))
			ParseBlock(g, c);
		else
			ParseStatement(g, c);
	}
	void ParseIf(variable &g, variable &c)
	{
		variable v;
		if (t->getNext() != '('/*')'*/)
			Error(TINA, '('/*')'*/, "if");
		else
			ParseExpression(v, /*'('*/')');
		int l = v.codelength();
		Variable::OpCode *oc = NULL;
		if (l)
		{
			oc = new Variable::JF(l);
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
			oc = new Variable::JMP(0);
			v.pushcode(oc);
			ParseDangling(g, v);
			oc->set(v.codelength());
		}
		else
		{
			if (oc)
				oc->set(v.codelength());
		}
		if (Variable::Code *x = v.getcode())
		{
			#ifdef PSL_DEBUG
			c.pushcode(new Variable::IF(x));
			#else
			c.pushcode(new Variable::SCOPE(x));
			#endif
		}
	}
	void ParseFor(variable &g, variable &c)
	{
		variable v;
		int l = 0;
		if (t->getNext() != '('/*')'*/)
			Error(TINA, '('/*')'*/, "for");
		if (!t->getNextIf(';'))
		{
			v.pushcode(new Variable::POP);
			ParseExpression(v, ';');
			l = v.codelength();
		}
		Variable::OpCode *oc = NULL;
		if (!t->getNextIf(';'))
		{
			ParseExpression(v, ';');
			if (v.codelength() - l)
			{
				oc = new Variable::JF(v.codelength());
				v.pushcode(oc);
			}
		}
		variable x;
		if (!t->getNextIf( /*'('*/')'))
			ParseExpression(x, /*'('*/')');
		ParseDangling(g, v);
		int cline = v.codelength();
		if (x.codelength())
		{
			v.pushcode(new Variable::POP);
			v.getcode()->push(x.getcode());
		}
		v.pushcode(new Variable::JMP(l));
		if (oc)
			oc->set(v.codelength());
		if (t->checkNext() == Tokenizer::IDENTIFIER && t->nstr == "else")
		{
			t->getNext();
			ParseDangling(g, v);
		}
		if (Variable::Code *z = v.getcode())
			c.pushcode(new Variable::LOOP(z, cline));
	}
	void ParseWhile(variable &g, variable &c)
	{
		variable v;
		if (t->getNext() != '('/*')'*/)
			Error(TINA, '('/*')'*/, "while");
		else
			ParseExpression(v, /*'('*/')');
		int l = v.codelength();
		Variable::OpCode *oc = NULL;
		if (l)
		{
			oc = new Variable::JF(l);
			v.pushcode(oc);	// 取り敢えず足す
		}
		else
			Error(TINSC, 0, "while");

		ParseDangling(g, v);

		v.pushcode(new Variable::JMP(0));
		if (oc)
			oc->set(v.codelength());
		if (t->checkNext() == Tokenizer::IDENTIFIER && t->nstr == "else")
		{
			t->getNext();
			ParseDangling(g, v);
		}
		if (Variable::Code *x = v.getcode())
			c.pushcode(new Variable::LOOP(x, 0));
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
				c.pushcode(new Variable::SCOPE(v.getcode()));
			return;
		}
		if (n == Tokenizer::IDENTIFIER)
		{
			if (t->nstr == "global" || t->nstr == "static" || t->nstr == "local" || t->nstr == "yield")
			{
				c.pushcode(new Variable::POP);
				ParseExpression(c, ';');
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
				if (t->checkNext() != ';')
				{
					c.pushcode(new Variable::POP);
					ParseExpression(c, ';');
				}
				c.pushcode(new Variable::RETURN);
				return;
			}
			if (t->nstr == "continue")
			{
				if (!t->getNextIf(';'))	Error(TINA, ';', "continue");
				c.pushcode(new Variable::CONTINUE);
				return;
			}
			if (t->nstr == "break")
			{
				if (!t->getNextIf(';'))	Error(TINA, ';', "break");
				c.pushcode(new Variable::BREAK);
				return;
			}
			if (t->nstr == "goto")
			{
				if (!t->getNextIf(Tokenizer::IDENTIFIER))
					Error(TINLAG);
				else
				{
					string label = t->nstr;
					if (!t->getNextIf(';'))
						Error(TINA, ';', "goto");
					c.pushcode(new Variable::GOTO(label));
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
				ParseBlock(m, m);
				return;
			}
			variable e;
			if (n == '('/*')'*/)
			{
				variable arg;
				t->getNext();
				if (!t->getNextIf(/*'('*/')'))
					ParseExpression(arg, /*'('*/')');
				else
					arg.pushcode(new Variable::PUSH_NULL);
				n = t->checkNext();
				if (n == '{'/*'}'*/ || n == Tokenizer::IDENTIFIER)	// 関数定義
				{
					if (g.exist(name))
						Error(IIAE, 0, name, line);
					g[name] = arg;
					variable m = g[name];
					g[name].pushcode(new Variable::ARGUMENT);
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
					e.pushcode(new Variable::VARIABLE(name));
					e.getcode()->push(arg.getcode());
					e.pushcode(new Variable::CALL);
				}
			}
			else if (n == Tokenizer::IDENTIFIER)
			{
				t->getNext();
				e.pushcode(new Variable::VARIABLE(name));
				e.pushcode(new Variable::INSTANCE);
				e.pushcode(new Variable::DECLARATION(t->nstr));
			}
			else
			{
				e.pushcode(new Variable::VARIABLE(name));
			}
			getSuffOp(e);
			c.pushcode(new Variable::POP);
			c.getcode()->push(e.getcode());
			ParseExpression(c, ';', true);
		}
		else
		{
			c.pushcode(new Variable::POP);
			ParseExpression(c, ';');
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
				c.pushcode(new Variable::INC);
			}
			else if (n == Tokenizer::DEC)
			{
				t->getNext();
				c.pushcode(new Variable::DEC);
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
					c.pushcode(new Variable::PUSH_NULL);
				}
				c.pushcode(new Variable::CALL);
			}
			else if (n == '[')
			{
				t->getNext();
				ParseExpression(c, ']');
				c.pushcode(new Variable::INDEX);
			}
			else if (n == '.')
			{
				t->getNext();
				if (t->checkNext() == Tokenizer::IDENTIFIER)
				{
					t->getNext();
					c.pushcode(new Variable::MEMBER(t->nstr));
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
	#define PRE_OP(m,o) if(n==m){t->getNext();getTerm(c);c.pushcode(new Variable::o);}
	#define TERM(m,o) if(n==m){t->getNext();c.pushcode(new Variable::o);getSuffOp(c);}
	void getTerm(variable &c)
	{
		int n = t->checkNext();
		if (n == '('/*')'*/)
		{
			t->getNext();
			variable v;
			if (t->getNextIf(/*'('*/')'))
				v.pushcode(new Variable::PUSH_NULL);
			else
				ParseExpression(v, /*'('*/')');
			if (t->getNextIf('{'/*'}'*/))
			{
				v.pushcode(new Variable::ARGUMENT);
				ParseBlock(v, v);
				c.pushcode(new Variable::PUSH_CODE(v));
			}
			else	// 式内の括弧
			{
				if (v.codelength())
				{
					if (c.codelength())
						c.getcode()->push(v.getcode());
					else
						c = v;
					c.pushcode(new Variable::PARENTHESES);
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
		else PRE_OP('$', CLOSURE)
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
		}
	}
	#undef PRE_OP
	#undef TERM
	#define EXP0(m,e,o) if(n==m){t->getNext();get##e(c);c.pushcode(new Variable::o);}
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
				Variable::OpCode *oc = new Variable::JRF(0);
				c.pushcode(oc);
				int b = c.codelength();
				getexp8(c);
				c.pushcode(new Variable::JR(1));
				oc->set(c.codelength() - b);
				c.pushcode(new Variable::PUSH_NULL);
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
				Variable::OpCode *oc = new Variable::JRT(0);
				c.pushcode(oc);
				int b = c.codelength();
				getexp9(c);
				c.pushcode(new Variable::JR(1));
				oc->set(c.codelength() - b);
				c.pushcode(new Variable::PUSH_INT(1));
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
		if (!l)
		{
			if (t->checkNext() == Tokenizer::IDENTIFIER)
			{
				Declare d = DECLARE_NONE;
				if (t->nstr == "global")		d = DECLARE_GLOBAL;
				else if (t->nstr == "static")	d = DECLARE_STATIC;
				else if (t->nstr == "local")	d = DECLARE_LOCAL;
				if (d)
				{
					t->getNext();
					string scope = t->nstr;
					if (t->getNextIf(Tokenizer::IDENTIFIER))
					{
						string name = t->nstr;
						variable temp;
						getSuffOp(temp);
						if (t->getNextIf(Tokenizer::IDENTIFIER))
						{
							c.pushcode(new Variable::VARIABLE(name));
							if (temp.codelength())
								c.getcode()->push(temp.getcode());
							c.pushcode(new Variable::INSTANCE);
							Declaration(c, d, t->nstr);
						}
						else
						{
							c.pushcode(new Variable::PUSH_NULL());
							Declaration(c, d, name);
							if (temp.codelength())
								c.getcode()->push(temp.getcode());
						}
					}
					else
					{
						getTerm(c);
						if (t->getNextIf(Tokenizer::IDENTIFIER))
						{
							c.pushcode(new Variable::INSTANCE);
							Declaration(c, d, t->nstr);
						}
						else
						{
							Error(TINIA, 0, scope);
						}
					}
					l = true;
				}
			}
			if (!l)
			{
				getTerm(c);
				if (t->getNextIf(Tokenizer::IDENTIFIER))
				{
					c.pushcode(new Variable::INSTANCE);
					c.pushcode(new Variable::DECLARATION(t->nstr));
				}
				l = true;
			}
		}
		getexp10(c, l);
		if (t->getNextIf('?'))
		{
			Variable::OpCode *oc = new Variable::JRF(0);
			c.pushcode(oc);
			int b = c.codelength();
			getexp11(c);
			Variable::OpCode *oc2 = new Variable::JR(0);
			c.pushcode(oc2);
			oc->set(c.codelength() - b);
			b = c.codelength();
			if (!t->getNextIf(':'))	Error(TINCOT);
			getexp11(c);
			#if defined(PSL_OPTIMIZE_PARENTHESES) && defined(PSL_OPTIMIZE_IN_COMPILE)
			if (t->checkNext() && t->checkNext() != ',')
			{
				Variable::Code *code = c.getcode();
				if (code->get(code->length() - 1) == Variable::OpCode::MNEMONIC::PARENTHESES)
					b += 1;
			}
			#endif
			oc2->set(c.codelength() - b);
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
				c.pushcode(new Variable::PUSH_NULL);
			else
				getexp13(c);
			c.pushcode(new Variable::YIELD);
		}
		else
			getexp12(c, l);
		while (int n = t->checkNext())
		{
			EXP('=', 13, SUBSTITUTION)
			else EXP(Tokenizer::ASSIGN, 13, ASSIGNMENT)
			else EXP(Tokenizer::SADD, 13, SADD)
			else EXP(Tokenizer::SSUB, 13, SSUB)
			else EXP(Tokenizer::SMUL, 13, SMUL)
			else EXP(Tokenizer::SDIV, 13, SDIV)
			else EXP(Tokenizer::SMOD, 13, SMOD)
			else EXP(Tokenizer::SAND, 13, SAND)
			else EXP(Tokenizer::SOR, 13, SOR)
			else EXP(Tokenizer::SXOR, 13, SXOR)
			else EXP(Tokenizer::SSHL, 13, SSHL)
			else EXP(Tokenizer::SSHR, 13, SSHR)
			else
				break;
		}
	}
	void ParseExpression(variable &c, char e, bool l = false)
	{
		getexp13(c, l);
		if (!t->getNextIf(e))
			Error(TINAE, e, "", t->getPrevLine());
	}
private:
	Tokenizer *t;
	int error;
};
