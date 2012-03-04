#ifndef VARIABLE_H
#define VARIABLE_H

#include "pstring.h"

#include <cstring>
#include <vector>
//#include <map>
//#include <stack>
//#define PSL_USE_STL_VECTOR
//#define PSL_USE_STL_STACK
//#define PSL_USE_STL_MAP
	#define PSL_USE_HASH_MAP
//#define PSL_NULL_RSV		// rsv��NULL�����e����ASTL_VECTOR���g���ꍇ�͕K�{

//#define PSL_DEBUG

#define PSL_WARNING_POP_EMPTY_STACK			// ��X�^�b�N�����POP��ʒm����
#define PSL_WARNING_STACK_REMAINED			// Environment��delete���ɃX�^�b�N���c���Ă��邱�Ƃ�ʒm����
//#define PSL_WARNING_UNDECLARED_IDENTIFIER	// ���錾�̕ϐ��̎g�p��ʒm����
//#define PSL_WARNING_DECLARED_IDENTIFIER_ALREADY_EXIST	// �錾�ς݂̖��O���X�ɐ錾�����ꍇ�ɒʒm����

#define PSL_OPTIMIZE_TAILCALL				// �����Ăяo���̍œK��
#define PSL_OPTIMIZE_IN_COMPILE				// �R���p�C�����œK��(����ȉ�)�𗘗p����
#define PSL_OPTIMIZE_IMMEDIATELY_POP		// PUSH���đ�POP����R�[�h�̍œK��(a;�̗l�ȕ�)
#define PSL_OPTIMIZE_CONSTANT_CALCULATION	// �萔�v�Z�̍œK��(�Ȉ�)
#define PSL_OPTIMIZE_SUFFIX_INCREMENT		// ��u�C���N�������g�̒l�𗘗p���Ȃ��ꍇ�̑O�u�ւ̍œK��
#define PSL_OPTIMIZE_BOOL_AND				// �_��AND/OR�̍����Ō��ʂ����܂�ꍇ�ɉE����]�����Ȃ�
// ���̑��ȒP�ɉ\�ȍœK����
// �W�����v�������萔�̏ꍇ
// �f�b�h�R�[�h�̍폜(return/break/continue��̃R�[�h�A��̒萔�]���W�����v�̍l��)
// ������֐��Ăяo�����Ȃ������͖������Ă��\��Ȃ��H�C���N�������g����

//#define PSL_USE_VARIABLE_MEMORY_MANAGER

#define PSL_USE_TOKENIZER_DEFINE

#ifdef _DEBUG
	#include <new>
#endif

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
	typedef variable(*method)(variable&,variable&);
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
		CMETHOD,
		CPOINTER,
	};


	variable()					{x = new Variable();}
	variable(bool b)			{x = new Variable((int)b);}
	variable(int i)				{x = new Variable(i);}
	variable(double d)			{x = new Variable(d);}
	variable(const char *s)		{x = new Variable(s);}
	variable(const string &s)	{x = new Variable(s);}
	variable(function f)		{x = new Variable(f);}
	variable(method f)			{x = new Variable(f);}
	variable(void *p)			{x = new Variable(p);}
	variable(const variable &v)	{x = v.x->clone();}
	variable(Type t)			{x = new Variable(t);}
	~variable()					{x->finalize();}

	Type type()			const	{return x->type();}
	bool type(Type t)	const	{return x->type() == t;}

	variable &substitution(const variable &v)	{x->substitution(v.x);return *this;}
	variable &assignment(const variable &v)		{x->assignment(v.x);return *this;}
	variable &operator=(const variable &v)		{x->substitution(v.x);return *this;}
	variable &operator->*(const variable &v)	{x->assignment(v.x);return *this;}		// �����]���Ă鉉�Z�q������Ȃ̂Łc

	variable &operator=(function f)		{
		if (f == NULL)
		{
			variable v = 0;
			x->substitution(v.x);
		}
		else
		{
			variable v = f;
			x->substitution(v.x);
		}
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
//	variable operator&();	// vPointer�ׂ̈Ɏg���������A��������̂͂�����Ɓc
	variable operator~();
	bool operator!()	{return !x->toBool();}
	variable &operator++();
	variable &operator--();
	variable operator++(int i);	// ��u
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
	operator void*()		const	{return x->toPointer();}

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
		Variable(method f)			{rc = 1;x = new vCMethod(f, NULL);}
		Variable(void *p)			{rc = 1;x = new vCPointer(p);}
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
		void *toPointer()	const {return x->toPointer();}

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
		Variable *instance()					{return x->instance(this);}
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
			virtual void *toPointer()	const {return NULL;}

			virtual size_t length()				const {return 0;}
			virtual bool exist(const string &s)	const {return false;}
			virtual Variable *index(size_t t)			{return NULL;}
			virtual Variable *child(const string &s)	{return NULL;}
			virtual void push(Variable *v){}
			virtual Variable *keys()	{return new Variable();}
			virtual bool set(const string &s, const variable &v)	{return false;}

			virtual void prepare(Environment &env, Variable *v)	{}
			virtual void prepareInstance(Environment &env, Variable *v)	{env.push(rsv(v->clone(), 0));}
			virtual Variable *instance(Variable *v)	{return v->clone();}
			virtual rsv call(Environment &env, variable &arg, Variable *v)	{return variable(NIL);}

			virtual size_t codelength()	{return 0;}
			virtual Code *getcode()		{return NULL;}
			virtual void pushcode(OpCode *c){}
			virtual void pushlabel(const string &s){}
			virtual void write(const string &s, bytecode &b){}

		#ifdef PSL_DEBUG
			virtual void dump(){std::printf("vBase\n");}
		#endif
		} *x;
	private:
		int rc;
		~Variable()	{rc=0x80000000;delete x;}
	public:
		Variable(vBase *v)	{rc = 1;x = v;}
		Variable *clone()	{return new Variable(x->clone());}
		Variable *ref()		{++rc;return this;}
	#ifdef PSL_DEBUG
		void dump()		{std::printf("rc:%4d, ", rc);x->dump();}
	#endif
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
	friend class Variable::vCMethod;
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
	rsv instance()												{return rsv(x->instance(), 0);}

	#ifdef PSL_DEBUG
	void dump()	{x->dump();}
	#endif
};

#endif
