class vInt : public vBase
{
public:
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	static void *operator new(size_t t)		{return MemoryManager<sizeof(vInt)>::Next();}
	static void operator delete(void *ptr)	{MemoryManager<sizeof(vInt)>::Release(ptr);}
#endif
	vInt(int i)	{x = i;}
	Type type()	const	{return INT;}
	vBase *clone()	{return new vInt(x);}

	vBase *substitution(Variable *v)	{x = v->toInt();return this;}

	#define OP(n,o) void n(Variable *v)	{x o v->toInt();}
	OP(add,+=)
	OP(sub,-=)
	OP(mul,*=)
//	OP(div,/=)
//	OP(mod,%=)
	OP(oand,&=)
	OP(oor,|=)
	OP(oxor,^=)
	OP(shl,<<=)
	OP(shr,>>=)
	#undef OP
	#define OP(n,o) void n(Variable *v)	{int i = v->toInt();if (i)x o i;else x=0;}
	OP(div,/=)
	OP(mod,%=)
	#undef OP
	#define CMP(n,o) bool n(Variable *v)	{return x o v->toInt();}
	CMP(eq,==)
	CMP(ne,!=)
	CMP(le,<=)
	CMP(ge,>=)
	CMP(lt,<)
	CMP(gt,>)
	#undef CMP
	void neg()	{x = -x;}

	bool toBool()		const {return x;}
	int toInt()			const {return x;}
	double toDouble()	const {return x;}
	string toString()	const {return x;}

	size_t length()		const {return 1;}

	PSL_DUMP((){PSL_PRINTF(("vInt:%d\n", x));})
private:
	int x;
};

class vHex : public vBase
{
public:
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	static void *operator new(size_t t)		{return MemoryManager<sizeof(vHex)>::Next();}
	static void operator delete(void *ptr)	{MemoryManager<sizeof(vHex)>::Release(ptr);}
#endif
	vHex(hex i)	{x = i;}
	Type type()	const	{return HEX;}
	vBase *clone()	{return new vHex(x);}

	vBase *substitution(Variable *v)	{x = v->toInt();return this;}

	#define OP(n,o) void n(Variable *v)	{x o v->toInt();}
	OP(add,+=)
	OP(sub,-=)
	OP(mul,*=)
	OP(oand,&=)
	OP(oor,|=)
	OP(oxor,^=)
	OP(shl,<<=)
	OP(shr,>>=)
	#undef OP
	#define OP(n,o) void n(Variable *v)	{int i = v->toInt();if (i)x o i;else x=0;}
	OP(div,/=)
	OP(mod,%=)
	#undef OP
	#define CMP(n,o) virtual bool n(Variable *v)	{return x o (unsigned)v->toInt();}
	CMP(eq,==)
	CMP(ne,!=)
	CMP(le,<=)
	CMP(ge,>=)
	CMP(lt,<)
	CMP(gt,>)
	#undef CMP

	bool toBool()		const {return x;}
	int toInt()			const {return x;}
	double toDouble()	const {return x;}
	string toString()	const {string s;return s.sprintf("%X", x);}

	size_t length()		const {return 1;}

	PSL_DUMP((){PSL_PRINTF(("vHex:%d\n", x));})
private:
	hex x;
};

class vFloat : public vBase
{
public:
	vFloat(double d)	{x = d;}
	Type type()	const	{return FLOAT;}
	vBase *clone()	{return new vFloat(x);}

	vBase *substitution(Variable *v)	{x = v->toInt();return this;}

	#define OP(n,o) void n(Variable *v)	{x o v->toDouble();}
	OP(add,+=)
	OP(sub,-=)
	OP(mul,*=)
//	OP(div,/=)
	#undef OP
	void div(Variable *v)	{int i = v->toDouble();if (i)x /= i;else x=0;}
	#define CMP(n,o) virtual bool n(Variable *v)	{return x o v->toDouble();}
	CMP(eq,==)
	CMP(ne,!=)
	CMP(le,<=)
	CMP(ge,>=)
	CMP(lt,<)
	CMP(gt,>)
	#undef CMP

