class PSLlib
{
public:
	static void Basic(const rsv &r)
	{
		variable v = r;
		v.set("int", 0);
		v.set("float", 0.0);
		v.set("string", "");
		v.set("true", 1);
		v.set("false", 0);
		v.set("nil", NIL);
		v.set("hex", HEX);
		v.set("ref", REFERENCE);
		v.set("pointer", POINTER);
		v.set("nullptr", POINTER);
		v.set("thread", THREAD);
		v["object"];
		v["null"];

		v["print"] = Print;
		v["type"] = Type;
		#ifdef PSL_DEBUG
		v["debug"] = Debug;
		#endif
	}
	static void Standard(const rsv &r)
	{
		variable v = r;
		v["range"] = Range;
		v["foreach"] = Foreach;
		v["new"] = New;
		v["eval"] = Eval;
		#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
		v["GarbageCollection"] = GarbageCollection;
		#endif
		Strlib::set(v["strlib"]);
		Array::set(v["array"]);
		Table::set(v["table"]);
		File::set(v["file"]);
		Binary::set(v["binary"]);
		Time::set(v["time"]);
		Math::set(v["math"]);
	}
private:
	static variable Print(variable &v)
	{
		switch (v.type())
		{
		case NIL:		PSL_PRINTF(("nil"));break;
		case INT:		PSL_PRINTF(("%d", (int)v));break;
		case HEX:		PSL_PRINTF(("%X", (int)v));break;
		case FLOAT:		PSL_PRINTF(("%f", (double)v));break;
//		case RARRAY:	PSL_PRINTF(("[tuple:%d]", v.length()));break;
		case THREAD:	PSL_PRINTF(("[thread:%s]", (bool)v ? "alive" : "dead"));break;
		default:
			{
				PSL_PRINTF(("%s", v.toString().c_str()));
				break;
			}
		}
		return v;
	}
	static variable Type(variable &v)
	{
		switch (v.type())
		{
		case NIL:		return "nil";
		case INT:		return "int";
		case HEX:		return "hex";
		case FLOAT:		return "float";
		case STRING:	return "string";
		case POINTER:	return "pointer";
		case REFERENCE:	return "ref";
		case RARRAY:	return "tuple";
		case OBJECT:	return "object";
		case METHOD:	return "method";
		case CFUNCTION:	return "Cfunction";
		case CMETHOD:	return "Cmethod";
		case CPOINTER:	return "Cpointer";
		case THREAD:	return "thread";
		}
		return "";
	}
	#ifdef PSL_DEBUG
	static variable Debug(variable &v)
	{
		v.dump();
		return v;
	}
	#endif
	static variable Range(variable &v)
	{
		variable l;
		if (v.length() == 1)
		{
			int len = v;
			for (int i = len; --i >= 0;)
				l[i] = i;
		}
		else if (v.length() == 2)
		{
			int start = v[0];
			int end = v[1];
			int len = end - start;
			for (int i = len; --i >= 0;)
				l[i] = --end;
		}
		else if (v.length() > 2)
		{
			int start = v[0];
			int end = v[1];
			int step = v[2];
			if (!step)
				step = start < end ? 1 : -1;
			if (step > 0)
				while (start < end)
				{
					l.push(start);
					start += step;
				}
			else
				while (start > end)
				{
					l.push(start);
					start += step;
				}
		}
		return l;
	}
	static variable Foreach(variable &v)
	{
		variable l = v[0];
		variable f = v[1];
		int size = l.length();
		if (size)
		{
			for (int i = 0; i < size; ++i)
			{
				variable a = l[i];
				f(a);
			}
			return size;
		}
		else
		{
			variable k = l.keys();
			size = k.length();
			for (int i = 0; i < size; ++i)
				f(k[i], l[k[i]]);
			return k;
		}
	}
	static variable New(variable &v)
	{
		variable x = v.instance();
		return x.pointer();
	}
	static variable Eval(variable &v)
	{
		string s = v;
		variable g;
		Tokenizer t(s.c_str(), "eval", 0);
		Parser p(&t);
		p.Parse(g);
		return g;
	}
	#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
	static variable GarbageCollection(variable &v)
	{
		StaticObject::vpool().GarbageCollection();
		return variable();
	}
	#endif
	class Strlib
	{
	public:
		static void set(const rsv &r)
		{
			variable v = r;
			v["char"] = Char;
			v["ctoi"] = ctoi;
			v["length"] = Strlen;
			v["getchar"] = getChar;
		}
	private:
		static variable Char(variable &v)
		{
			char c = v;
			string s = c;
			variable ch = s;
			return ch;
		}
		static variable Strlen(variable &v)
		{
			string s = v;
			return s.length();
		}
		static variable getChar(variable &v)
		{
			string s;
			using namespace std;
			char c = getchar();
			s = c;
			variable ch = s;
			return ch;
		}
		static variable ctoi(variable &v)
		{
			string s = v;
			return s[0];
		}
	};
	class Array
	{
	public:
		static void set(const rsv &r)
		{
			variable v = r;
			v["length"] = variable(Length);
			v["push"] = variable(Push);
			v["foreach"] = variable(Foreach);
		}
	private:
		static variable Length(variable &this_v, variable &v)
		{
			if (!this_v)	return v.length();
			else		return this_v.length();
		}
		static variable Push(variable &this_v, variable &v)
		{
			if (!this_v)
			{
				v[0].push(v[1]);
				return v[0].length();
			}
			else
			{
				this_v.push(v);
				return this_v.length();
			}
		}
		static variable Foreach(variable &this_v, variable &v)
		{
			int size;
			if (!this_v)
			{
				variable array = v[0];
				size = array.length();
				for (int i = 0; i < size; ++i)
				{
					variable a = array[i];
					v[1](a);
				}
			}
			else
			{
				size = this_v.length();
				for (int i = 0; i < size; ++i)
				{
					variable a = this_v[i];
					v(a);
				}
			}
			return size;
		}
	};
	class Table
	{
	public:
		static void set(const rsv &r)
		{
			variable v = r;
			v["exist"] = variable(Exist);
			v["delete"] = variable(Delete);
			v["keys"] = variable(Keys);
			v["foreach"] = variable(Foreach);
		}
	private:
		static variable Exist(variable &this_v, variable &v)
		{
			if (!this_v)	return v[0].exist(v[1]);
			else			return this_v.exist(v);
		}
		static variable Delete(variable &this_v, variable &v)
		{
			if (!this_v)	v[0].del(v[1]);
			else			this_v.del(v);
			return variable();	// 取り除いた変数を返すというのも手？
		}
		static variable Keys(variable &this_v, variable &v)
		{
			if (!this_v)return v.keys();
			else		return this_v.keys();	// 当然ながらexistやkeysが含まれることになる、まあいいか
		}
		static variable Foreach(variable &this_v, variable &v)
		{
			if (!this_v)
			{
				variable l = v[0];
				variable f = v[1];
				variable k = l.keys();
				int s = k.length();
				for (int i = 0; i < s; ++i)
				{
					string str = k[i];
					if (!l[str].type(METHOD) && !l[str].type(CMETHOD))
						f(str, l[str]);
				}
				return k;
			}
			else
			{
				variable k = this_v.keys();
				int s = k.length();
				for (int i = 0; i < s; ++i)
				{
					string str = k[i];
					if (!this_v[str].type(METHOD) && !this_v[str].type(CMETHOD))
						v(str, this_v[str]);
				}
				return k;
			}
		}
	};
	class File
	{
	public:
		static void set(const rsv &r)
		{
			variable v = r;
			variable fc = Close;
			v["open"] = variable(Open);
			v["close"] = fc;
			v["destructor"] = fc;
			v["read"] = variable(Read);
		}
	private:
		static variable Open(variable &this_v, variable &v)
		{
			string name = v.toString();
			if (!this_v)
			{
				using namespace std;
				FILE *fp = fopen(name, "r");
				if (!fp)
					return 0;
				variable f;
				f["$$__FILE*fp__$$"] = fp;
				f["name"] = name;
				set(f);
				variable r = f.instance();
				return r.pointer();
			}
			else
			{
				using namespace std;
				FILE *fp = (FILE*)(void*)this_v["$$__FILE*fp__$$"];
				if (fp)
					fclose(fp);
				this_v["$$__FILE*fp__$$"] = fp = fopen(name, "r");;
				if (fp)	this_v["name"] = name;
				else	this_v["name"] = "";
				return 0;
			}
		}
		static variable Close(variable &this_v, variable &v)
		{
			using namespace std;
			FILE *fp = (FILE*)(void*)this_v["$$__FILE*fp__$$"];
			if (fp)
				fclose(fp);
			this_v["$$__FILE*fp__$$"] = NULL;
			this_v["name"] = "";
			return v;
		}
		static variable Read(variable &this_v, variable &v)
		{
			std::FILE *fp = (std::FILE*)(void*)this_v["$$__FILE*fp__$$"];
			int size = v;
			if (!fp || !size)
				return "";
			vector<Variable::bytecode::byte> vbuf(size+1);	// templateによる容量の肥大化を防ぐ為の型の再利用
			char *buf = (char*)&vbuf[0];
			size = std::fread(buf, 1, size, fp);
			buf[size] = 0;
			string s = buf;
			return s;
		}
	};
	class Binary
	{
	public:
		static void set(const rsv &r)
		{
			variable v = r;
			variable fc = Close;
			v["open"] = variable(Open);
			v["close"] = fc;
			v["destructor"] = fc;
			v["read"] = variable(Read);
			v["readdw"] = variable(ReadDW);
		}
	private:
		static variable Open(variable &this_v, variable &v)
		{
			string name = v.toString();
			if (!this_v)
			{
				using namespace std;
				FILE *fp = fopen(name, "rb");
				if (!fp)
					return 0;
				variable f;
				f["$$__FILE*fp__$$"] = fp;
				f["name"] = name;
				set(f);
				variable r = f.instance();
				return r.pointer();
			}
			else
			{
				using namespace std;
				FILE *fp = (FILE*)(void*)this_v["$$__FILE*fp__$$"];
				if (fp)
					fclose(fp);
				this_v["$$__FILE*fp__$$"] = fp = fopen(name, "r");;
				if (fp)	this_v["name"] = name;
				else	this_v["name"] = "";
				return 0;
			}
		}
		static variable Close(variable &this_v, variable &v)
		{
			using namespace std;
			FILE *fp = (FILE*)(void*)this_v["$$__FILE*fp__$$"];
			if (fp)
				fclose(fp);
			this_v["$$__FILE*fp__$$"] = NULL;
			this_v["name"] = "";
			return v;
		}
		static variable Read(variable &this_v, variable &v)
		{
			using namespace std;
			FILE *fp = (FILE*)(void*)this_v["$$__FILE*fp__$$"];
			int size = v;
			variable r;
			if (!fp || !size)
				return r;
			for (int i = 0; i < size; ++i)
			{
				int c = fgetc(fp);
				if (c == EOF)
				{
					size = i;
					break;
				}
				hex h = c;
				r[i] = h;
			}
			r["length"] = size;
			return r;
		}
		static variable ReadDW(variable &this_v, variable &v)
		{
			using namespace std;
			FILE *fp = (FILE*)(void*)this_v["$$__FILE*fp__$$"];
			int size = v;
			variable r;
			if (!fp || !size)
				return r;
			for (int i = 0; i < size; ++i)
			{
				hex h;
				if (fread(&h, 1, sizeof(hex), fp) < sizeof(hex))
				{
					size = i;
					break;
				}
				r[i] = h;
			}
			r["length"] = size;
			return r;
		}
	};
	class Time
	{
	public:
		static void set(const rsv &r)
		{
			variable v = r;
			v["time"] = time;
			v["clock"] = Clock;
			v["CLOCKS_PER_SEC"] = CLOCKS_PER_SEC;
		}
	private:
		static variable time(variable &v)	{return static_cast<int>(std::time(NULL));}
		static variable Clock(variable &v)	{return std::clock();}
	};
	class Math
	{
	public:
		static void set(const rsv &r)
		{
			variable v = r;
			v["abs"] = Abs;
			v["fabs"] = FAbs;
			v["sqrt"] = Sqrt;
			v["pow"] = Pow;
			v["log"] = Log;
		}
	private:
		static variable Abs(variable &v)	{return std::abs(static_cast<int>(v));}
		static variable FAbs(variable &v)	{return std::fabs(static_cast<double>(v));}
		static variable Sqrt(variable &v)	{return std::sqrt(static_cast<double>(v));}
		static variable Pow(variable &v)	{return std::pow(static_cast<double>(v[0]), static_cast<double>(v[1]));}
		static variable Log(variable &v)	{return std::log(static_cast<double>(v));}
	};
};
