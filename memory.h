
// Variable::operator new専用(8バイト固定)メモリマネージャ

class MemoryManager
{
	const static int poolsize = 256;
public:
	static int *Next()
	{
		int *ptr = pool();
		for (int i = current(); i < poolsize; i += 2)
		{
			if (!ptr[i])
			{
				current() = i+2;
				count() += 1;;
				return ptr + i;
			}
		}
		for (int i = 0; i < current(); i += 2)
		{
			if (!ptr[i])
			{
				current() = i+2;
				count() += 1;;
				return ptr + i;
			}
		}
		std::printf("throw,%d,%d", count(), current());
		throw std::bad_alloc();
	}
	static void Release(void *ptr)
	{
		int *p = pool();
		current() = (int*)ptr - p;
		count()--;
		p[current()] = 0;
	}
private:
	class mpool
	{
		int *ptr;
		mpool *next;
		/*
		足りなくなったらリンクリストで拡張する
		NextとReleaseが管理するのではなく
		こいつのメンバに値を渡して再帰的に処理して貰う
		*/
	public:
		mpool(size_t t)	{ptr = new int[t];std::memset(ptr, 0, t*sizeof(int));}
		~mpool()		{
			// これでメモリリークを回避出来る
			// が、開放されないまま終了するのを回避出来るだけで
			// 開放されるのが終了時では結局意味がないかと
			// vBase派生のデストラクタで何らかのIO処理等を行うなら話は別だが
			for (int i = 0; i < poolsize; i += 2)
			{
				if (ptr[i])
				{
					Variable *v = (Variable*)(ptr+i);
					v->safedelete();
				}
			}
			delete []ptr;
//			std::printf("end,%d,%d", count(), current());
		}
		int *get()		{return ptr;}
	};
	static int *pool()
	{
		return MemoryPool().get();
	}
	static mpool &MemoryPool()
	{
		static mpool p(poolsize);
		return p;
	}
	static int &current()
	{
		static int c = 0;
		return c;
	}
	static int &count()
	{
		static int c = 0;
		return c;
	}
	friend class mpool;
};

