class rsv	// 要はスマートポインタ
{
public:
	rsv(Variable *v)	{x = v->ref();}
	rsv(Variable *v,int i)	{x = v;}
	rsv(const variable &v)	{x = v.x->ref();}
#ifdef PSL_NULL_RSV
	rsv()				{x = NULL;}
	rsv(const rsv &v)	{x = v.x ? v.x->ref() : NULL;}
	~rsv()				{if (x) x->finalize();}
	rsv &operator=(const rsv &v)
	{
		if (x) x->finalize();
		x = v.x ? v.x->ref() : NULL;
		return *this;
	}
	void copy(const rsv &v)
	{
		if (x) x->finalize();
		x = v.x ? v.x->clone() : NULL;
	}
	void copy(Variable *v)
	{
		if (x) x->finalize();
		x = v->clone();
	}
	void set(Variable *v)
	{
		if (x) x->finalize();
		x = v;
	}
	Variable *get()	const{if(!x)x = new Variable();return x;}
#else	// 上で大丈夫である確信が持てない、新規変数を作る時にrsvで作って(共有して)いなければOKの筈だが
	// そして今や僅差になった
	// また上だとinline化した時に肥大化著しい
	// 更に速度の違いもあるのだが(多分下の方が速い？)
	// STL_MAPを使うなら↑、そうでないなら↓がいい、とおもう
	rsv()				{x = new Variable();}
	rsv(const rsv &v)	{x = v.x->ref();}
	~rsv()				{x->finalize();}
	rsv &operator=(const rsv &v)
	{
		x->finalize();
		x = v.x->ref();
		return *this;
	}
	void copy(const rsv &v)
	{
		x->finalize();
		x = v.x->clone();
	}
	void copy(Variable *v)
	{
		x->finalize();
		x = v->clone();
	}
	void set(Variable *v)
	{
		x->finalize();
		x = v;
	}
	Variable *get()	const{return x;}
#endif
private:
	mutable Variable *x;
};

#ifdef PSL_USE_STL_VECTOR
typedef std::vector vector;
#else
template<typename T> class vector	// std::vectorのresizeが2つ目以降、コピーコンストラクタを呼ぶ為都合が悪い
{
public:
	vector()	{res = len = 0;x = NULL;}
	vector(int i)	{len = 0;res = i;x = new T[i];}
	~vector()	{delete[] x;}
	void resize(size_t t)
	{
		reserve(t);
		len = t;
	}
	void reserve(size_t t)
	{
		if (t > res)
		{
			if (res*2 > t)
				t = res * 2;
			T *n = new T[t];
			for (size_t i = 0; i < len; i++)
				n[i] = x[i];
			res = t;
			delete[] x;
			x = n;
		}
	}
	void push_back(const T &v)
	{
		if (len >= res)
			reserve(res*2+1);
		x[len++] = v;
	}
	T &operator[](size_t t)
	{
//			if (t >= len)
//				resize(t+1);
		return x[t];
	}
	size_t size() const	{return len;}
	bool empty() const	{return !len;}
protected:
	T *x;
	size_t res;
	size_t len;
};
#endif

