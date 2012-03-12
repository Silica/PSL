class AnonymousScope : public Scope
{
public:
	AnonymousScope(Code *statement) : Scope(statement)	{}
	virtual Type getType()	{return ANONYMOUS;}
	virtual Scope *Return()
	{
		Scope *s = owner->Return();
		owner = NULL;
		delete this;
		return s;
	}
	virtual Scope *Break()
	{
		Scope *s = owner->Break();
		owner = NULL;
		delete this;
		return s;
	}
	virtual Scope *Continue()
	{
		Scope *s = owner->Continue();
		owner = NULL;
		delete this;
		return s;
	}
};

class LoopScope : public Scope
{
public:
	LoopScope(Code *statement, int c) : Scope(statement)	{cline = c;}
	virtual Type getType()	{return LOOP;}
	virtual Scope *Return()
	{
		Scope *s = owner->Return();
		owner = NULL;
		delete this;
		return s;
	}
	virtual Scope *Break()
	{
		Scope *s = owner;
		owner = NULL;
		delete this;
		return s;
	}
	virtual Scope *Continue()
	{
		line = cline;
		return this;
	}
private:
	int cline;
};

class FunctionScope : public Scope
{
public:
	FunctionScope(Code *statement, Variable *v) : Scope(statement),static_v(v)	{}
	virtual Type getType()	{return FUNCTION;}
	virtual Variable *getVariable(const string &name)
	{
		if (local.get()->exist(name))	return local.get()->child(name);
		if (static_v.get()->exist(name))return static_v.get()->child(name);
		else							return NULL;
	}
	virtual bool addStatic(const string &name, variable &v, Environment *env)
	{
		if (static_v.get()->exist(name))
		{
			variable x;
			if (env)	env->push(x);
			return false;
		}
		bool r = static_v.get()->set(name, v);
		if (env)	env->push(v);
		return r;
	}
	virtual Scope *Return()
	{
		Scope *s = owner;
		owner = NULL;
		delete this;
		return s;
	}
	virtual Scope *Break()
	{
		return this;
	}
	virtual Scope *Continue()
	{
		line = 0;
		return this;
	}
	virtual Scope *Goto(const string &label)
	{
		if (code->Goto(label, line))
			return this;
		return NULL;
	}
	virtual Scope *End(Environment &env)
	{
		Scope *s = owner;
		owner = NULL;
		variable v;
		env.push(v);
		delete this;
		return s;
	}
protected:
	rsv static_v;
};

class MethodScope : public FunctionScope
{
public:
	MethodScope(Code *statement, Variable *s, Variable *t) : FunctionScope(statement, s),this_v(t)	{}
	virtual Type getType()	{return METHOD;}
	Variable *getVariable(const string &name)
	{
		if (local.get()->exist(name))	return local.get()->child(name);
		if (this_v.get()->exist(name))	return this_v.get()->child(name);
		if (static_v.get()->exist(name))return static_v.get()->child(name);
		else							return NULL;
	}
	bool Declaration(const string &name, variable &v, Environment *env)
	{
		bool r = this_v.get()->set(name, v);
		if (env)	env->push(v);
		return r;
	}
protected:
	rsv this_v;
};
class ConstructorScope : public MethodScope
{
public:
	ConstructorScope(Code *statement, Variable *s, Variable *t) : MethodScope(statement, s, t)	{}
	virtual Type getType()	{return CONSTRUCTOR;}
	Scope *End(Environment &env)
	{
		Scope *s = owner;
		owner = NULL;
		env.push(this_v);
		delete this;
		return s;
	}
};

// GlobalScopeってのもあっていいのでは？
// 一番外側
// デフォでglobal変数として作成する

// switch
/*
	switchは
	breakで親を返す
	continueは親に投げる
	returnは親に投げる

	table[label]を作ってそこに飛ばす形式か
*/

// try
/*
	tryは
	大体は親に投げる
	throwでcatchに移って(continueみたいなジャンプか？)自分を返す
*/