	bool toBool()		const {return x;}
	int toInt()			const {return x;}
	double toDouble()	const {return x;}
	string toString()	const {return x;}

	size_t length()		const {return 1;}

	PSL_DUMP((){PSL_PRINTF(("vFloat:%f\n", x));})
private:
	double x;
};

class vString : public vBase
{
public:
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	static void *operator new(size_t t)		{return MemoryManager<sizeof(vString)>::Next();}
	static void operator delete(void *ptr)	{MemoryManager<sizeof(vString)>::Release(ptr);}
#endif
	vString(const string &s)	{x = s;}
	Type type()	const	{return STRING;}
	vBase *clone()	{return new vString(x);}

	vBase *substitution(Variable *v)	{x = v->toString();return this;}

	void add(Variable *v)	{x += v->toString();}
	#define OP(n,o) void n(Variable *v)	{x o v->toInt();}
	OP(sub,-=)
	OP(div,/=)
	OP(mod,%=)
	#undef OP
	#define CMP(n,o) virtual bool n(Variable *v)	{return x o v->toString();}
	CMP(eq,==)
	CMP(ne,!=)
	CMP(le,<=)
	CMP(ge,>=)
	CMP(lt,<)
	CMP(gt,>)
	#undef CMP

	bool toBool()		const {return x.length();}
	int toInt()			const {return x;}
	double toDouble()	const {return x;}
	string toString()	const {return x;}

	size_t length()		const {return 1;}

	PSL_DUMP((){PSL_PRINTF(("vString:%s\n", (const char*)x));})
private:
	string x;
};

class vPointer : public vBase
{
public:
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	static void *operator new(size_t t)		{return MemoryManager<sizeof(vPointer)>::Next();}
	static void operator delete(void *ptr)	{MemoryManager<sizeof(vPointer)>::Release(ptr);}
#endif
	vPointer()	{x = NULL;}
	vPointer(Variable *v)	{x = v ? v->ref() : NULL;}
	~vPointer()	{if (x)x->finalize();}
	Type type()	const	{return POINTER;}
	vBase *clone()	{return new vPointer(x);}
	void searchcount(Variable *v, int &c){if (x)x->searchcount(v, c);}
	void mark(){if (x)x->mark();}

	vBase *substitution(Variable *v)
	{
		if (x)			x->finalize();
		if (v->type() == POINTER)	x = v->deref()->ref();	// 一段階は考慮しよう
		else						x = v->ref();			// v自体がポインタだったらどうする？
		return this;
	}

	Variable *deref()	{return x;}

	bool toBool()		const {return x ? true : false;}
	int toInt()			const {return x ? 1 : 0;}
	double toDouble()	const {return x ? 1 : 0;}
	string toString()	const {return x ? "[pointer]" : "NULL";}

	size_t length()		const {return x ? 1 : 0;}
	PSL_DUMP((){PSL_PRINTF(("vPointer:%s\n", x ? "" : "NULL"));if(x)x->dump();})
private:
	Variable *x;
};

class vReference : public vBase
{
public:
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	static void *operator new(size_t t)		{return MemoryManager<sizeof(vReference)>::Next();}
	static void operator delete(void *ptr)	{MemoryManager<sizeof(vReference)>::Release(ptr);}
#endif
	vReference(Variable *v)	{Variable *z = v->x->referenceTo();x = (z ? z : v)->ref();}
	~vReference()			{x->finalize();}
	Type type()	const	{return x->type();}
	vBase *clone()	{return x->x->clone();}
	Variable *referenceTo()	{return x;}
	void searchcount(Variable *v, int &c){x->searchcount(v, c);}
	void mark(){x->mark();}

	vBase *substitution(Variable *v)	{x->substitution(v);return this;}
	vBase *assignment(Variable *v)		{x->assignment(v);return this;}

