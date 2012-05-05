#define FV(n,t,g) class vBCFunctionV##n:public vBase{void(*x)t;public:vBCFunctionV##n(void(*f)t){x=f;}\
	void prepare(Environment &env, Variable *v){variable a = env.pop();x g;env.push(a);}\
	rsv call(Environment &env, variable &a, Variable *v){x g;return a;}};
FV(0,(),())
template<class A0>FV(1,(A0),(a))
template<class A0,class A1>FV(2,(A0,A1),(a[0],a[1]))
template<class A0,class A1,class A2>FV(3,(A0,A1,A2),(a[0],a[1],a[2]))
template<class A0,class A1,class A2,class A3>FV(4,(A0,A1,A2,A3),(a[0],a[1],a[2],a[3]))
template<class A0,class A1,class A2,class A3,class A4>FV(5,(A0,A1,A2,A3,A4),(a[0],a[1],a[2],a[3],a[4]))
template<class A0,class A1,class A2,class A3,class A4,class A5>FV(6,(A0,A1,A2,A3,A4,A5),(a[0],a[1],a[2],a[3],a[4],a[5]))
#undef FV
#define FR(n,t,g) class vBCFunctionR##n:public vBase{R(*x)t;public:vBCFunctionR##n(R(*f)t){x=f;}\
	void prepare(Environment &env, Variable *v){variable a = env.pop();variable r = x g;env.push(r);}\
	rsv call(Environment &env, variable &a, Variable *v){variable r = x g;return r;}};
template<class R>FR(0,(),())
template<class R, class A0>FR(1,(A0),(a))
template<class R, class A0,class A1>FR(2,(A0,A1),(a[0],a[1]))
template<class R, class A0,class A1,class A2>FR(3,(A0,A1,A2),(a[0],a[1],a[2]))
template<class R, class A0,class A1,class A2,class A3>FR(4,(A0,A1,A2,A3),(a[0],a[1],a[2],a[3]))
template<class R, class A0,class A1,class A2,class A3,class A4>FR(5,(A0,A1,A2,A3,A4),(a[0],a[1],a[2],a[3],a[4]))
template<class R, class A0,class A1,class A2,class A3,class A4,class A5>FR(6,(A0,A1,A2,A3,A4,A5),(a[0],a[1],a[2],a[3],a[4],a[5]))
#undef FR
static vBase *BCFunction(void(*f)()){return new vBCFunctionV0(f);}
template<class A0>
static vBase *BCFunction(void(*f)(A0)){return new vBCFunctionV1<A0>(f);}
template<class A0, class A1>
static vBase *BCFunction(void(*f)(A0,A1)){return new vBCFunctionV2<A0,A1>(f);}
template<class A0, class A1, class A2>
static vBase *BCFunction(void(*f)(A0,A1,A2)){return new vBCFunctionV3<A0,A1,A2>(f);}
template<class A0, class A1, class A2, class A3>
static vBase *BCFunction(void(*f)(A0,A1,A2,A3)){return new vBCFunctionV4<A0,A1,A2,A3>(f);}
template<class A0, class A1, class A2, class A3, class A4>
static vBase *BCFunction(void(*f)(A0,A1,A2,A3,A4)){return new vBCFunctionV5<A0,A1,A2,A3,A4>(f);}
template<class A0, class A1, class A2, class A3, class A4, class A5>
static vBase *BCFunction(void(*f)(A0,A1,A2,A3,A4,A5)){return new vBCFunctionV6<A0,A1,A2,A3,A4,A5>(f);}
template<class R>
static vBase *BCFunction(R(*f)()){return new vBCFunctionR0<R>(f);}
template<class R, class A0>
static vBase *BCFunction(R(*f)(A0)){return new vBCFunctionR1<R,A0>(f);}
template<class R, class A0, class A1>
static vBase *BCFunction(R(*f)(A0,A1)){return new vBCFunctionR2<R,A0,A1>(f);}
template<class R, class A0, class A1, class A2>
static vBase *BCFunction(R(*f)(A0,A1,A2)){return new vBCFunctionR3<R,A0,A1,A2>(f);}
template<class R, class A0, class A1, class A2, class A3>
static vBase *BCFunction(R(*f)(A0,A1,A2,A3)){return new vBCFunctionR4<R,A0,A1,A2,A3>(f);}
template<class R, class A0, class A1, class A2, class A3, class A4>
static vBase *BCFunction(R(*f)(A0,A1,A2,A3,A4)){return new vBCFunctionR5<R,A0,A1,A2,A3,A4>(f);}
template<class R, class A0, class A1, class A2, class A3, class A4, class A5>
static vBase *BCFunction(R(*f)(A0,A1,A2,A3,A4,A5)){return new vBCFunctionR6<R,A0,A1,A2,A3,A4,A5>(f);}
