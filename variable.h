#ifndef VARIABLE_H
#define VARIABLE_H

#include "pstring.h"

#include <new>
#include <cstring>
#include <vector>
#include <map>
#include <stack>
//#define PSL_USE_STL_VECTOR
//#define PSL_USE_STL_STACK
//#define PSL_USE_STL_MAP
	#define PSL_USE_HASH_MAP
//#define PSL_NULL_RSV		// rsvはNULLを許容する、STL_VECTORを使う場合は必須

//#define PSL_DEBUG

#define PSL_WARNING_POP_EMPTY_STACK			// 空スタックからのPOPを通知する
#define PSL_WARNING_STACK_REMAINED			// Environmentのdelete時にスタックが残っていることを通知する
//#define PSL_WARNING_UNDECLARED_IDENTIFIER	// 未宣言の変数の使用を通知する

#define PSL_USE_VARIABLE_MEMORY_MANAGER

#define PSL_USE_TOKENIZER_DEFINE

class variable
{
	class rsv;
#ifdef __GNUC__
public:
#else
private:
#endif
	class Variable;
public:
	variable(const rsv &v)	{x = v.get()->ref();}
private:
	#include "container.h"
	friend class rsv;
public:
	typedef variable(*function)(variable&);
	typedef unsigned long hex;
	enum Type
	{
		NIL,
		INT,
		HEX,
		FLOAT,
		STRING,
		POINTER,
		REFERENCE,
//		ARRAY,
		RARRAY,
		OBJECT,
		METHOD,
		CFUNCTION,
		THREAD,
	};


	variable()					{x = new Variable();}
	variable(bool b)			{x = new Variable((int)b);}
	variable(int i)				{x = new Variable(i);}
	variable(double d)			{x = new Variable(d);}
	variable(const char *s)		{x = new Variable(s);}
	variable(const string &s)	{x = new Variable(s);}
	variable(function f)		{x = new Variable(f);}
	variable(const variable &v)	{x = v.x->clone();}
	variable(Type t)			{x = new Variable(t);}
	~variable()					{x->finalize();}

	Type type()			const	{return x->type();}
	bool type(Type t)	const	{return x->type() == t;}

	variable &substitution(const variable &v)	{x->substitution(v.x);return *this;}
	variable &assignment(const variable &v)		{x->assignment(v.x);return *this;}
	variable &operator=(const variable &v)		{x->substitution(v.x);return *this;}
	variable &operator->*(const variable &v)	{x->assignment(v.x);return *this;}		// 何せ余ってる演算子がこれなので…

	variable &operator=(function f)		{
		variable v = f;
		x->substitution(v.x);
		return *this;
	}
	variable &operator=(int i)		{
		variable v = i;
		x->substitution(v.x);
		return *this;
	}

	#define OP(n,o) variable &operator o##=(const variable &v)	{x->n(v.x);return *this;}\
					rsv operator o(const variable &v)	{variable z = *this;z.x->n(v.x);return z;}
	OP(add,+)
	OP(sub,-)
	OP(mul,*)
	OP(div,/)
	OP(mod,%)
	OP(oand,&)
	OP(oor,|)
	OP(oxor,^)
	OP(shl,<<)
	OP(shr,>>)
	#undef OP
	#define CMP(n,o) bool operator o(const variable &v)	{return x->n(v.x);}
	CMP(eq,==)
	CMP(ne,!=)
	CMP(le,<=)
	CMP(ge,>=)
	CMP(lt,<)
	CMP(gt,>)
	#undef CMP
	variable operator+();
	rsv operator-()	{variable v = *this;v.x->neg();return v;}
	variable operator*()	{return x->deref();}
//	variable operator&();	// vPointerの為に使いたいが、これをやるのはちょっと…
	variable operator~();
	bool operator!()	{return !x->toBool();}
	variable &operator++();
	variable &operator--();
	variable operator++(int i);	// 後置
	variable operator--(int i);

