template<size_t S> class OverLoad{};
template<size_t S, int poolsize = 256> class MemoryPool
#ifdef PSL_MEMORY_MANAGER_SLIM
{
public:
	MemoryPool()	{next = NULL;current = 0;
		for (int i = 0; i < poolsize; ++i)
		{
			ptr[i].x = 0;
			ptr[i].next = i+1;
		}
		ptr[poolsize-1].next = -1;
	}
	~MemoryPool()	{delete next;}
	void *nextptr()
	{
		if (current == -1)
		{
			if (!next)
				next = new MemoryPool;
			return next->nextptr();
		}
		int c = current;
		current = ptr[c].next;
		return ptr + c;
	}
	void release(void *x)
	{
		DATA *p = static_cast<DATA*>(x);
		if (p < ptr || p >= ptr + poolsize)
		{
			#ifdef _DEBUG
			if (next)
			#endif
			next->release(p);
			return;
		}
		int c = p - ptr;
		ptr[c].x = 0;
		ptr[c].next = current;
		current = c;
	}
protected:
	int current;
	const static int psize = poolsize;
	MemoryPool *next;
	union DATA
	{
		char s[S];
		struct
		{
			int x;
			int next;
		};
	} ptr[poolsize];
};
class VMemoryPool : public MemoryPool<8>
{
public:
	void GarbageCollection()
	{
		Mark(this);

		for (int i = 0; i < psize; ++i)
			if (ptr[i].x)
				((Variable*)(ptr+i))->destructor_unmark();	// デストラクタだけ先に実行する
		for (int i = 0; i < psize; ++i)
			if (ptr[i].x)
				((Variable*)(ptr+i))->delete_unmark();

		UnMark();
	}
private:
	void searchcount(Variable *v, int &c)
	{
		for (int i = 0; i < psize; ++i)
		{
			if (ptr[i].x)
			{
				Variable *x = (Variable*)(ptr+i);
				if (v == x)
					continue;
				x->searchcount(v, c);
			}
		}
		if (next)
			((VMemoryPool*)next)->searchcount(v, c);
	}
	void Mark(VMemoryPool *m)
	{
		for (int i = 0; i < psize; ++i)
		{
			if (ptr[i].x)
			{
				Variable *v = (Variable*)(ptr+i);
				int count = 0;
				if (v->searchcount(v, count))
					continue;
				m->searchcount(v, count);
				v->markstart(count);
				m->Mark2();
			}
		}
		if (next)
			((VMemoryPool*)next)->Mark(m);
	}
	void Mark2()
	{
		for (int i = 0; i < psize; ++i)
			if (ptr[i].x)
				((Variable*)(ptr+i))->unmark(0x7FFFFFFF);
		if (next)
			((VMemoryPool*)next)->Mark2();
	}
	void UnMark()
	{
		for (int i = 0; i < psize; ++i)
			if (ptr[i].x)
				((Variable*)(ptr+i))->unmark(0x3FFFFFFF);
		if (next)
			((VMemoryPool*)next)->UnMark();
	}
};
#elif !defined(PSL_MEMORY_MANAGER_LARGE)
{
public:
	MemoryPool()	{next = NULL;current = 0;
		for (int i = 0; i < poolsize; ++i)
		{
			ptr[i].prev = i-1;
			ptr[i].next = i+1;
		}
		ptr[poolsize-1].next = -1;
		used = -1;
	}
	~MemoryPool()	{delete next;}
	void *nextptr()
	{
		if (current == -1)
		{
			if (!next)
				next = new MemoryPool;
			return next->nextptr();
		}
		int c = current;
		current = ptr[c].next;
		ptr[c].next = used;
		if (used != -1)		ptr[used].prev = c;
		used = c;
		ptr[c].prev = -1;
		return ptr + c;
	}
	void release(void *x)
	{
		DATA *p = static_cast<DATA*>(x);
		if (p < ptr || p >= ptr + poolsize)
		{
			#ifdef _DEBUG
			if (next)
			#endif
			next->release(p);
			return;
		}
		int c = p - ptr;
		if (ptr[c].prev == -1)	used = ptr[c].next;
		else					ptr[ptr[c].prev].next = ptr[c].next;
		if (ptr[c].next != -1)	ptr[ptr[c].next].prev = ptr[c].prev;
		ptr[c].next = current;
		current = c;
	}
private:
	int current;
protected:
	int used;
	const static int psize = poolsize;
	MemoryPool *next;
	struct DATA
	{
		char s[S];
		short prev;
		short next;
	} ptr[poolsize];
};
class VMemoryPool : public MemoryPool<8>
{
public:
	void GarbageCollection()
	{
		Mark(this);

		for (int i = used; i != -1; i = ptr[i].next)
			((Variable*)(ptr+i))->destructor_unmark();	// デストラクタだけ先に実行する
		for (int i = used; i != -1; i = ptr[i].next)
			((Variable*)(ptr+i))->delete_unmark();

		UnMark();
	}
private:
	void searchcount(Variable *v, int &c)
	{
		for (int i = used; i != -1; i = ptr[i].next)
		{
			Variable *x = (Variable*)(ptr+i);
			if (v == x)
				continue;
			x->searchcount(v, c);
		}
		if (next)
			((VMemoryPool*)next)->searchcount(v, c);
	}
	void Mark(VMemoryPool *m)
	{
		for (int i = used; i != -1; i = ptr[i].next)
		{
			Variable *v = (Variable*)(ptr+i);
			int count = 0;
			if (v->searchcount(v, count))
				continue;
			m->searchcount(v, count);
			v->markstart(count);
			m->Mark2();
		}
		if (next)
			((VMemoryPool*)next)->Mark(m);
	}
	void Mark2()
	{
		for (int i = used; i != -1; i = ptr[i].next)
			((Variable*)(ptr+i))->unmark(0x7FFFFFFF);
		if (next)
			((VMemoryPool*)next)->Mark2();
	}
	void UnMark()
	{
		for (int i = used; i != -1; i = ptr[i].next)
			((Variable*)(ptr+i))->unmark(0x3FFFFFFF);
		if (next)
			((VMemoryPool*)next)->UnMark();
	}
};
#else
{
public:
	MemoryPool()	{used = NULL;current = p.ptr;}
	void *nextptr()
	{
		if (current == NULL)
			current = p.add();
		DATA *c = current;
		current = c->next;
		c->next = used;
		if (used != NULL)	used->prev = c;
		used = c;
		c->prev = NULL;
		return c;
	}
	void release(void *x)
	{
		DATA *p = static_cast<DATA*>(x);
		if (p->prev == NULL)	used = p->next;
		else					p->prev->next = p->next;
		if (p->next != NULL)	p->next->prev = p->prev;
		p->next = current;
		current = p;
	}
protected:
	struct DATA
	{
		char s[S];
		DATA *prev;
		DATA *next;
	};
	DATA *current;
	const static int psize = poolsize;
	DATA *used;
	struct pool
	{
		DATA ptr[poolsize];
		pool *next;
		pool()	{next = NULL;
			for (int i = 0; i < poolsize; ++i)
				ptr[i].next = ptr+i+1;
			ptr[poolsize-1].next = NULL;
		}
		~pool()	{delete next;}
		DATA *add()
		{
			pool *n = new pool;
			if (next)
				n->next = next;
			next = n;
			return next->ptr;
		}
	} p;
};
class VMemoryPool : public MemoryPool<8>
{
public:
	void GarbageCollection()
	{
		for (DATA *d = used; d != NULL; d = d->next)
		{
			Variable *v = (Variable*)d;
			int count = 0;
			if (v->searchcount(v, count))
				continue;
			for (DATA *d = used; d != NULL; d = d->next)
			{
				Variable *x = (Variable*)d;
				if (v == x)
					continue;
				x->searchcount(v, count);
			}
			v->markstart(count);
			for (DATA *d = used; d != NULL; d = d->next)
				((Variable*)d)->unmark(0x7FFFFFFF);
		}
		for (DATA *d = used; d != NULL; d = d->next)
			((Variable*)d)->destructor_unmark();	// デストラクタだけ先に実行する
		for (DATA *d = used; d != NULL; d = d->next)
			((Variable*)d)->delete_unmark();
		for (DATA *d = used; d != NULL; d = d->next)
			((Variable*)d)->unmark(0x3FFFFFFF);
	}
};
#endif

