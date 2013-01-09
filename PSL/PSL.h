#ifndef PSL_H
#define PSL_H
/*Copyright (c) 2012 Silica
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.*/

#define PSL_VERSION 10101

// �ݒ荀��
//#define PSL_USE_STL_VECTOR
//#define PSL_USE_STL_STACK
//#define PSL_USE_STL_MAP
//#define PSL_NOT_SETLOCALE	// PSLVM��setlocale���Ȃ�
//#define PSL_NULL_RSV		// rsv��NULL�����e����ASTL_VECTOR���g���ꍇ�͕K�{

//#define PSL_DEBUG		// �s�ԍ��̖��ߍ��݂ƃX�e�b�v���s�̃T�|�[�g

#define PSL_WARNING_POP_EMPTY_STACK			// ��X�^�b�N�����POP��ʒm����
#define PSL_WARNING_STACK_REMAINED			// Environment��delete���ɃX�^�b�N���c���Ă��邱�Ƃ�ʒm����
//#define PSL_WARNING_UNDECLARED_IDENTIFIER	// ���錾�̕ϐ��̎g�p��ʒm����
//#define PSL_WARNING_DECLARED_IDENTIFIER_ALREADY_EXIST	// �錾�ς݂̖��O���X�ɐ錾�����ꍇ�ɒʒm����
//#define PSL_WARNING_UNDEFINED_MEMBER		// ����`�����o�ɃX�N���v�g����.�A�N�Z�X�����ۂɒʒm����

#define PSL_OPTIMIZE_TAILCALL				// �����Ăяo���̍œK��
#define PSL_OPTIMIZE_IN_COMPILE				// �R���p�C�����œK��(����ȉ�)�𗘗p����
#define PSL_OPTIMIZE_IMMEDIATELY_POP		// PUSH���đ�POP����R�[�h�̍œK��(a;�̗l�ȕ�)
#define PSL_OPTIMIZE_CONSTANT_CALCULATION	// �萔�v�Z�̍œK��(�Ȉ�)
#define PSL_OPTIMIZE_SUFFIX_INCREMENT		// ��u�C���N�������g�̒l�𗘗p���Ȃ��ꍇ�̑O�u�ւ̍œK��
#define PSL_OPTIMIZE_BOOL_AND				// �_��AND/OR�̍����Ō��ʂ����܂�ꍇ�ɉE����]�����Ȃ�(�Z���]��)
#define PSL_OPTIMIZE_PARENTHESES			// �v�Z������ς���ׂ�����()�����Z�q�ɂ��Ȃ�

#define PSL_POPSTACK_NULL	// Env�X�^�b�N��STL�łȂ����APOP�����X�^�b�N�𑦋�ɂ���(�ϐ��̐������Ԃɉe��)
//#define PSL_CHECKSTACK_POP	// POP���ɃX�^�b�N���`�F�b�N����
#define PSL_CHECKSTACK_PUSH	// PUSH���ɃX�^�b�N���`�F�b�N����(���Ȃ��ꍇ�Œ蒷�X�^�b�N�ō����ɓ��삷��)
//#define PSL_CHECK_SCOPE_NEST	// ���s�X�R�[�v�̃l�X�g�̐[�����`�F�b�N����(��O�g�p)
//#define PSL_MEMBER_REGISTER		// �����o�A�N�Z�X�����ۂɐe�����W�X�^�ɕۑ�����(���\�b�h�`�F�[���p)

#define PSL_IF_STATEMENT_NOT_SCOPE			// if�����X�R�[�v�����Ȃ�

#define PSL_USE_VARIABLE_MEMORY_MANAGER		// Variable�p�I���I���������}�l�[�W�� PSL���C�u�����֐�GarbageCollection�𗘗p�\�ɂȂ�
#define PSL_SHARED_GLOBAL					// global�ϐ���S�Ă̊��ŋ��ʂɂ���

#define PSL_USE_TOKENIZER_DEFINE			// #define�̎g�p��
#define PSL_USE_CONSOLE						// std::printf���g��
#define PSL_USE_DESTRUCTOR					// PSL���̃N���X�̃f�X�g���N�^���g��

#define PSL_CLOSURE_REFERENCE				// �N���[�W�����R�s�[�ł͂Ȃ��Q�Ƃɂ���

//#define PSL_THREAD_SAFE						// �������}�l�[�W���g��Ȃ��Aglobal���L���Ȃ��Astatic�ϐ����g��Ȃ�
// �����܂�

#ifdef PSL_USE_CONSOLE
	#define PSL_PRINTF(x) std::printf x
#else
	#define PSL_PRINTF(x)
#endif



#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <clocale>
#include <ctime>
#include <cmath>

#ifdef PSL_USE_STL_VECTOR
#include <vector>
#include <stack>
#endif
#ifdef PSL_USE_STL_STACK
#include <stack>
#endif
#ifdef PSL_USE_STL_MAP
#include <map>
#endif
#include <iterator>