	operator int()		const	{return x->toInt();}
	operator double()	const	{return x->toDouble();}
	operator bool()		const	{return x->toBool();}
	operator char()				const	{return x->toInt();}
	operator signed char()		const	{return x->toInt();}
	operator unsigned char()	const	{return x->toInt();}
	operator short()			const	{return x->toInt();}
	operator unsigned short()	const	{return x->toInt();}
	operator unsigned()			const	{return x->toInt();}
	operator long()				const	{return x->toInt();}
	operator unsigned long()	const	{return x->toInt();}
	operator __int64()			const	{return x->toInt();}
	operator unsigned __int64()	const	{return x->toInt();}
	operator float()		const	{return x->toDouble();}
	operator long double()	const	{return x->toDouble();}
	operator string()		const	{return x->toString();}
	string toString()		const	{return x->toString();}
	operator const char*()	const	{static string s;s = x->toString();return s.c_str();}

	variable operator[](int i)				{return x->index(i);}
	variable operator[](const char *s)		{return x->child(s);}
	variable operator[](const string &s)	{return x->child(s);}
	variable operator[](const variable &v)	{
		if (v.type(STRING) || v.type(FLOAT))	return x->child(v);
		else									return x->index(v);
	}
	size_t length() const				{return x->length();}
	bool exist(const string &s)			{return x->exist(s);}
	void push(const variable &v)		{return x->push(v.x);}
	variable keys()	{return x->keys();}
private:
	#include "PSLlib.h"
	class Variable
	{
	public:
		Variable()					{rc = 1;x = new vObject();}
//		Variable()					{rc = 1;x = new vBase();}
		Variable(int i)				{rc = 1;x = new vInt(i);}
		Variable(double d)			{rc = 1;x = new vFloat(d);}
//		Variable(const char *s)		{rc = 1;x = new vString(s);}
		Variable(const string &s)	{rc = 1;x = new vString(s);}
		Variable(function f)		{rc = 1;x = new vCFunction(f);}
		Variable(Variable *v)		{rc = 1;x = new vPointer(v);}
		Variable(Type t){rc = 1;switch (t){
			case NIL:		x = new vBase();break;
			case INT:		x = new vInt(0);break;
			case HEX:		x = new vHex(0);break;
			case FLOAT:		x = new vFloat(0);break;
			case STRING:	x = new vString("");break;
			case POINTER:	x = new vPointer();break;
			case REFERENCE: x = new vNReference();break;
			case RARRAY:	x = new vRArray();break;
			case THREAD:	x = new vThread();break;
			default:		x = new vObject();break;
		}}
		void finalize()		{if (!--rc)	delete this;}
		void safedelete()	{vBase *v = x;x = NULL;delete v;}
		Type type()	const	{return x->type();}

		void substitution(Variable *v)	{x = x->substitution(v);}
//		void assignment(Variable *v)	{x = v->x->clone();}
		void assignment(Variable *v)	{x = x->assignment(v);}

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
		Variable *deref()	{Variable *v = x->deref();return v ? v : this;}

		bool toBool()		const {return x->toBool();}
		int toInt()			const {return x->toInt();}
		double toDouble()	const {return x->toDouble();}
		string toString()	const {return x->toString();}

		size_t length()				const {return x->length();}
		bool exist(const string &s)	const {return x->exist(s);}
		void push(Variable *v)		{x->push(v);}
		Variable *index(size_t t)			{Variable *v = x->index(t);return v ? v : this;}
		Variable *child(const string &s)	{Variable *v = x->child(s);return v ? v : this;}
		Variable *keys()	{Variable *v = x->keys();v->rc = 0;return v;}
		bool set(const string &s, const variable &v)	{return x->set(s, v);}

	private:
		#include "environment.h"
		friend class Parser;
		friend class variable;
	public:
		void prepare(Environment &env)			{x->prepare(env, this);}
		void prepareInstance(Environment &env)	{x->prepareInstance(env, this);}
		variable call(Environment &env, variable &arg)	{return x->call(env, arg, this);}

