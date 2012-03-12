template<size_t S> class OverLoad{};
#ifdef PSL_MEMORY_MANAGER_SLIM
template<size_t S, int poolsize = 256> class MemoryPool
{
public:
	MemoryPool()	{next = NULL;current = count = 0;std::memset(ptr, 0, poolsize*S);}
	~MemoryPool()	{delete next;}
	void *nextptr()
	{
		if (count == poolsize)
		{
			if (!next)
				next = new MemoryPool;
			return next->nextptr();
		}
		for (int i = current; i < poolsize; ++i)
		{
			if (!ptr[i].x)
			{
				current = i+1;
				++count;
				return ptr + i;
			}
		}
		for (int i = 0; i < current; ++i)
		{
			if (!ptr[i].x)
			{
				current = i+1;
				++count;
				return ptr + i;
			}
		}
		#ifdef _DEBUG
		PSL_PRINTF(("throw,%d,%d\n", count, current));
		throw std::bad_alloc();
		#else
		return NULL;
		#endif
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
		current = p - ptr;
		count--;
		ptr[current].x = 0;
	}
private:
	int current;
protected:
	int count;
	const static int psize = poolsize;
	MemoryPool *next;
	union DATA
	{
		char s[S];
		int x;
	} ptr[poolsize];
};

class VMemoryPool : public MemoryPool<8>
{
public:
	void GarbageCollection()
	{
		Mark(this);

		for (int i = 0; i < psize; ++i)
		{
			if (ptr[i].x)
			{
				Variable *v = (Variable*)(ptr+i);
				v->destructor_unmark();	// デストラクタだけ先に実行する
			}
		}
		for (int i = 0; i < psize; ++i)
		{
			if (ptr[i].x)
			{
				Variable *v = (Variable*)(ptr+i);
				v->delete_unmark();
			}
		}

		UnMark();
	}
private:
	void searchcount(Variable *v, int &c)
	{
		if (count)
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
		}
		if (next)
			((VMemoryPool*)next)->searchcount(v, c);
	}
	void Mark(VMemoryPool *m)
	{
		if (count)
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
		}
		if (next)
			((VMemoryPool*)next)->Mark(m);
	}
	void Mark2()
	{
		if (count)
		{
			for (int i = 0; i < psize; ++i)
			{
				if (ptr[i].x)
				{
					Variable *v = (Variable*)(ptr+i);
					v->unmark(0x7FFFFFFF);
				}
			}
		}
		if (next)
			((VMemoryPool*)next)->Mark2();
	}
	void UnMark()
	{
		if (count)
		{
			for (int i = 0; i < psize; ++i)
			{
				if (ptr[i].x)
				{
					Variable *v = (Variable*)(ptr+i);
					v->unmark(0x3FFFFFFF);
				}
			}
		}
		if (next)
			((VMemoryPool*)next)->UnMark();
	}
};
#else
template<size_t S, int poolsize = 256> class MemoryPool
{
public:
	MemoryPool()	{next = NULL;current = 0;
		for (int i = 0; i < poolsize; i++)
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
//		if (current != -1)	ptr[current].prev = -1;
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
//		if (current != -1)		ptr[current].prev = c;
		ptr[c].next = current;
//		ptr[c].prev = -1;
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
			delete envtemp_p();
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
	static Environment *envtemp_p()
	{
		static Environment *envtemp = new Environment;
		return envtemp;
	}
public:
	static rsv &rsvnull()			{return *rsvnull_p();}
	static Environment &envtemp()	{return *envtemp_p();}
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
