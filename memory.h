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
protected:
	union DATA
	{
		char s[S];
		int x;
	} ptr[poolsize];
	MemoryPool *next;
	const static int psize = poolsize;
private:
	int current;
	int count;
};

class VMemoryPool : public MemoryPool<8>
{
public:
	~VMemoryPool()	{
		// ����Ń��������[�N������o����
		// ���A�J������Ȃ��܂܏I������̂�����o���邾����
		// �J�������̂��I�����ł͌��ǈӖ����Ȃ�����
		// vBase�h���̃f�X�g���N�^�ŉ��炩��IO���������s���Ȃ�b�͕ʂ���
		for (int i = 0; i < psize; ++i)
		{
			if (ptr[i].x)
			{
				Variable *v = (Variable*)(ptr+i);
				v->safedelete();
			}
		}
	}
};

class StaticObject
{
	struct sobj
	{
		MemoryPool<8> *pool8;
		MemoryPool<44> *pool44;
		VMemoryPool *vpool;
		sobj()
		{
			pool8 = new MemoryPool<8>;
			pool44 = new MemoryPool<44>;
			vpool = new VMemoryPool;
		}
		~sobj()
		{
			delete optimizer_p();
			delete rsvnull_p();
			delete vpool;
			delete pool44;
			delete pool8;
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
	static Environment *optimizer_p()
	{
		static Environment *optimizer = new Environment;
		return optimizer;
	}
public:
	static rsv &rsvnull()			{return *rsvnull_p();}
	static Environment &optimizer()	{return *optimizer_p();}
	static MemoryPool<8> &pool(OverLoad<8> x)	{return *so().pool8;};
	static MemoryPool<44> &pool(OverLoad<44> x)	{return *so().pool44;};
	static VMemoryPool &vpool()		{return *so().vpool;}
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