	#define OP(n) void n(Variable *v)	{x->n(v);}
	OP(add)
	OP(sub)
	OP(mul)
	OP(div)
	OP(mod)
	OP(oand)
	OP(oor)
	OP(oxor)
	OP(shl)
	OP(shr)
	#undef OP
	#define CMP(n) bool n(Variable *v)	{return x->n(v);}
	CMP(eq)
	CMP(ne)
	CMP(le)
	CMP(ge)
	CMP(lt)
	CMP(gt)
	#undef CMP
	void neg()	{x->neg();}
	Variable *deref()	{return x->deref();}

	bool toBool()		const {return x->toBool();}
	int toInt()			const {return x->toInt();}
	double toDouble()	const {return x->toDouble();}
	string toString()	const {return x->toString();}
	void *toPointer()	const {return x->toPointer();;}

	size_t length()				const {return x->length();}
	bool exist(const string &s)	const {return x->exist(s);}
	Variable *index(size_t t)			{return x->index(t);}
	Variable *child(const string &s)	{return x->child(s);}
	void push(Variable *v)				{return x->push(v);}
	Variable *keys()					{return x->keys();}
	bool set(const string &s, const variable &v)	{return x->set(s, v);}
	void del(const string &s)	{x->del(s);}

	void prepare(Environment &env, Variable *v)			{x->prepare(env);}
	void prepareInstance(Environment &env, Variable *v)	{x->prepareInstance(env);}
	Variable *instance(Variable *v)	{return x->instance();}
	rsv call(Environment &env, variable &arg, Variable *v)	{return x->call(env, arg);}

	// ライブラリ関数で使うかもしれないので一応
	size_t codelength()			{return x->codelength();}
	Code *getcode()				{return x->getcode();}
	void pushcode(OpCode *c)	{x->pushcode(c);}
	void pushlabel(const string &s){x->pushlabel(s);}
	void write(const string &s, bytecode &b){x->write(s, b);}
	PSL_DUMP((){PSL_PRINTF(("vReference:\n"));x->dump();})
private:
	Variable *x;
};

class vNReference : public vBase
{
public:
	vNReference(){}
	Type type()	const	{return REFERENCE;}
	vBase *clone()	{return new vNReference();}

	vBase *substitution(Variable *v)
	{
		if (v->type() == REFERENCE)
		{
			delete this;
			return new vObject();
		}
		delete this;
		return new vReference(v);
	}

	PSL_DUMP((){PSL_PRINTF(("vNReference\n"));})
};

class vRArray : public vBase
{
public:
	vRArray():x(2){}
	vRArray(int i):x(i){}
	Type type()	const	{return RARRAY;}
	void searchcount(Variable *v, int &c)
	{
		int size = x.size();
		for (int i = 0; i < size; ++i)
			x[i].get()->searchcount(v, c);
	}
	void mark()
	{
		int size = x.size();
		for (int i = 0; i < size; ++i)
			x[i].get()->mark();
	}
	vBase *clone()
	{
		int size = x.size();
		if (size == 1)
			return x[0].get()->x->clone();
		vObject *v = new vObject();
		for (int i = 0; i < size; ++i)
			v->push(x[i].get());
		return v;
	}
	Variable *referenceTo()
	{
		if (x.size() == 1)
		{
			Variable *v = x[0].get();
			Variable *r = v->x->referenceTo();
			return r ? r : v;
		}
		return NULL;
	}

