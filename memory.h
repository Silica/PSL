template<size_t S> class OverLoad{};
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
