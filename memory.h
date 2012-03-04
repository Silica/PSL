
// Variable::operator new専用(8バイト固定)メモリマネージャ
// Variableはメンバが0にならない、という仕様を利用しているので
// 他に応用するには使用中・未使用に関する判定の修正が必要

class VMemoryManager
{
public:
	const static int poolsize = 256;
	static int *Next()				{return MemoryPool().nextptr();}
	static void Release(void *ptr)	{MemoryPool().release((int*)ptr);}
private:
	class mpool
	{
	public:
		mpool(size_t t)	{next = NULL;current = count = 0;ptr = new int[t];std::memset(ptr, 0, t*sizeof(int));}
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
			delete next;
//			std::printf("end,%d,%d", count(), current());
		}
		int *nextptr()
		{
			if (count == poolsize/2)
			{
				if (!next)
					next = new mpool(poolsize);
				return next->nextptr();
			}
			for (int i = current; i < poolsize; i += 2)
			{
				if (!ptr[i])
				{
					current = i+2;
					count += 1;
					return ptr + i;
				}
			}
			for (int i = 0; i < current; i += 2)
			{
				if (!ptr[i])
				{
					current = i+2;
					count += 1;
					return ptr + i;
				}
			}
			#ifdef _DEBUG
			std::printf("throw,%d,%d\n", count, current);
			throw std::bad_alloc();
			#else
			return NULL;
			#endif
		}
		void release(int *p)
		{
			if (p < ptr || p >= ptr+poolsize)
			{
				#ifdef _DEBUG
				if (next)
				#endif
				next->release(p);
				return;
			}
			current = p - ptr;
			count--;
			ptr[current] = 0;
		}
	private:
		int *ptr;
		mpool *next;
		int current;
		int count;
	};
	static mpool &MemoryPool()
	{
		static mpool p(poolsize);
		return p;
	}
};

// 準汎用固定サイズメモリマネージャ
// 最初の4バイトが0にならないオブジェクト限定
// 仮想関数テーブルがあるからvirtualなら大丈夫、なはず
template<size_t S, int poolsize = 256> class MemoryManager
{
public:
//	const static int poolsize = 256;
	static void *Next()				{return MemoryPool().nextptr();}
	static void Release(void *ptr)	{MemoryPool().release((char*)ptr);}
	class mpool
	{
	public:
		mpool(size_t t)	{next = NULL;current = count = 0;ptr = new char[t*S];std::memset(ptr, 0, t*S);}
		~mpool()		{delete []ptr;delete next;}
		void *nextptr()
		{
			if (count == poolsize)
			{
				if (!next)
					next = new mpool(poolsize);
				return next->nextptr();
			}
			for (int i = current; i < poolsize*S; i += S)
			{
				if (!ptr[i])
				{
					current = i+S;
					count += 1;
					return ptr + i;
				}
			}
			for (int i = 0; i < current; i += S)
			{
				if (!ptr[i])
				{
					current = i+S;
					count += 1;
					return ptr + i;
				}
			}
			#ifdef _DEBUG
			std::printf("throw,%d,%d\n", count, current);
			throw std::bad_alloc();
			#else
			return NULL;
			#endif
		}
		void release(char *p)
		{
			if (p < ptr || p >= ptr+poolsize*S)
			{
				#ifdef _DEBUG
				if (next)
				#endif
				next->release(p);
				return;
			}
			current = p - ptr;
			count--;
			*(int*)(ptr+current) = 0;
		}
	private:
		char *ptr;
		int current;
		int count;
		mpool *next;
	};
	static mpool &MemoryPool()
	{
		static mpool p(poolsize);
		return p;
	}
};