	vBase *substitution(Variable *v)
	{
		bool ra = false;
		Variable *o = v;
		if (v->type() == RARRAY)
		{
			v = v->clone();
			ra = true;
		}

		int size = x.size();
		int vsize = v->length();
		for (int i = 0; i < size && i < vsize; ++i)
		{
			if (x[i].get()->type() == POINTER || x[i].get()->type() == REFERENCE)
				x[i].get()->substitution(o->index(i));
			else
				x[i].get()->substitution(v->index(i));
		}

		if (ra)
			v->finalize();

		return this;
	}
	vBase *assignment(Variable *v)
	{
		bool ra = false;
		Variable *o = v;
		if (v->type() == RARRAY)
		{
			v = v->clone();
			ra = true;
		}

		int size = x.size();
		int vsize = v->length();
		for (int i = 0; i < size && i < vsize; ++i)
		{
			if (x[i].get()->type() == POINTER || x[i].get()->type() == REFERENCE)
				x[i].get()->assignment(o->index(i));
			else
				x[i].get()->assignment(v->index(i));
		}

		if (ra)
			v->finalize();

		return this;
	}

	#define OP(n) void n(Variable *v)	{if (x.size() == 1)x[0].get()->n(v);}
	OP(add)
	OP(sub)
	OP(mul)
	OP(div)
	OP(mod)
	OP(oand)
	OP(oor)
	OP(oxor)
	OP(shl)
	OP(shr)
	#undef OP
	#define CMP(n) bool n(Variable *v)	{if (x.size() == 1)return x[0].get()->n(v);return false;}
	CMP(eq)
	CMP(ne)
	CMP(le)
	CMP(ge)
	CMP(lt)
	CMP(gt)
	#undef CMP
	Variable *deref()	{if (x.size() == 1)return x[0].get()->deref();return NULL;}

	bool toBool()		const {if (x.size() == 1)return x[0].get()->toBool();return x.size();}
	int toInt()			const {if (x.size() == 1)return x[0].get()->toInt();return x.size();}
	double toDouble()	const {if (x.size() == 1)return x[0].get()->toDouble();return x.size();}
	string toString()	const {if (x.size() == 1)return x[0].get()->toString();return "[array]";}
	void *toPointer()	const {if (x.size() == 1)return x[0].get()->toPointer();return NULL;}

	size_t length()		const {if (x.size() == 1)return x[0].get()->length();return x.size();}
	Variable *index(size_t t)	{
		if (x.size() == 1)return x[0].get()->index(t);
		if (t>=x.size())x.resize(t+1);return x[t].get();
	}
	Variable *child(const string &s)	{if (x.size() == 1)return x[0].get()->child(s);return NULL;}
	void push(Variable *v)	{x.push_back(v);}

	void prepare(Environment &env, Variable *v)	{if (x.size() == 1)x[0].get()->prepare(env);}
	rsv call(Environment &env, variable &arg, Variable *v)	{
		if (x.size() == 1)
			x[0].get()->call(env, arg);
		return variable(NIL);
	}

	PSL_DUMP((){PSL_PRINTF(("vRArray:%d\n", x.size()));for (size_t i = 0; i < x.size(); ++i)x[i].get()->dump();})
private:
	mutable rlist x;
};

class vObject : public vBase
{
public:
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	static void *operator new(size_t t)		{return MemoryManager<sizeof(vObject)>::Next();}
	static void operator delete(void *ptr)	{MemoryManager<sizeof(vObject)>::Release(ptr);}
#endif
	vObject()				{_class = NULL;code = NULL;}
	vObject(Variable *v)	{_class = v->ref();code = NULL;}
	vObject(Code *c)		{_class = NULL;code = c->inc();}
	~vObject()	{
		if (_class)
		{
			const static string destructor = "destructor";
			if (member.count(destructor))
			{
				Environment env;
				variable arg;
				member[destructor].get()->call(env, arg);
			}
			_class->finalize();
		}
		if (code)	code->finalize();
	}
	Type type()	const	{return OBJECT;}
	vBase *clone()	{return new vObject(this);}
	vObject(vObject *v)
	{
		int size = v->array.size();
		array.resize(size);
		for (int i = 0; i < size; ++i)
			array[i].copy(v->array[i]);
		for (table::iterator it = v->member.begin(); it != v->member.end(); ++it)
			member[it->first].copy(it->second);
		if (v->_class)	_class = v->_class->ref();
		else			_class = NULL;
		if (v->code)	code = v->code->inc();
		else			code = NULL;
	}
	void searchcount(Variable *v, int &c)
	{
		int size = array.size();
		for (int i = 0; i < size; ++i)
			array[i].get()->searchcount(v, c);
		for (table::iterator it = member.begin(); it != member.end(); ++it)
			member[it->first].get()->searchcount(v, c);
		if (_class)
			_class->searchcount(v, c);
	}
	void mark()
	{
		int size = array.size();
		for (int i = 0; i < size; ++i)
			array[0].get()->mark();
		for (table::iterator it = member.begin(); it != member.end(); ++it)
			member[it->first].get()->mark();
		if (_class)
			_class->mark();
	}