namespace PSL {
const static int version = PSL_VERSION;
using std::size_t;
#include "variable.h"
typedef variable::string string;
typedef variable::string::wstring wstring;

class PSLVM
{
	typedef variable::rsv rsv;
	variable::Environment env;
	PSLVM(const PSLVM&);
	void operator=(const PSLVM&);
public:
	enum error
	{
		NONE = 0,
		FOPEN_ERROR,
		PARSE_ERROR,
		NOT_COMPILED_CODE,
	};
	error loadString(const string &str, const string &label = "loadString")
	{
		variable::Tokenizer t(str, label, 1);
		return parse(&t) ? PARSE_ERROR : NONE;
	}
	error loadScript(const string &filename)
	{
		if (!loadCompiledCode(filename))
			return NONE;
		variable::Tokenizer *t = variable::Tokenizer::New(filename);
		if (!t)
			return FOPEN_ERROR;

		bool e = parse(t);
		delete t;

		if (e)
			return PARSE_ERROR;
		return NONE;
	}
	error writeCompiledCode(std::FILE *fp)
	{
		variable::Variable::bytecode b;
		env.global.get()->write("", b);
		unsigned long l = 0xBCDEF01A;
		fwrite(&l, 1, sizeof(l), fp);
		fwrite(b.get(), 1, b.size(), fp);
		return NONE;
	}
	error writeCompiledCode(const string &filename)
	{
		using namespace std;
		FILE *fp = fopen(filename, "wb");
		if (!fp)
			return FOPEN_ERROR;
		writeCompiledCode(fp);
		fclose(fp);
		return NONE;
	}
	error writeCompiledCode(variable::buffer &buf)
	{
		env.global.get()->write("", buf);
		return NONE;
	}
	error loadCompiledCode(std::FILE *fp, unsigned long size)
	{
		unsigned long l;
		fread(&l, 1, sizeof(l), fp);
		if (l != 0xBCDEF01A)
			return NOT_COMPILED_CODE;
		size -= sizeof(l);
		variable::Variable::bytecode b(size);
		fread(b.get(), 1, size, fp);
		return loadCompiledCode(b);
	}
	error loadCompiledCode(const string &filename)
	{
		using namespace std;
		FILE *fp = fopen(filename, "rb");
		if (!fp)
			return FOPEN_ERROR;
		fseek(fp, 0, SEEK_END);
		long size = ftell(fp);
		if (size < 0)
			size = 0;
		fseek(fp, 0, SEEK_SET);
		error e = loadCompiledCode(fp, static_cast<unsigned long>(size));
		fclose(fp);
		return e;
	}
	error loadCompiledCode(variable::buffer &buf)
	{
		variable c;
		variable::Variable::bcreader::read(buf, c);
		variable g = env.global;
		g.gset(c);
		g.prepare(env);
		return NONE;
	}
	error loadCompiledCode(void *buf, unsigned long size)
	{
		variable::Variable::bytecode b;
		b.push(buf, size);
		return loadCompiledCode(b);
	}
	rsv run(const variable &arg = 0)
	{
		if (!env.Runable())
			return rsv();
		#ifdef PSL_DEBUG
		if (!init)
		#endif
		env.push(arg);
		env.Run();
		return env.pop();
	}
	PSLVM() : env(1)
	{
	#ifndef PSL_NOT_SETLOCALE
		std::setlocale(LC_ALL, "");
	#endif
	#ifdef PSL_DEBUG
		init = false;
	#endif
	}
	#ifdef PSL_DEBUG
	rsv stepExec()
	{
		if (!env.Runable())
		{
			if (env.stack.size())
				return env.pop();
			return variable(1);
		}
		if (!init)
		{
			init = true;
			env.push(rsv());
		}
		env.StepExec();
		return rsv();
	}
	#endif
	rsv get(const string &name)							{return env.global.get()->child(name);}
	void add(const string &name, const variable &v)		{env.global.get()->set(name, v);}
	void add(const string &name, variable::function f)	{env.global.get()->set(name, variable(f));}
	void add(const string &name, variable::method f)	{env.global.get()->set(name, variable(f));}
	#ifndef PSL_SHARED_GLOBAL
	operator variable::Environment&()	{return env;}
	variable call(variable &f, variable &arg = variable())	{return f(env, arg);}
	#endif
private:
	bool parse(variable::Tokenizer *t)
	{
		variable::Parser p(t);

		variable g;
		p.Parse(g);
		if (p.getErrorNum())
			return true;

		variable gl = env.global;
		gl.gset(g);
		gl.prepare(env);
		return false;
	}
	#ifdef PSL_DEBUG
	bool init;
	#endif
	class addf
	{
		PSLVM *p;
	public:
		addf(PSLVM *x){p = x;}
		template<class F>addf &operator()(const string &s, F f)
		{
			p->addFunction(s, f);
			return *this;
		}
	};
	template<class C>class addc
	{
		rsv g;
		rsv r;
	public:
		addc(rsv &gl, variable &v){g = gl;r = v;}
		template<class M>addc &operator()(const string &s, M m)
		{
			variable c = r;
			variable v(variable::Method<C>(), m);
			c.set(s, v);
			return *this;
		}
		addc &instance(const string &s, C *p)
		{
			variable c = r;
			variable i = c.instance();
			i.push(p);
			variable n = g;
			n.set(s, i);
			return *this;
		}
	};
public:
	template<class F>addf addFunction(const string &s, F f)
	{
		variable g = env.global;
		variable v(variable::Function(), f);
		g.set(s, v);
		return addf(this);
	}
	template<class C>addc<C> addClass(const string &s)
	{
		variable g = env.global;
		variable v;
		g.set(s, v);
		return addc<C>(env.global, v);
	}
	template<class C>addc<C> addInstance(const string &classname, const string &s, C *p)
	{
		variable g = env.global;
		variable v = g[classname];
		variable i = v.instance();
		i.push(p);
		g.set(s, i);
		return addc<C>(env.global, v);
	}
};

}
#endif
