#ifndef VARIABLE_H
#define VARIABLE_H

#include "pstring.h"

//#include <vector>
//#include <map>
//#include <stack>

// �ݒ荀��
//#define PSL_USE_STL_VECTOR
//#define PSL_USE_STL_STACK
//#define PSL_USE_STL_MAP
//#define PSL_NULL_RSV		// rsv��NULL�����e����ASTL_VECTOR���g���ꍇ�͕K�{

//#define PSL_DEBUG		// �s�ԍ��̖��ߍ��݂ƃX�e�b�v���s�̃T�|�[�g

#define PSL_WARNING_POP_EMPTY_STACK			// ��X�^�b�N�����POP��ʒm����
#define PSL_WARNING_STACK_REMAINED			// Environment��delete���ɃX�^�b�N���c���Ă��邱�Ƃ�ʒm����
//#define PSL_WARNING_UNDECLARED_IDENTIFIER	// ���錾�̕ϐ��̎g�p��ʒm����
//#define PSL_WARNING_DECLARED_IDENTIFIER_ALREADY_EXIST	// �錾�ς݂̖��O���X�ɐ錾�����ꍇ�ɒʒm����

#define PSL_OPTIMIZE_TAILCALL				// �����Ăяo���̍œK��
#define PSL_OPTIMIZE_IN_COMPILE				// �R���p�C�����œK��(����ȉ�)�𗘗p����
#define PSL_OPTIMIZE_IMMEDIATELY_POP		// PUSH���đ�POP����R�[�h�̍œK��(a;�̗l�ȕ�)
#define PSL_OPTIMIZE_CONSTANT_CALCULATION	// �萔�v�Z�̍œK��(�Ȉ�)
#define PSL_OPTIMIZE_SUFFIX_INCREMENT		// ��u�C���N�������g�̒l�𗘗p���Ȃ��ꍇ�̑O�u�ւ̍œK��
#define PSL_OPTIMIZE_BOOL_AND				// �_��AND/OR�̍����Ō��ʂ����܂�ꍇ�ɉE����]�����Ȃ�(�Z���]��)
#define PSL_OPTIMIZE_PARENTHESES			// �v�Z������ς���ׂ�����()�����Z�q�ɂ��Ȃ�
// ���̑��ȒP�ɉ\�ȍœK����
// �W�����v�������萔�̏ꍇ(�p�[�T�̓s���Ŋy����Ȃ������c�֐�������ɂ��Ȃ炢���񂾂�)
// �f�b�h�R�[�h�̍폜(return/break/continue��̃R�[�h�A��̒萔�]���W�����v�̍l��)
// ������֐��Ăяo�����Ȃ������͖������Ă��\��Ȃ��H�C���N�������g����
#define PSL_POPSTACK_NULL	// Env�X�^�b�N��STL�łȂ����APOP�����X�^�b�N�𑦋�ɂ���(�ϐ��̐������Ԃɉe��)

#define PSL_USE_VARIABLE_MEMORY_MANAGER		// Variable�p�I���I���������}�l�[�W��
/*	���x�͖��m��
	PSL���C�u�����֐�GarbageCollection�𗘗p�\�ɂȂ�
	�z�Q�Ƃ��N������Variable�̎���vBase�h�����A�v���P�[�V�����I�����ɂ͊J���o����
	��PSL�N���X�̃f�X�g���N�^���m���ɌĂׂ�A�l�ȋC������	*/
#define PSL_MEMORY_MANAGER_SLIM				// ���O�v�[�����ȊO�ɖ��ʂȗe�ʂ�����Ȃ��o�[�W�������g�p����
//#define PSL_MEMORY_MANAGER_LARGE				// �]�v�ȗe�ʂ͑����邪��ʊm�ێ��ł����x�������Ȃ�
// �ǂ����OFF�Ȃ班���e�ʂ͑����邪GarbageCollection��������������
#define PSL_SHARED_GLOBAL					// global�ϐ���S�Ă̊��ŋ��ʂɂ���

#define PSL_USE_TOKENIZER_DEFINE			// #define�̎g�p��
#define PSL_USE_CONSOLE						// std::printf���g��
// �����܂�



#ifdef PSL_USE_CONSOLE
	#define PSL_PRINTF(x) std::printf x
#else
	#define PSL_PRINTF(x)
#endif
#ifdef PSL_DEBUG
	#define PSL_DUMP(x) void dump x