	vBase *substitution(Variable *v)
	{
		if (array.empty() && member.empty())
		{
			vBase *x = v->x->clone();
			delete this;
			return x;
		}
		size_t size = v->length();
		array.resize(size);
		for (size_t i = 0; i < size; ++i)
			array[i].get()->substitution(v->index(i));

		Variable *k = v->keys();
		size = k->length();
		for (size_t i = 0; i < size; ++i)
		{
			string s = k->index(i)->toString();
			member[s].get()->substitution(v->child(s));
		}
		k->ref()->finalize();

		if (Code *c = v->getcode())
		{
			if (code)
				code->finalize();
			code = c->inc();
		}

		return this;
	}
//	vBase *assignment(Variable *v)	{delete this;return v->x->clone();}
	//	個別にassignmentを繰り返す案もある(ARRAYと同じ様な)

	// 演算子系は比較ぐらいはあってもいいが
	// +=で連結とかどう？って=と同じかそれ、ARRAYだな
	// あ、memberに対しては同じだがarrayに対しては違うな

	bool toBool()		const {return !array.empty() || !member.empty() || code;}
	int toInt()			const {return array.size();}
	double toDouble()	const {return array.size();}
	string toString()	const {return !array.empty() || !member.empty() ? "[Object]" : code ? "[function]" : "nil";}

	size_t length()				const {return array.size();}
	bool exist(const string &s)	const {return member.count(s);}
	Variable *index(size_t t)			{if(t>=array.size())array.resize(t+1);return array[t].get();}
	Variable *child(const string &s)	{return member[s].get();}
	void push(Variable *v)	{Variable *x = v->clone();array.push_back(x);x->finalize();}
	Variable *keys()
	{
		Variable *v = new Variable();
		for (table::iterator it = member.begin(); it != member.end(); ++it)
		{
			Variable *x = new Variable(it->first);
			v->push(x);
			x->finalize();			// すっごい二度手間なんだけど
		}
		return v;
	}
	bool set(const string &s, const variable &v)
	{
		bool r = true;
		if (exist(s))
			r = false;
		member[s] = v;
		return r;
	}
	void del(const string &s)	{member.erase(s);}
	void method_this(Variable *v)	// メソッドのthisを差し替える
	{
		int size = array.size();
		for (int i = 0; i < size; ++i)
		{
			if (array[i].get()->type() == METHOD || array[i].get()->type() == CMETHOD)
				array[i].get()->push(v);
		}
		for (table::iterator it = member.begin(); it != member.end(); ++it)
		{
			if (it->second.get()->type() == METHOD || it->second.get()->type() == CMETHOD)
				it->second.get()->push(v);
		}
	}

