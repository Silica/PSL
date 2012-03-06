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
//	friend variable::variable(const rsv &v);
};

#ifdef PSL_USE_STL_VECTOR
typedef std::vector vector;
#else
template<typename T> class vector	// std::vectorのresizeが2つ目以降、コピーコンストラクタを呼ぶ為都合が悪い
{
public:
	vector()	{reserve = len = 0;x = NULL;}
	vector(int i)	{len = 0;reserve = i;x = new T[i];}
	~vector()	{delete[] x;}
	void resize(size_t t, bool b = true)
	{
		if (t > reserve)
		{
			T *n = new T[t];
			for (size_t i = 0; i < len; i++)
				n[i] = x[i];
			reserve = t;
			delete[] x;
			x = n;
		}
		if (b)
			len = t;
	}
	void push_back(const T &v)
	{
		if (len >= reserve)
			resize(reserve*2+1, false);
		x[len++] = v;
	}
	T &operator[](size_t t)
	{
//			if (t >= len)
//				len = t+1;
		// どうせやるなら
//				resize(t+1);
		return x[t];
	}
	size_t size() const	{return len;}
	bool empty() const	{return !len;}
private:
	T *x;
	size_t reserve;
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
			while (++n < (int)(ta->_reserve))
				if (ta->d[n])
					return;
			n = -1;
		}
	private:
		table *ta;
		int n;
	};
	friend class iterator;
	table()		{_reserve = _size = 0;}
	~table()
	{
		int s = d.size();
		for (int i = 0; i < s; i++)
			delete d[i];
	}
	size_t count(const string &s) const
	{
		if (!_reserve)	return 0;
		if (search(s) < 0)
			return 0;
		return 1;
	}
	rsv &operator[](const string &s) const
	{
		if (_size)
		{
			int i = search(s);
			if (i >= 0)
				return d[i]->second;
		}
		if (_size == _reserve)
			resize();
		hash h = gethash(s, _reserve);
		int i = getnextnull(h);
		d[i] = new data(s);
		_size++;
		return d[i]->second;
	}
	void erase(const string &s)
	{
		if (!_size)
			return;
		int i = search(s);
		if (i < 0)
			return;
		delete d[i];
		d[i] = NULL;
	}
	bool empty()	const{return !_size;}
	size_t size()	const{return _size;}
	iterator begin()	{
		if (!_size)
			return iterator(this, -1);
		else
		{
			for (size_t i = 0; i < _reserve; i++)
				if (d[i])
					return iterator(this, i);
		}
		return iterator(this, -1);
	}
	iterator end()		{return iterator(this, -1);}
private:
	int search(const string &s) const
	{
		hash h = gethash(s, _reserve);
		for (size_t i = h; i < _reserve; ++i)
			if (d[i] && d[i]->first == s)
				return i;
		for (size_t i = 0; i < h; ++i)
			if (d[i] && d[i]->first == s)
				return i;
		return -1;
	}
	size_t getnextnull(size_t t) const
	{
		for (size_t i = t; i < _reserve; ++i)
			if (!d[i])
				return i;
		for (size_t i = 0; i < t; ++i)
			if (!d[i])
				return i;
		std::printf("table error:\n");
		return 0;
	}
	void resize() const
	{
#ifdef PSL_USE_STL_VECTOR
		std::vector<data*> temp(_reserve, NULL);
#else
		vector<data*> temp(_reserve);
#endif
		int max = _reserve;
		_reserve = _reserve*2 + 1;
#ifdef PSL_USE_STL_VECTOR
		temp.swap(d);
		d.resize(_reserve, NULL);
#else
		d.resize(_reserve);
		for (int i = 0; i < max; i++)
		{
			temp[i] = d[i];
			d[i] = NULL;
		}
		for (size_t i = max; i < _reserve; i++)
			d[i] = NULL;
#endif

		for (int i = 0; i < max; i++)
		{
			if (temp[i])
			{
				hash h = gethash(temp[i]->first, _reserve);
				int n = getnextnull(h);
				d[n] = temp[i];
			}
		}
	}
	mutable size_t _size;
	mutable size_t _reserve;
	mutable vector<data*> d;
};
#endif

#ifdef PSL_USE_STL_VECTOR
typedef std::vector<rsv> rlist;
#else
typedef vector<rsv> rlist;
#endif
