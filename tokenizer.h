class Tokenizer
{
public:
	enum TOKEN
	{
		UNSET = -1,
		NONE = 0,
		IDENTIFIER = 1,
		INT,
		HEX,
		NUMBER,
		STRING,
		ASSIGN,
		INC,
		DEC,
		EQ,
		NE,
		LE,
		GE,

		SADD,
		SSUB,
		SMUL,
		SDIV,
		SMOD,
		SAND,
		SOR,
		SXOR,

		SHL,
		SHR,
		SSHL,
		SSHR,

		BAND,
		BOR,
	};
	static Tokenizer *New(const string &fn, table *dl = NULL)
	{
		using namespace std;
		FILE *fp = fopen(fn, "r");
		if (!fp)
			return NULL;
		Tokenizer *t = new Tokenizer(fp, fn, dl);
		fclose(fp);
		return t;
	}
	Tokenizer(std::FILE *fp, const string &fn, table *dl = NULL)
	{
		using namespace std;
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		s = new char[len+1];
		fseek(fp, 0, SEEK_SET);
		len = fread(s, 1, len, fp);
		s[len] = 0;
		Init(fn, 1, dl);
	}
	Tokenizer(const char *str, const string &fn, int l, table *dl = NULL)
	{
		using namespace std;
		len = strlen(str);
		s = new char[len+1];
		strcpy(s, str);
		Init(fn, l, dl);
	}
	~Tokenizer()
	{
		delete[] s;
		delete including;
#ifdef PSL_USE_TOKENIZER_DEFINE
		if (dlnew)
			delete definelist;
#endif
	}
	int getNext()
	{
		if (next > 0)
		{
			int n = next;
			next = UNSET;
			return n;
		}
		if (next < 0)
			return doNext();
		return 0;
	}
	int checkNext()
	{
		if (next > 0)
			return next;
		if (next < 0)
			return next = doNext();
		return 0;
	}
	bool getNextIf(int t)
	{
		if (checkNext() == t)
		{
			next = UNSET;
			return true;
		}
		return false;
	}
private:
	void Init(const string &fn, int l, table *dl)
	{
		i = 0;
		next = UNSET;
		filename = fn;
		line = pline = l;
		including = NULL;
#ifdef PSL_USE_TOKENIZER_DEFINE
		if (dl)
		{
			definelist = dl;
			dlnew = false;
		}
		else
		{
			definelist = new table();
			dlnew = true;
		}
#endif
	}
	string filename;
	int line;
	int pline;
	char *s;
	int len;
	int i;
	int next;
	Tokenizer *including;
	table *definelist;
#ifdef PSL_USE_TOKENIZER_DEFINE
//	std::map<string,string> definelist;	// これやるとでかいんだ…
	bool dlnew;
#endif
	bool whitespace()
	{
		for (; i < len; ++i)
		{
			if (s[i] == ' ' || s[i] == '\t')			// 空白を飛ばす
				continue;
			break;
		}
		if (i == len)
			return true;
		return false;
	}
	void lcomment()
	{
		for (; i < len; ++i)
		{
			if (s[i] == '\r' || s[i] == '\n')
				return;
		}
	}
	void mcomment()
	{
		int l = line;
		for (; i < len-1; ++i)
		{
			if (s[i] == '\n')
			{
				++line;
				continue;
			}
			if (s[i] == '\r')
			{
				++line;
				if (i < len && s[i+1] == '\n')
					++i;
				continue;
			}
			if (s[i] == '*' && s[i+1] == '/')
			{
				i += 2;
				return;
			}
		}
		PSL_PRINTF(("error %s %d: comment not closed from %d\n", filename.c_str(), line, l));
		++i;
	}
	void string_literal(char end = '"')
	{
		int h = i;
		nstr = "";
		for (; i < len; ++i)
		{
			if (s[i] == end)
			{
				nstr += string(s+h, i-h);
				++i;
				return;
			}
			if (s[i] == '\\')
			{
				nstr += string(s+h, i-h);
				h = i;
				int j = i+1;
				if (s[j] == 'n')		nstr += '\n';
				else if (s[j] == '"')	nstr += '"';
				else if (s[j] == 't')	nstr += '\t';
/*				else if (s[j] == 'r')	nstr += '\r';
				else if (s[j] == 'a')	nstr += '\a';
				else if (s[j] == 'f')	nstr += '\f';
				else if (s[j] == 'v')	nstr += '\v';*/
/*				else if (s[j] == 'x')
				{
					// これは後の課題
					// バイナリ列をどう扱うかも決めていないので
					char c = 0;
					nstr += c;
				}*/
				else					continue;
				h = j+1;
				i = j;
				continue;
			}
		}
	}
	bool getIdentifier()
	{
		if (!(s[i] == '_' || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= 'a' && s[i] <= 'z')))
			return false;
		int h = i;
		for (++i; i < len; ++i)
		{
			if (s[i] == '_' || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= '0' && s[i] <= '9'))
				continue;
			break;
		}
		nstr = string(s+h, i-h);
		return true;
	}
	int preprocessor()
	{
		if (whitespace())
			return 0;
		if (!getIdentifier())
		{
			PSL_PRINTF(("warning %s %d: tokenizer directive syntax error\n", filename.c_str(), line));
			lcomment();
			return doNext();
		}
		string directive = nstr;
		if (whitespace())
			return 0;
		if (directive == "include")
		{
			if (s[i] == '<' || s[i] == '"')
			{
				int h = ++i;
				for (; i < len; ++i)
				{
					if (s[i] == '>' || s[i] == '"' || s[i] == '\r' || s[i] == '\n')
					{
						string name(s+h, i-h);
						++i;
						including = New(name, definelist);
						if (!including)
							PSL_PRINTF(("tokenizer %s %d: can't open include file <%s>\n", filename.c_str(), line, name.c_str()));
						break;
					}
				}
			}
			return doNext();
		}
#ifdef PSL_USE_TOKENIZER_DEFINE
		else if (directive == "define")
		{
			if (!getIdentifier())
				PSL_PRINTF(("warning %s %d: define syntax error\n", filename.c_str(), line));
			else
			{
				if (whitespace())
					return 0;
				int h = i;
				lcomment();
				variable temp = string(s+h, i-h);
				#ifdef PSL_USE_STL_MAP
				definelist->insert(table::value_type(nstr, temp));
				#else
				(*definelist)[nstr] = temp;
				#endif
				return doNext();
			}
		}
		else if (directive == "undef")
		{
			if (!getIdentifier())
				PSL_PRINTF(("warning %s %d: undef syntax error\n", filename.c_str(), line));
			else
			{
				definelist->erase(nstr);
			}
		}
#endif
		else
		{
			PSL_PRINTF(("tokenizer %s %d: unknown tokenizer directive #%s\n", filename.c_str(), line, directive.c_str()));
		}
		lcomment();
		return doNext();
	}
	int doNext()
	{
		pline = line;
		if (including)
		{
			int r = including->doNext();
			if (r)
			{
				nstr = including->nstr;
				nint = including->nint;
				nnum = including->nnum;
				return r;
			}
			else
			{
				delete including;
				including = NULL;
			}
		}
		for (; i < len; ++i)
		{
			if (s[i] == ' ' || s[i] == '\t')			// 空白を飛ばす
				continue;
			if (s[i] == '\n')
			{
				++line;
				continue;
			}
			if (s[i] == '\r')
			{
				++line;
				if (i < len && s[i+1] == '\n')
					++i;
				continue;
			}
			break;
		}
		if (i == len)
			return 0;
		switch (s[i++])
		{
		case '=':	if (s[i] == '='){++i;return EQ;}
										return '=';
		case '+':	if (s[i] == '+'){++i;return INC;}
					if (s[i] == '='){++i;return SADD;}
										return '+';
		case '-':	if (s[i] == '-'){++i;return DEC;}
					if (s[i] == '='){++i;return SSUB;}
										return '-';
		case '*':	if (s[i] == '='){++i;return SMUL;}
										return '*';
		case '/':	if (s[i] == '='){++i;return SDIV;}
					if (s[i] == '/'){++i;lcomment();return doNext();}
					if (s[i] == '*'){++i;mcomment();return doNext();}
										return '/';
		case '%':	if (s[i] == '='){++i;return SMOD;}
										return '%';
		case '&':	if (s[i] == '='){++i;return SAND;}
					if (s[i] == '&'){++i;return BAND;}
										return '&';
		case '|':	if (s[i] == '='){++i;return SOR;}
					if (s[i] == '|'){++i;return BOR;}
										return '|';
		case '^':	if (s[i] == '='){++i;return SXOR;}
										return '^';
		case '<':	if (s[i] == '='){++i;return LE;}
					if (s[i] == '<'){++i;
					if (s[i] == '='){++i;return SSHL;}
										return SHL;}
										return '<';
		case '>':	if (s[i] == '='){++i;return GE;}
					if (s[i] == '>'){++i;
					if (s[i] == '='){++i;return SSHR;}
										return SHR;}
										return '>';
		case '!':	if (s[i] == '='){++i;return NE;}
										return '!';
		case '~':						return '~';
		case '.':	if (s[i] >= '0' && s[i] <= '9'){
				char *e;
				nnum = std::strtod(s+i-1, &e);
				i = e-s;
				return NUMBER;
			}	// 小数点から始まる数字の場合もある
										return '.';
		case ',':						return ',';
		case ';':						return ';';
		case '[':						return '[';
		case ']':						return ']';
		case '(':						return '(';
		case ')':						return ')';
		case '{':						return '{';
		case '}':						return '}';
		case '?':						return '?';
		case ':':	if (s[i] == '='){++i;return ASSIGN;}
										return ':';
		case '"':string_literal();		return STRING;	// "を見付けるまでスキップする

//		case '\'':						return '\'';	// 保留
			// 未使用予定
/*		case '@':						return '@';
		case '$':						return '$';
		case '\\':						return '\\';
		case '`':						return '`';*/

		case '#':						return preprocessor();
		}
		i--;

		if (getIdentifier())
		{
#ifdef PSL_USE_TOKENIZER_DEFINE
			if (definelist->count(nstr))
			{
				including = new Tokenizer((*definelist)[nstr].get()->toString(), filename + " define:" + nstr, line, definelist);
				return getNext();
			}
#endif
			return IDENTIFIER;
		}
		if (s[i] >= '0' && s[i] <= '9')
		{
			if (s[i] == '0' && (s[i+1] == 'x' || s[i+1] == 'X'))
			{
				nint = 0;
				for (i += 2; i < len; ++i)
				{
					if (s[i] >= '0' && s[i] <= '9')
					{
						nint *= 16;
						nint += s[i] - '0';
						continue;
					}
					if (s[i] >= 'a' && s[i] <= 'f')
					{
						nint *= 16;
						nint += s[i] - 'a' + 10;
						continue;
					}
					if (s[i] >= 'A' && s[i] <= 'F')
					{
						nint *= 16;
						nint += s[i] - 'A' + 10;
						continue;
					}
					break;
				}
				return HEX;
			}

			nint = s[i] - '0';
			int h = i;
			for (++i; i < len; ++i)
			{
				if (s[i] >= '0' && s[i] <= '9')
				{
					nint *= 10;
					nint += s[i] - '0';
					continue;
				}
				if (s[i] == 'e' || s[i] == 'E' || s[i] == '.')
				{
					char *e;
					nnum = std::strtod(s+h, &e);
					i = e-s;
					return NUMBER;
				}
				break;
			}
			return INT;
		}
		++i;
		return -1;
	}
public:
	int nint;
	double nnum;
	string nstr;
	int getLine()		{return including ? including->getLine() : line;}
	int getPrevLine()	{return including ? including->getPrevLine() : pline;}
	string &getFile()	{return including ? including->getFile() : filename;}
};