	void prepare(Environment &env, Variable *v)
	{
		if (!code)
			return;
		Scope *scope = new FunctionScope(code, _class ? _class : v);
		env.addScope(scope);
	}
	void prepareInstance(Environment &env, Variable *v)
	{
		Variable *x = instance(v);
		if (!code)
		{
			env.push(x);
		}
		else
		{
			Scope *scope = new ConstructorScope(code, v, x);
			env.addScope(scope);
			variable z;
			env.push(z);
		}
		x->finalize();
	}
	rsv call(Environment &env, variable &arg, Variable *v)
	{
		if (!code)
			return variable(NIL);
		Scope *scope = new FunctionScope(code, _class ? _class : v);
		env.addScope(scope);
		env.push(arg);
		env.Run();
		return env.pop();
	}
	Variable *instance(Variable *v)
	{
		vObject *o = new vObject(v);
		Variable *x = new Variable(o);

		int size = array.size();
		o->array.resize(size);
		for (int i = 0; i < size; ++i)
			o->array[i].get()->assignment(array[i].get());

		for (table::iterator it = member.begin(); it != member.end(); ++it)
		{
			if (it->second.get()->getcode())	// コード持ってれば
			{
				Variable *z = new Variable(new vMethod(it->second.get(), x));
				o->member[it->first].set(z);
			}
			else if (it->second.get()->type() == CMETHOD || it->second.get()->type() == METHOD)
			{
				Variable *z = it->second.get()->clone();
				z->push(x);
				o->member[it->first].set(z);
			}
			else
			{
				o->member[it->first].get()->assignment(it->second.get());
			}
		}
		return x;
	}

	size_t codelength()		{return code ? code->length() : 0;}
	Code *getcode()			{return code;}
	void pushcode(OpCode *c){if (!code) code = new Code();code->push(c);}
	void pushlabel(const string &s){if (!code) code = new Code();code->pushlabel(s);}
	void write(const string &s, bytecode &b)
	{
		if (!code)
			return;
		b.push(OpCode::MNEMONIC::BEGIN);
		b.push(s.c_str(), s.length()+1);
		for (table::iterator it = member.begin(); it != member.end(); ++it)
			it->second.get()->write(it->first, b);
		code->write(b);
	}

	PSL_DUMP((){PSL_PRINTF(("vObject:%d,%d,%d\n", array.size(), member.size(), codelength()));
		for (size_t i = 0; i < array.size(); ++i)array[i].get()->dump();
		for (table::iterator it = member.begin(); it != member.end(); ++it){PSL_PRINTF(("[%s]:\n", it->first.c_str()));it->second.get()->dump();}
		if(code){PSL_PRINTF(("--CODE--\n"));code->dump();}
	})
private:
	rlist array;
	table member;
	Variable *_class;
	Code *code;
};

class vMethod : public vBase
{
public:
	vMethod(Variable *v, Variable *x)
	{
		function = v->ref();
//		_this = x->ref();	// 循環参照で死ぬ
		_this = x;
	}
	~vMethod()
	{
		function->finalize();
//		_this->finalize();
	}
	Type type()	const	{return METHOD;}
	vBase *clone()	{return new vMethod(function, _this);}
	void searchcount(Variable *v, int &c){function->searchcount(v, c);}
	void mark(){function->mark();}

	bool toBool()		const {return true;}
	string toString()	const {return "[Method]";}
	size_t length()		const {return 1;}
	void push(Variable *v)	{_this = v;}

	void prepare(Environment &env, Variable *v)
	{
		if (!function->getcode())
			return;
		Scope *scope = new MethodScope(function->getcode(), function, _this);
		env.addScope(scope);
	}
	rsv call(Environment &env, variable &arg, Variable *v)
	{
		if (!function->getcode())
			return variable(NIL);
		Scope *scope = new MethodScope(function->getcode(), function, _this);
		env.addScope(scope);
		env.push(arg);
		env.Run();
		return env.pop();
	}

	PSL_DUMP((){PSL_PRINTF(("vMethod:\n"));})
private:
	Variable *function;
	Variable *_this;
};

class vCFunction : public vBase
{
public:
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	static void *operator new(size_t t)		{return MemoryManager<sizeof(vCFunction)>::Next();}
	static void operator delete(void *ptr)	{MemoryManager<sizeof(vCFunction)>::Release(ptr);}
#endif
	vCFunction(function _f)	{f = _f;}
	Type type()	const	{return CFUNCTION;}
	vBase *clone()	{return new vCFunction(f);}

