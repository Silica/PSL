#include "tokenizer.h"
#include "parser.h"

class PSL : private variable::Variable::Environment
{
public:
	enum error
	{
		NONE,
		FOPEN_ERROR,
		PARSE_ERROR,
		NOT_COMPILED_CODE,
	};
	error LoadScript(const char *filename)
	{
		Tokenizer *t = Tokenizer::New(filename);
		if (!t)
			return FOPEN_ERROR;

		bool e = parse(t);
		delete t;

		if (e)
			return PARSE_ERROR;
		return NONE;
	}
	error WriteCompiledCode(std::FILE *fp)
	{
		variable::bytecode b;
		global.get()->write("", b);
		unsigned long l = 0xBCDEF01A;
		fwrite(&l, 1, sizeof(l), fp);
		fwrite(b.get(), 1, b.size(), fp);
		return NONE;
	}
	error WriteCompiledCode(const char *filename)
	{
		using namespace std;
		FILE *fp = fopen(filename, "wb");
		if (!fp)
			return FOPEN_ERROR;
		WriteCompiledCode(fp);
		fclose(fp);
		return NONE;
	}
	error LoadCompiledCode(std::FILE *fp, unsigned long size)
	{
		variable cc = global;
		unsigned long l;
		fread(&l, 1, sizeof(l), fp);
		if (l != 0xBCDEF01A)
			return NOT_COMPILED_CODE;
		size -= sizeof(l);
		variable::bytecode b(size);
		fread(b.get(), 1, size, fp);
		LoadLibrary();
		variable::Variable::bcreader::read(b, cc);
		cc.prepare(*this);
		return NONE;
	}
	error LoadCompiledCode(const char *filename)
	{
		using namespace std;
		FILE *fp = fopen(filename, "rb");
		if (!fp)
			return FOPEN_ERROR;
		fseek(fp, 0, SEEK_END);
		int size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		error e = LoadCompiledCode(fp, size);
/*		unsigned long l;
		fread(&l, 1, sizeof(l), fp);
		if (l != 0xBCDEF01A)
			return NOT_COMPILED_CODE;
		fseek(fp, 0, SEEK_END);
		int size = ftell(fp) - sizeof(l);
		variable::bytecode b(size);
		fseek(fp, sizeof(l), SEEK_SET);
		fread(b.get(), 1, size, fp);*/
		fclose(fp);
/*		LoadLibrary();
		variable::Variable::bcreader::read(b, cc);
		cc.prepare(*this);*/
		return e;
	}
	variable Run(void)
	{
		if (!Runable())
			return variable();
		variable::Variable::Environment::Run();
//		std::printf("stack:%d\n", stack.size());
		return pop();
	}
	variable::rsv get(const string &name)
	{
		return global.get()->child(name);
	}
	operator variable::Variable::Environment&()
	{
		return *this;
	}
private:
	static variable Print(variable &v)
	{
		switch (v.type())
		{
		case variable::NIL:			std::printf("nil");break;
		case variable::INT:			std::printf("%d", (int)v);break;
		case variable::HEX:			std::printf("%X", (int)v);break;
		case variable::FLOAT:		std::printf("%f", (double)v);break;
//		case variable::STRING:		{string s = v;std::printf("%s", (const char*)s);break;}
//		case variable::RARRAY:		std::printf("[rarray:%d]", v.length());break;
//		case variable::OBJECT:		std::printf("[object]");break;
		case variable::METHOD:		std::printf("[method]");break;
		case variable::CFUNCTION:	std::printf("[cfunc]");break;
		case variable::THREAD:		std::printf("[thread:%s]", (bool)v ? "alive" : "dead");break;
		default://				v.dump();
			{
				string s = v;
				std::printf("%s", (const char*)s);
				break;
			}
		}
		return v;
	}
	static variable Debug(variable &v)
	{
		v.dump();
		return v;
	}
	void LoadLibrary()
	{
		variable g = global;
		g["print"] = Print;
		g["debug"] = Debug;
		g["int"] = 0;
		g["float"] = 0.0;
		g["string"] = "";
		g["true"] = 1;
		g["false"] = 0;
		variable hex = variable::HEX;
		variable ref = variable::REFERENCE;
		variable pointer = variable::POINTER;
		variable thread = variable::THREAD;
		g["hex"] = hex;
		g["ref"] = ref;
		g["pointer"] = pointer;
		g["thread"] = thread;
	}
	bool parse(Tokenizer *t)
	{
		Parser p(t);
		LoadLibrary();

		variable g = global;
		p.Parse(g);
		if (p.getErrorNum())
			return true;

//		std::printf("stack:%d\n", stack.size());
//		g.dump();

		global.get()->prepare(*this);
		return false;
	}
};
