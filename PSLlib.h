class PSLlib
{
public:
	static void Basic(rsv &r)
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
		variable hex = variable::HEX;
		variable ref = variable::REFERENCE;
		variable pointer = variable::POINTER;
		variable thread = variable::THREAD;
		v["hex"] = hex;
		v["ref"] = ref;
		v["pointer"] = pointer;
		v["thread"] = thread;
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
};
