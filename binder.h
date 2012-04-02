private:
	#define ARG(n) A##n a##n = v[n];
	#define TMV(a,b) static variable tmv(variable &this_v, variable &v){F t = this_v[0];a t b;return v;}
	template<typename F>
	TMV(,())
	template<typename F, typename A0>
	TMV(ARG(0),(a0))
	template<typename F, typename A0, typename A1>
	TMV(ARG(0)ARG(1),(a0,a1))
	template<typename F, typename A0, typename A1, typename A2>
	TMV(ARG(0)ARG(1)ARG(2),(a0,a1,a2))
	template<typename F, typename A0, typename A1, typename A2, typename A3>
	TMV(ARG(0)ARG(1)ARG(2)ARG(3),(a0,a1,a2,a3))
	template<typename F, typename A0, typename A1, typename A2, typename A3, typename A4>
	TMV(ARG(0)ARG(1)ARG(2)ARG(3)ARG(4),(a0,a1,a2,a3,a4))
	template<typename F, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
	TMV(ARG(0)ARG(1)ARG(2)ARG(3)ARG(4)ARG(5),(a0,a1,a2,a3,a4,a5))
	#undef TMV

	#define TMR(a,b) static variable tmr(variable &this_v, variable &v){F t = this_v[0];a R r = t b;return r;}
	template<typename F, typename R>
	TMR(,())
	template<typename F, typename R, typename A0>
	TMR(ARG(0),(a0))
	template<typename F, typename R, typename A0, typename A1>
	TMR(ARG(0)ARG(1),(a0,a1))
	template<typename F, typename R, typename A0, typename A1, typename A2>
	TMR(ARG(0)ARG(1)ARG(2),(a0,a1,a2))
	template<typename F, typename R, typename A0, typename A1, typename A2, typename A3>
	TMR(ARG(0)ARG(1)ARG(2)ARG(3),(a0,a1,a2,a3))
	template<typename F, typename R, typename A0, typename A1, typename A2, typename A3, typename A4>
	TMR(ARG(0)ARG(1)ARG(2)ARG(3)ARG(4),(a0,a1,a2,a3,a4))
	template<typename F, typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
	TMR(ARG(0)ARG(1)ARG(2)ARG(3)ARG(4)ARG(5),(a0,a1,a2,a3,a4,a5))
	#undef TMR
	#undef ARG

	#define F2T(n) variable f(n);variable v;v["m"] = f;variable i = v.instance();i.push(func);i.push(i.pointer());return i["m"];
	#ifdef __BORLANDC__
	static rsv f2tv(void (*func)(void)){F2T((tmv<void (*)(void)>));}
	#endif
	template<typename F>
	static rsv f2t(void (*func)(void)){F2T((tmv<F>));}
	template<typename F, typename A1>
	static rsv f2t(void (*func)(A1)){F2T((tmv<F,A1>));}
	template<typename F, typename A1, typename A2>
	static rsv f2t(void (*func)(A1)){F2T((tmv<F,A1,A2>));}
	template<typename F, typename A1, typename A2, typename A3>
	static rsv f2t(void (*func)(A1)){F2T((tmv<F,A1,A2,A3>));}
	template<typename F, typename A1, typename A2, typename A3, typename A4>
	static rsv f2t(void (*func)(A1)){F2T((tmv<F,A1,A2,A3,A4>));}
	template<typename F, typename A1, typename A2, typename A3, typename A4, typename A5>
	static rsv f2t(void (*func)(A1)){F2T((tmv<F,A1,A2,A3,A4,A5>));}
	template<typename F, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	static rsv f2t(void (*func)(A1)){F2T((tmv<F,A1,A2,A3,A4,A5,A6>));}
	template<typename F, typename R>
	static rsv f2t(R (*func)(void)){F2T((tmr<F,R>));}
	template<typename F, typename R, typename A1>
	static rsv f2t(R (*func)(A1)){F2T((tmr<F,R,A1>));}
	template<typename F, typename R, typename A1, typename A2>
	static rsv f2t(R (*func)(A1,A2)){F2T((tmr<F,R,A1,A2>));}
	template<typename F, typename R, typename A1, typename A2, typename A3>
	static rsv f2t(R (*func)(A1,A2)){F2T((tmr<F,R,A1,A2,A3>));}
	template<typename F, typename R, typename A1, typename A2, typename A3, typename A4>
	static rsv f2t(R (*func)(A1,A2)){F2T((tmr<F,R,A1,A2,A3,A4>));}
	template<typename F, typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
	static rsv f2t(R (*func)(A1,A2)){F2T((tmr<F,R,A1,A2,A3,A4,A5>));}
	template<typename F, typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	static rsv f2t(R (*func)(A1,A2)){F2T((tmr<F,R,A1,A2,A3,A4,A5,A6>));}
	#undef TEMPLATE
public:
	template<typename F>
	void addFunction(const string &s, F f)
	{
		variable g = global;
		g.set(s, f2t<F>(f));
	}
	#ifdef __BORLANDC__
	void addFunction(const string &s, void(*f)())
	{
		variable g = global;
		g.set(s, f2tv(f));
	}
	#endif