#ifdef PSL_USE_STL_MAP
typedef std::map<string,rsv> table;
#else
class table
{
	typedef unsigned long hash;
	static hash gethash(const string &s, hash max)	{return s.hash() & max;}
public:
	struct data
	{
		data(const string &s):first(s)	{}
		string first;
		rsv second;
	};
	class iterator
	{
	public:
		iterator(table *t, int i)	{ta = t;n = i;}
		bool operator!=(const iterator &it)	{return n != it.n;}
		data *operator->()			{return ta->d[n];}
		void operator++()
		{
			while (++n < (int)(ta->reserve))
				if (ta->d[n])
					return;
			n = -1;
		}
	private:
		table *ta;
		int n;
	};
	friend class iterator;
	table()		{reserve = len = 0;}
	~table()
	{
		int s = d.size();
		for (int i = 0; i < s; i++)
			delete d[i];
	}
	size_t count(const string &s) const
	{
		if (!reserve)	return 0;
		if (search(s) < 0)
			return 0;
		return 1;
	}
	rsv &operator[](const string &s) const
	{
		if (len)
		{
			int i = search(s);
			if (i >= 0)
				return d[i]->second;
		}
		if (len == reserve)
			resize();
		hash h = gethash(s, reserve);
		int i = getnextnull(h);
		d[i] = new data(s);
		len++;
		return d[i]->second;
	}
	void erase(const string &s)
	{
		if (!len)
			return;
		int i = search(s);
		if (i < 0)
			return;
		delete d[i];
		d[i] = NULL;
	}
	bool empty()	const{return !len;}
	size_t size()	const{return len;}
	iterator begin()	{
		if (!len)
			return iterator(this, -1);
		else
		{
			for (size_t i = 0; i < reserve; i++)
				if (d[i])
					return iterator(this, i);
		}
		return iterator(this, -1);
	}
	iterator end()		{return iterator(this, -1);}
private:
	int search(const string &s) const
	{
		hash h = gethash(s, reserve);
		for (size_t i = h; i < reserve; ++i)
			if (d[i] && d[i]->first == s)
				return i;
		for (size_t i = 0; i < h; ++i)
			if (d[i] && d[i]->first == s)
				return i;
		return -1;
	}
	size_t getnextnull(size_t t) const
	{
		for (size_t i = t; i < reserve; ++i)
			if (!d[i])
				return i;
		for (size_t i = 0; i < t; ++i)
			if (!d[i])
				return i;
		PSL_PRINTF(("table error:\n"));
		return 0;
	}
	void resize() const
	{
#ifdef PSL_USE_STL_VECTOR
		std::vector<data*> temp(reserve, NULL);
#else
		vector<data*> temp(reserve);
#endif
		int max = reserve;
		reserve = reserve*2 + 1;
#ifdef PSL_USE_STL_VECTOR
		temp.swap(d);
		d.resize(reserve, NULL);
#else
		d.resize(reserve);
		for (int i = 0; i < max; i++)
		{
			temp[i] = d[i];
			d[i] = NULL;
		}
		for (size_t i = max; i < reserve; i++)
			d[i] = NULL;
#endif

		for (int i = 0; i < max; i++)
		{
			if (temp[i])
			{
				hash h = gethash(temp[i]->first, reserve);
				int n = getnextnull(h);
				d[n] = temp[i];
			}
		}
	}
	mutable size_t len;
	mutable size_t reserve;
	mutable vector<data*> d;
};
#endif

#ifdef PSL_USE_STL_VECTOR
	#define PSL_USE_STL_STACK
typedef std::vector<rsv> rlist;
#else
typedef vector<rsv> rlist;
#endif

#ifdef PSL_USE_STL_STACK
typedef std::stack<rsv> rstack;
#else
class rstack : public rlist
{
public:
	void push(const rsv &v)	{push_back(v);}
	rsv &top()	{return x[len-1];}
	rsv pop()
	{
	#ifdef PSL_POPSTACK_NULL
		rsv v = x[--len];
		#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
		x[len] = Variable::StaticObject::rsvnull();
		#else
		const static rsv null;
		x[len] = null;
		#endif
		return v;
	#else
		return x[--len];
	#endif
	}
	#ifndef PSL_POPSTACK_NULL
	void clear()
	{
		for (int i = len; i < res; i++)
		{
			#ifdef PSL_USE_VARIABLE_MEMORY_MANAGER
			x[i] = Variable::StaticObject::rsvnull();
			#else
			const static rsv null;
			x[i] = null;
			#endif
		}
	}
	// 当然ながら、一々reserveまで回さなくても
	// 最大値を記録しておきそこまで回せばいいのだが
	// pushする度に最大値を記録するコストとどっちが高いか？

	// 実行するのはどこか
	// Environment::endScopeの中あたりかと思うが…保留
	#endif
};
#endif
