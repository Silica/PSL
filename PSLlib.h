class PSLlib
{
public:
	static void Basic(const rsv &r)
	{
		variable v = r;
		v["int"] = 0;
		v["float"] = 0.0;
		v["string"] = "";
		v["true"] = 1;
		v["false"] = 0;
		variable nil = NIL;
		variable hex = HEX;
		variable ref = REFERENCE;
		variable pointer = POINTER;
		variable thread = THREAD;
		v["nil"] = nil;
		v["null"] = nil;
		v["hex"] = hex;
		v["ref"] = ref;
		v["pointer"] = pointer;
		v["thread"] = thread;
		v["object"];

		v["print"] = Print;
		#ifdef PSL_DEBUG
		v["debug"] = Debug;
		#endif
		v["foreach"] = Foreach;
		v["eval"] = Eval;
		#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
		v["GarbageCollection"] = GarbageCollection;
		#endif
		Strlib::set(v["strlib"]);
		Array::set(v["array"]);
		Table::set(v["table"]);
		File::set(v["file"]);
		Binary::set(v["binary"]);
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
//		case RARRAY:	PSL_PRINTF(("[list:%d]", v.length()));break;
		case THREAD:	PSL_PRINTF(("[thread:%s]", (bool)v ? "alive" : "dead"));break;
		default:
			{
				PSL_PRINTF(("%s", v.toString().c_str()));
				break;
			}
		}
		return v;
	}
	#ifdef PSL_DEBUG
	static variable Debug(variable &v)
	{
		v.dump();
		return v;
	}
	#endif
	static variable Foreach(variable &v)
	{
		variable l = v[0];
		variable f = v[1];
		variable k = l.keys();
		int s = k.length();
		for (int i = 0; i < s; i++)
			f(k[i], l[k[i]]);
		return k;
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
		Variable::StaticObject::vpool().GarbageCollection();
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
			variable length = Length;
			variable push = Push;
			variable foreach = Foreach;
			v["length"] = length;
			v["push"] = push;
			v["foreach"] = foreach;
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
				variable array = v[0];
				array.push(v[1]);
				return array.length();
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
				for (int i = 0; i < size; i++)
				{
					variable a = array[i];
					v[1](a);
				}
			}
			else
			{
				size = this_v.length();
				for (int i = 0; i < size; i++)
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
			variable exist = Exist;
			variable del = Delete;
			variable keys = Keys;
			v["exist"] = exist;
			v["delete"] = del;
			v["keys"] = keys;
		}
	private:
		static variable Exist(variable &this_v, variable &v)
		{
			if (!this_v)
			{
				variable table = v[0];
				variable key = v[1];
				return table.exist(key);
			}
			else
			{
				return this_v.exist(v);
			}
		}
		static variable Delete(variable &this_v, variable &v)
		{
			if (!this_v)
			{
				variable table = v[0];
				variable key = v[1];
				table.del(key);
			}
			else
			{
				this_v.del(v);
			}
			return variable();	// Žæ‚èœ‚¢‚½•Ï”‚ð•Ô‚·‚Æ‚¢‚¤‚Ì‚àŽèH
		}
		static variable Keys(variable &this_v, variable &v)
		{
			if (!this_v)	return v.keys();
			else		return this_v.keys();	// “–‘R‚È‚ª‚çexist‚âkeys‚ªŠÜ‚Ü‚ê‚é‚±‚Æ‚É‚È‚éA‚Ü‚ ‚¢‚¢‚©
		}
	};
	class File
	{
	public:
		static void set(const rsv &r)
		{
			variable v = r;
			variable fo = Open;
			variable fc = Close;
			variable fr = Read;
			v["open"] = fo;
			v["close"] = fc;
			v["destructor"] = fc;
			v["read"] = fr;
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
			char *buf = new char[size+1];
			size = std::fread(buf, 1, size, fp);
			buf[size] = 0;
			string s = buf;
			delete[] buf;
			return s;
		}
	};
	class Binary
	{
	public:
		static void set(const rsv &r)
		{
			variable v = r;
			variable fo = Open;
			variable fc = Close;
			variable fr = Read;
			variable frd = ReadDW;
			v["open"] = fo;
			v["close"] = fc;
			v["destructor"] = fc;
			v["read"] = fr;
			v["readdw"] = frd;
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
			for (int i = 0; i < size; i++)
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
			for (int i = 0; i < size; i++)
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
};
