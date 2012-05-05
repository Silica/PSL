#define FV(n,t,g) class vBCFunctionV##n:public vBase{void(*x)t;public:vBCFunctionV##n(void(*f)t){x=f;}\
	Type type()const{return BCFUNCTION;}vBase *clone(){return new vBCFunctionV##n(x);}\
	bool toBool()const{return true;}size_t length()const{return 1;}\
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
	Type type()const{return BCFUNCTION;}vBase *clone(){return new vBCFunctionR##n(x);}\
	bool toBool()const{return true;}size_t length()const{return 1;}\
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

#define MV(n,t,g) class vCCMethodV##n : public vBase{void(C::*x)t;Variable *this_v;public:\
	vCCMethodV##n(void(C::*m)t){x=m;this_v=NULL;}vBase *clone(){return new vCCMethodV##n(x);}\
	Type type()const{return CCMETHOD;}void push(Variable *v){this_v=v;}\
	bool toBool()const{return true;}size_t length()const{return 1;}\
	void prepare(Environment &env, Variable *v){variable a = env.pop();C*i=static_cast<C*>(this_v->index(0)->toPointer());(i->*x)g;env.push(a);}\
	rsv call(Environment &env, variable &a, Variable *v){C*i=static_cast<C*>(this_v->index(0)->toPointer());(i->*x)g;return a;}};
template<class C>MV(0,(),())
template<class C, class A0>MV(1,(A0),(a[0]))
template<class C, class A0, class A1>MV(2,(A0,A1),(a[0],a[1]))
template<class C, class A0, class A1, class A2>MV(3,(A0,A1,A2),(a[0],a[1],a[2]))
template<class C, class A0, class A1, class A2, class A3>MV(4,(A0,A1,A2,A3),(a[0],a[1],a[2],a[3]))
template<class C, class A0, class A1, class A2, class A3, class A4>MV(5,(A0,A1,A2,A3,A4),(a[0],a[1],a[2],a[3],a[4]))
template<class C, class A0, class A1, class A2, class A3, class A4, class A5>MV(6,(A0,A1,A2,A3,A4,A5),(a[0],a[1],a[2],a[3],a[4],a[5]))
#undef MV
#define MR(n,t,g) class vCCMethodR##n : public vBase{R(C::*x)t;Variable *this_v;public:\
	vCCMethodR##n(R(C::*m)t){x=m;this_v=NULL;}vBase *clone(){return new vCCMethodR##n(x);}\
	Type type()const{return CCMETHOD;}void push(Variable *v){this_v=v;}\
	bool toBool()const{return true;}size_t length()const{return 1;}\
	void prepare(Environment &env, Variable *v){variable a = env.pop();C*i=static_cast<C*>(this_v->index(0)->toPointer());variable r = (i->*x)g;env.push(r);}\
	rsv call(Environment &env, variable &a, Variable *v){C*i=static_cast<C*>(this_v->index(0)->toPointer());variable r = (i->*x)g;return r;}};
template<class C, class R>MR(0,(),())
template<class C, class R, class A0>MR(1,(A0),(a[0]))
template<class C, class R, class A0, class A1>MR(2,(A0,A1),(a[0],a[1]))
template<class C, class R, class A0, class A1, class A2>MR(3,(A0,A1,A2),(a[0],a[1],a[2]))
template<class C, class R, class A0, class A1, class A2, class A3>MR(4,(A0,A1,A2,A3),(a[0],a[1],a[2],a[3]))
template<class C, class R, class A0, class A1, class A2, class A3, class A4>MR(5,(A0,A1,A2,A3,A4),(a[0],a[1],a[2],a[3],a[4]))
template<class C, class R, class A0, class A1, class A2, class A3, class A4, class A5>MR(6,(A0,A1,A2,A3,A4,A5),(a[0],a[1],a[2],a[3],a[4],a[5]))
#undef MR
template<class C>
static vBase *CCMethod(void(C::*m)()){return new vCCMethodV0<C>(m);}
template<class C, class A0>
static vBase *CCMethod(void(C::*m)(A0)){return new vCCMethodV1<C,A0>(m);}
template<class C, class A0, class A1>
static vBase *CCMethod(void(C::*m)(A0,A1)){return new vCCMethodV2<C,A0,A1>(m);}
template<class C, class A0, class A1, class A2>
static vBase *CCMethod(void(C::*m)(A0,A1,A2)){return new vCCMethodV3<C,A0,A1,A2>(m);}
template<class C, class A0, class A1, class A2, class A3>
static vBase *CCMethod(void(C::*m)(A0,A1,A2,A3)){return new vCCMethodV4<C,A0,A1,A2,A3>(m);}
template<class C, class A0, class A1, class A2, class A3, class A4>
static vBase *CCMethod(void(C::*m)(A0,A1,A2,A3,A4)){return new vCCMethodV5<C,A0,A1,A2,A3,A4>(m);}
template<class C, class A0, class A1, class A2, class A3, class A4, class A5>
static vBase *CCMethod(void(C::*m)(A0,A1,A2,A3,A4,A5)){return new vCCMethodV6<C,A0,A1,A2,A3,A4,A5>(m);}
template<class C, class R>
static vBase *CCMethod(R(C::*m)()){return new vCCMethodR0<C,R>(m);}
template<class C, class R, class A0>
static vBase *CCMethod(R(C::*m)(A0)){return new vCCMethodR1<C,R,A0>(m);}
template<class C, class R, class A0, class A1>
static vBase *CCMethod(R(C::*m)(A0,A1)){return new vCCMethodR2<C,R,A0,A1>(m);}
template<class C, class R, class A0, class A1, class A2>
static vBase *CCMethod(R(C::*m)(A0,A1,A2)){return new vCCMethodR3<C,R,A0,A1,A2>(m);}
template<class C, class R, class A0, class A1, class A2, class A3>
static vBase *CCMethod(R(C::*m)(A0,A1,A2,A3)){return new vCCMethodR4<C,R,A0,A1,A2,A3>(m);}
template<class C, class R, class A0, class A1, class A2, class A3, class A4>
static vBase *CCMethod(R(C::*m)(A0,A1,A2,A3,A4)){return new vCCMethodR5<C,R,A0,A1,A2,A3,A4>(m);}
template<class C, class R, class A0, class A1, class A2, class A3, class A4, class A5>
static vBase *CCMethod(R(C::*m)(A0,A1,A2,A3,A4,A5)){return new vCCMethodR6<C,R,A0,A1,A2,A3,A4,A5>(m);}