#else
	#define PSL_DUMP(x)
#endif
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	#define PSL_MEMORY_MANAGER(x) static void *operator new(size_t t){return MemoryManager<sizeof(x)>::Next();}static void operator delete(void *ptr){MemoryManager<sizeof(x)>::Release(ptr);}
#else
	#define PSL_MEMORY_MANAGER(x)
#endif
#if defined(PSL_USE_VARIABLE_MEMORY_MANAGER) && !defined(PSL_SHARED_GLOBAL)
	#define PSL_TEMPORARY_ENV(x) Environment &x = StaticObject::envtemp()
	#define PSL_TEMPORARY_ENV0(x) Environment &x = StaticObject::envtemp()
#else
	#define PSL_TEMPORARY_ENV(x) Environment x
	#define PSL_TEMPORARY_ENV0(x) Environment x(0)
#endif


class variable
{
	typedef unsigned int size_t;
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
		RARRAY,
		OBJECT,
		METHOD,
		CFUNCTION,
		CMETHOD,
		CPOINTER,
		THREAD,
	};

private:
	class Variable;
	#include "container.h"
	friend class rsv;
public:

	variable()					{x = new Variable();}
	variable(bool b)			{x = new Variable((int)b);}
	variable(int i)				{x = new Variable(i);}
	variable(unsigned u)		{x = new Variable(u);}
	variable(hex h)				{x = new Variable(h);}
	variable(double d)			{x = new Variable(d);}
	variable(const char *s)		{x = new Variable(s);}
	variable(const string &s)	{x = new Variable(s);}
	variable(function f)		{x = new Variable(f);}
	variable(method f)			{x = new Variable(f);}
	variable(void *p)			{x = new Variable(p);}
	variable(const variable &v)	{x = v.x->clone();}
	variable(Type t)			{x = new Variable(t);}
	variable(const rsv &v)		{x = v.get()->ref();}
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
		else if ((int)v < 0)					return x;
		else									return x->index(v);
	}
	size_t length() const				{return x->length();}
	bool exist(const string &s)			{return x->exist(s);}
	void push(const variable &v)		{return x->push(v.x);}
	variable keys()	{return x->keys();}
	bool set(const string &s, const variable &v)	{return x->set(s, v);}
	void del(const string &s)						{return x->del(s);}
private:
	#include "PSLlib.h"
	#include "tokenizer.h"
	#include "parser.h"
	class Variable
	{
	public:
		Variable()					{rc = 1;x = new vObject();}
//		Variable()					{rc = 1;x = new vBase();}
		Variable(int i)				{rc = 1;x = new vInt(i);}
		Variable(unsigned u)		{rc = 1;x = new vInt(u);}
		Variable(hex h)				{rc = 1;x = new vHex(h);}
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
		void finalize()		{if (!--rc)	{rc=0x80000000;x->destructor();delete this;}}
		void safedelete()	{rsv v(new Variable(x), 0);x = NULL;x = new vInt(0);}
		bool searchcount(Variable *v, int &c)
		{
			if (rc & 0x40000000)
				return true;
			if (rc & 0x80000000)
			{
				if (v == this)
					++c;
				return false;
			}
			rc |= 0x80000000;
			x->searchcount(v, c);
			return false;
		}
		void markstart(int c)
		{
			if ((rc & 0xFFFFFF) != c)
				mark();
		}
		void mark()
		{
			if (rc & 0x40000000)
				return;
			rc |= 0x40000000;
			x->mark();
		}
		void unmark(unsigned long m)
		{
			rc &= m;
		}
		void destructor_unmark()
		{
			if (rc & 0x40000000)
				return;
			x->destructor();
		}
		void delete_unmark()
		{
			if (rc & 0x40000000)
				return;
			safedelete();
		}
		Type type()	const	{return x->type();}

		void substitution(Variable *v)	{x = x->substitution(v);x->method_this(this);}
		void assignment(Variable *v)	{x = x->assignment(v);x->method_this(this);}

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
		void del(const string &s)						{x->del(s);}

	private:
		#include "bytecode.h"
		#include "environment.h"
		friend class Parser;
		friend class variable;
	public:
		void prepare(Environment &env)			{x->prepare(env, this);}
		void prepareInstance(Environment &env)	{x->prepareInstance(env, this);}
		Variable *instance()					{return x->instance(this);}
		variable call(Environment &env, variable &arg)	{return x->call(env, arg, this);}

		Variable(Code *c)		{rc = 1;x = new vObject(c);}
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
			virtual void searchcount(Variable *v, int &c){}
			virtual void mark(){}
			virtual void destructor(){}

			virtual vBase *substitution(Variable *v)	{vBase *x = v->bclone();delete this;return x;}
			virtual vBase *assignment(Variable *v)		{vBase *x = v->bclone();delete this;return x;}

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
			virtual void del(const string &s)	{}
			virtual void method_this(Variable *v)	{}

			virtual void prepare(Environment &env, Variable *v)	{}
			virtual void prepareInstance(Environment &env, Variable *v)	{env.push(rsv(v->clone(), 0));}
			virtual Variable *instance(Variable *v)	{return v->clone();}
			virtual rsv call(Environment &env, variable &arg, Variable *v)	{return variable(NIL);}

			virtual size_t codelength()	{return 0;}
			virtual Code *getcode()		{return NULL;}
			virtual void pushcode(OpCode *c){}
			virtual void pushlabel(const string &s){}
			virtual void write(const string &s, bytecode &b){}

			virtual void dump(){PSL_PRINTF(("vBase\n"));}
		} *x;
	private:
		int rc;
		~Variable()	{delete x;}
	public:
		Variable(vBase *v)	{rc = 1;x = v;x->method_this(this);}
		vBase *bclone()		{return x->clone();}
		Variable *clone()	{return new Variable(x->clone());}
		Variable *ref()		{++rc;return this;}
		PSL_DUMP((){PSL_PRINTF(("rc:%4d, ", rc));x->dump();})