		size_t codelength()		{return x->codelength();}
		Code *getcode()			{return x->getcode();}
		void pushcode(OpCode *c){return x->pushcode(c);}
		void pushlabel(const string &s){return x->pushlabel(s);}
		void write(const string &s, bytecode &b){x->write(s, b);}
	private:
		class vBase
		{
		public:
			vBase()	{}
			virtual ~vBase(){}
			virtual Type type()	const	{return NIL;}
			virtual vBase *clone()	{return new vBase();}
			virtual Variable *referenceTo()	{return NULL;}

			virtual vBase *substitution(Variable *v)	{delete this;return v->x->clone();}
			virtual vBase *assignment(Variable *v)		{delete this;return v->x->clone();}

			#define OP(n) virtual void n(Variable *v)	{}
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
			#define CMP(n) virtual bool n(Variable *v)	{return false;}
			CMP(eq)
			CMP(ne)
			CMP(le)
			CMP(ge)
			CMP(lt)
			CMP(gt)
			#undef CMP
			virtual void neg()	{}
			virtual Variable *deref()	{return NULL;}

			virtual bool toBool()		const {return false;}
			virtual int toInt()			const {return 0;}
			virtual double toDouble()	const {return 0;}
			virtual string toString()	const {return "";}

			virtual size_t length()				const {return 0;}
			virtual bool exist(const string &s)	const {return false;}
			virtual Variable *index(size_t t)			{return NULL;}
			virtual Variable *child(const string &s)	{return NULL;}
			virtual void push(Variable *v){}
			virtual Variable *keys()	{return new Variable();}
			virtual bool set(const string &s, const variable &v)	{return false;}

			virtual void prepare(Environment &env, Variable *v)	{}
			virtual void prepareInstance(Environment &env, Variable *v)	{v = new Variable(clone());env.push(v);v->finalize();}
			virtual rsv call(Environment &env, variable &arg, Variable *v)	{return variable(NIL);}

			virtual size_t codelength()	{return 0;}
			virtual Code *getcode()		{return NULL;}
			virtual void pushcode(OpCode *c){}
			virtual void pushlabel(const string &s){}
			virtual void write(const string &s, bytecode &b){}

			virtual void dump(){std::printf("vBase\n");}
		} *x;
	private:
		int rc;
		~Variable()	{delete x;}
	public:
		Variable(vBase *v)	{rc = 1;x = v;}
		Variable *clone()	{return new Variable(x->clone());}
		Variable *ref()		{++rc;return this;}
		void dump()		{std::printf("rc:%4d, ", rc);x->dump();}
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
		#include "memory.h"
		static void *operator new(size_t t)		{return VMemoryManager::Next();}
		static void operator delete(void *ptr)	{VMemoryManager::Release(ptr);}
#endif
	private:
		#include "vdata.h"
		friend class PSL;
		friend class vBase;
		friend class vRArray;
		friend class vObject;
		friend class vReference;
		friend class vNReference;
		#include "bytecode.h"
	} *x;
private:
	variable(Variable *v)	{x = v->ref();}
	variable(Type t, Variable *v)	{x = new Variable(v);}

	void prepare(Variable::Environment &env)			{x->prepare(env);}
	void prepareInstance(Variable::Environment &env)	{x->prepareInstance(env);}

	size_t codelength()					{return x->codelength();}
	Variable::Code *getcode()			{return x->getcode();}
	void pushcode(Variable::OpCode *c)	{return x->pushcode(c);}
	void pushlabel(const string &s)		{return x->pushlabel(s);}
	friend class PSL;
	friend class Tokenizer;
	friend class Parser;
	friend class Variable::CALL;
	friend class Variable::SCOPE;
	friend class Variable::LOOP;
	friend class Variable::INSTANCE;
	friend class Variable::bcreader;
public:
	rsv ref()		{return x;}
	rsv pointer()	{return variable(POINTER, x);}

	rsv operator()()											{Variable::Environment env;variable v;return x->call(env, v);}
	rsv operator()(variable &arg)								{Variable::Environment env;return x->call(env, arg);}
	rsv operator()(Variable::Environment &env, variable &arg)	{return x->call(env, arg);}
	// 一時環境で実行すると標準型すら存在しないことに…

	void dump()	{x->dump();}
};

#endif
