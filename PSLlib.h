class PSLlib
{
public:
	static void Basic(const rsv &r)
	{
		variable v = r;
		v["print"] = Print;
		#ifdef PSL_DEBUG
		v["debug"] = Debug;
		#endif
		v["foreach"] = Foreach;
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
		File::set(v["file"]);
		Binary::set(v["binary"]);
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
		default:
			{
				std::printf("%s", v.toString().c_str());
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
		{
			variable a(RARRAY);
			a.push(k[i]);
			a.push(l[k[i]]);
			f(a);
		}
		return k;
	}
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
		static variable Open(variable &_this, variable &v)
		{
			string name = v.toString();
			if (!_this)
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
				FILE *fp = (FILE*)(void*)_this["$$__FILE*fp__$$"];
				if (fp)
					fclose(fp);
				_this["$$__FILE*fp__$$"] = fp = fopen(name, "r");;
				if (fp)	_this["name"] = name;
				else	_this["name"] = "";
				return 0;
			}
		}
		static variable Close(variable &_this, variable &v)
		{
			using namespace std;
			FILE *fp = (FILE*)(void*)_this["$$__FILE*fp__$$"];
			if (fp)
				fclose(fp);
			_this["$$__FILE*fp__$$"] = NULL;
			_this["name"] = "";
			return v;
		}
		static variable Read(variable &_this, variable &v)
		{
			using namespace std;
			FILE *fp = (FILE*)(void*)_this["$$__FILE*fp__$$"];
			if (!fp)
				return "";
			int size = v;
			if (!size)
				return "";
			char *buf = new char[size+1];
			size = fread(buf, 1, size, fp);
			buf[size] = 0;
			::string s = buf;
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
		static variable Open(variable &_this, variable &v)
		{
			string name = v.toString();
			if (!_this)
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
				FILE *fp = (FILE*)(void*)_this["$$__FILE*fp__$$"];
				if (fp)
					fclose(fp);
				_this["$$__FILE*fp__$$"] = fp = fopen(name, "r");;
				if (fp)	_this["name"] = name;
				else	_this["name"] = "";
				return 0;
			}
		}
		static variable Close(variable &_this, variable &v)
		{
			using namespace std;
			FILE *fp = (FILE*)(void*)_this["$$__FILE*fp__$$"];
			if (fp)
				fclose(fp);
			_this["$$__FILE*fp__$$"] = NULL;
			_this["name"] = "";
			return v;
		}
		static variable Read(variable &_this, variable &v)
		{
			using namespace std;
			FILE *fp = (FILE*)(void*)_this["$$__FILE*fp__$$"];
			variable r;
			if (!fp)
				return r;
			int size = v;
			if (!size)
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
		static variable ReadDW(variable &_this, variable &v)
		{
			using namespace std;
			FILE *fp = (FILE*)(void*)_this["$$__FILE*fp__$$"];
			variable r;
			if (!fp)
				return r;
			int size = v;
			if (!size)
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