#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
		static void *operator new(size_t t)		{return VMemoryManager::Next();}
		static void operator delete(void *ptr)	{VMemoryManager::Release(ptr);}
#endif
	private:
		#include "vdata.h"
		friend class PSL;
		friend class vRArray;
		friend class vReference;
	} *x;
#ifdef __GNUC__
public:
#else
private:
#endif
	typedef Variable::Environment Environment;
private:
	#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	#include "memory.h"
	#endif
	variable(Variable *v)	{x = v->ref();}
	variable(Type t, Variable *v)	{x = new Variable(v);}

	void prepare(Environment &env)			{x->prepare(env);}
	void prepareInstance(Environment &env)	{x->prepareInstance(env);}

	size_t codelength()					{return x->codelength();}
	Variable::Code *getcode()			{return x->getcode();}
	void pushcode(Variable::OpCode *c)	{return x->pushcode(c);}
	void pushlabel(const string &s)		{return x->pushlabel(s);}
	friend class PSL;
	friend class Parser;
	friend class Variable::vCMethod;
	friend class Variable::CALL;
	friend class Variable::INSTANCE;
	friend class Variable::bcreader;
	friend class Variable::Code;
public:
	variable(variable::Variable::Code *c)	{x = new Variable(c);}
	rsv ref()		{return x;}
	rsv pointer()	{return variable(POINTER, x);}

	rsv operator()()									{PSL_TEMPORARY_ENV(env);variable v;return x->call(env, v);}
	rsv operator()(variable &arg)						{PSL_TEMPORARY_ENV(env);return x->call(env, arg);}
	#ifndef PSL_SHARED_GLOBAL
	rsv operator()(Environment &env, variable &arg)		{return x->call(env, arg);}
	#endif
	rsv instance()										{return rsv(x->instance(), 0);}
	#define cva(n) const variable &arg##n
	#define ap(n) arg.push(arg##n);
	#define CALL(z,y) rsv operator()z{variable arg = RARRAY;y PSL_TEMPORARY_ENV(env);return x->call(env, arg);}
	CALL((cva(1),cva(2)),								ap(1)ap(2))
	CALL((cva(1),cva(2),cva(3)),						ap(1)ap(2)ap(3))
	CALL((cva(1),cva(2),cva(3),cva(4)),					ap(1)ap(2)ap(3)ap(4))
	CALL((cva(1),cva(2),cva(3),cva(4),cva(5)),			ap(1)ap(2)ap(3)ap(4)ap(5))
	CALL((cva(1),cva(2),cva(3),cva(4),cva(5),cva(6)),	ap(1)ap(2)ap(3)ap(4)ap(5)ap(6))
	#undef cva
	#undef ap
	#undef CALL

	PSL_DUMP((){x->dump();})
};

#endif