	bool toBool()		const {return true;}
	string toString()	const {return "[CFunction]";}
	size_t length()		const {return 1;}

	void prepare(Environment &env, Variable *v)
	{
		variable x = env.pop();
		variable r = f(x);
		env.push(r);
	}
	rsv call(Environment &env, variable &arg, Variable *v)	{return f(arg);}
	PSL_DUMP((){PSL_PRINTF(("vCFunction:\n"));})
private:
	function f;
};

class vCMethod : public vBase
{
public:
	vCMethod(method _f, Variable *x)	{f = _f;_this = x;}
	Type type()	const	{return CMETHOD;}
	vBase *clone()	{return new vCMethod(f, _this);}

	bool toBool()		const {return true;}
	int toInt()			const {return _this ? 1 : 0;}
	string toString()	const {return "[CMethod]";}
	size_t length()		const {return 1;}
	void push(Variable *v)
	{
		_this = v;
	}

	void prepare(Environment &env, Variable *v)
	{
		variable x = env.pop();
		if (_this)
		{
			variable t = _this;
			env.push(f(t, x));
		}
		else
		{
			variable z;
			env.push(f(z, x));
		}
	}
	rsv call(Environment &env, variable &arg, Variable *v)
	{
		if (_this)
		{
			variable t = _this;
			return f(t, arg);
		}
		else
		{
			variable z;
			return f(z, arg);
		}
	}
	PSL_DUMP((){PSL_PRINTF(("vCMethod:\n"));})
private:
	method f;
	Variable *_this;
};

class vCPointer : public vBase
{
public:
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	static void *operator new(size_t t)		{return MemoryManager<sizeof(vCPointer)>::Next();}
	static void operator delete(void *ptr)	{MemoryManager<sizeof(vCPointer)>::Release(ptr);}
#endif
	vCPointer(void *p)	{x = p;}
	Type type()	const	{return CPOINTER;}
	vBase *clone()	{return new vCPointer(x);}

	vBase *substitution(Variable *v)	{x = v->toPointer();return this;}

	bool toBool()		const {return x ? true : false;}
	string toString()	const {return x ? "[cpointer]" : "NULL";}
	void *toPointer()	const {return x;}

	size_t length()		const {return x ? 1 : 0;}

	PSL_DUMP((){PSL_PRINTF(("vCPointer:%X\n", x));})
private:
	void *x;
};

class vThread : public vBase
{
public:
	vThread()	{x = NULL;e = NULL;}
	vThread(Variable *v)	{x = v ? v->ref() : NULL;e = NULL;}
	~vThread()	{if (x)x->finalize();delete e;}
	Type type()	const	{return THREAD;}
	vBase *clone()	{return new vThread(x);}
	void searchcount(Variable *v, int &c){if (x)x->searchcount(v, c);}
	void mark(){if (x)x->mark();}

	vBase *substitution(Variable *v)
	{
		if (x)
			x->finalize();
		x = v->ref();
		return this;
	}

	bool toBool() const
	{
		if (!x)
			return false;
		if (!e)
			return x->getcode();
		return e->Runable();
	}
	string toString()	const {return "[Thread]";}

	size_t length()		const {return x ? 1 : 0;}
	bool exist(const string &s)	const {return x ? x->exist(s) : false;}
	Variable *child(const string &s)	{return x ? x->child(s) : NULL;}

	void prepare(Environment &env, Variable *v)
	{
		if (!x)
			return;
		if (e && !e->Runable())
			return;
		if (!e)
		{
			e = new Environment(env);
			x->prepare(*e);
		}
		e->push(env.pop());
		e->Run();
		variable r = e->pop();
		env.push(r);
	}

	PSL_DUMP((){PSL_PRINTF(("vThread:%X\n", e));if(x)x->dump();})
private:
	Variable *x;
	Environment *e;
};