class StaticObject
{
	struct sobj
	{
		MemoryPool<8> pool8;
		MemoryPool<44> pool44;
		VMemoryPool vpool;
		~sobj()
		{
			vpool.GarbageCollection();
			#ifdef PSL_SHARED_GLOBAL
			delete global_p();
			#else
			delete envtemp_p();
			#endif
			delete rsvnull_p();
		}
	};
	friend class sobj;
	static sobj &so()
	{
		static sobj o;
		return o;
	}
	static rsv *rsvnull_p()
	{
		static rsv *null = new rsv;
		return null;
	}
	#ifdef PSL_SHARED_GLOBAL
	static rsv *global_p()
	{
		static rsv *global = new rsv;
		return global;
	}
public:
	static rsv &global()			{return *global_p();}
	#else
	static Environment *envtemp_p()
	{
		static Environment *envtemp = new Environment;
		return envtemp;
	}
public:
	static Environment &envtemp()	{return *envtemp_p();}
	#endif
public:
	static rsv &rsvnull()			{return *rsvnull_p();}
	static MemoryPool<8> &pool(OverLoad<8> x)	{return so().pool8;};
	static MemoryPool<44> &pool(OverLoad<44> x)	{return so().pool44;};
	static VMemoryPool &vpool()		{return so().vpool;}
};


class VMemoryManager
{
public:
	static void *Next()				{return StaticObject::vpool().nextptr();}
	static void Release(void *ptr)	{StaticObject::vpool().release(ptr);}
};

template<size_t S> class MemoryManager
{
public:
	static void *Next()				{return (StaticObject::pool(OverLoad<S>())).nextptr();}
	static void Release(void *ptr)	{(StaticObject::pool(OverLoad<S>())).release(ptr);}
};
