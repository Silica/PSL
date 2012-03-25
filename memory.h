#define OBJECT_SIZE 32

template<size_t S> class OverLoad{};
template<size_t S, int poolsize = 256> class MemoryPool
{
public:
	MemoryPool()	{current = p.ptr;}
	void *nextptr()
	{
		if (current == NULL)
			current = p.add();
		DATA *c = current;
		current = c->next;
		return c;
	}
	void release(void *x)
	{
		DATA *c = static_cast<DATA*>(x);
		c->x = 0;
		c->next = current;
		current = c;
	}
protected:
	const static int psize = poolsize;
	union DATA
	{
		char s[S];
		struct
		{
			int x;
			DATA *next;
		};
	};
	DATA *current;
	struct pool
	{
		DATA ptr[poolsize];
		pool *next;
		pool()	{next = NULL;
			for (int i = 0; i < poolsize; ++i)
			{
				ptr[i].x = 0;
				ptr[i].next = ptr+i+1;
			}
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
		bool empty()
		{
			for (int i = 0; i < poolsize; ++i)
				if (ptr[i].x)
					return false;
			return true;
		}
	} p;
};
class VMemoryPool : public MemoryPool<sizeof(Variable)>
{
public:
	void GarbageCollection()
	{
		#ifdef PSL_SHARED_GLOBAL
		StaticObject::global().get()->mark();
		#endif
		Mark();
		Destructor();
		Delete();
		UnMark();
		ReleaseEmpty();
	}
private:
	#define POOLOOP for (pool *pl = &p; pl != NULL; pl = pl->next)for (int i = 0; i < psize; ++i)if (pl->ptr[i].x)
	void searchcount(Variable *v, int &c)
	{
		POOLOOP
		{
			Variable *x = (Variable*)(pl->ptr+i);
			if (v == x)
				continue;
			x->searchcount(v, c);
		}
	}
	void Mark()
	{
		POOLOOP
		{
			Variable *v = (Variable*)(pl->ptr+i);
			int count = 0;
			if (v->searchcount(v, count))
				continue;
			searchcount(v, count);
			v->markstart(count);
			Mark2();
		}
	}
	void Mark2()		{POOLOOP{((Variable*)(pl->ptr+i))->unmark(0x7FFFFFFF);}}
	void UnMark()		{POOLOOP{((Variable*)(pl->ptr+i))->unmark(0x3FFFFFFF);}}
	void Destructor()	{POOLOOP{((Variable*)(pl->ptr+i))->destructor_unmark();}}
	void Delete()		{POOLOOP{((Variable*)(pl->ptr+i))->delete_unmark();}}
	void ReleaseEmpty()
	{
		for (pool **pl = &p.next; *pl != NULL;)
		{
			if ((*pl)->empty())
			{
				pool *n = (*pl)->next;
				(*pl)->next = NULL;
				delete *pl;
				*pl = n;
				continue;
			}
			pl = &(*pl)->next;
		}
		DATA **ptr = &current;
		POOLOOP;else{
			*ptr = pl->ptr+i;
			ptr = &pl->ptr[i].next;
		}
		*ptr = NULL;
	}
	#undef POOLOOP
};

class StaticObject
{
	struct sobj
	{
		const string destructor;
		#define pool(s,n) MemoryPool<s> pool##n;
		pool(8,8)
		pool(sizeof(Variable::vObject),VO)
		#undef pool
		VMemoryPool vpool;
		sobj():destructor("destructor"){}
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
	#define pool(s,n) static MemoryPool<s> &pool(OverLoad<s> x)	{return so().pool##n;}
	pool(8,8)
	pool(sizeof(Variable::vObject),VO)
	#undef pool
	static VMemoryPool &vpool()		{return so().vpool;}
	struct String
	{
		static const string &destructor()		{return so().destructor;}
	};
	friend class String;
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

#undef OBJECT_SIZE
