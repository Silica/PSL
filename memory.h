
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
		mpool()		{next = NULL;current = count = 0;std::memset(ptr, 0, poolsize*2*sizeof(int));}
		~mpool()	{
			// これでメモリリークを回避出来る
			// が、開放されないまま終了するのを回避出来るだけで
			// 開放されるのが終了時では結局意味がないかと
			// vBase派生のデストラクタで何らかのIO処理等を行うなら話は別だが
			for (int i = 0; i < poolsize*2; i += 2)
			{
				if (ptr[i])
				{
					Variable *v = (Variable*)(ptr+i);
					v->safedelete();
					// グローバルオブジェクトの開放順序は不明…つまり
					// こっちより先に下の汎用が死んでる可能性がある
					// それどころか幾つかstatic rsvとかあるよね
				}
			}
			delete next;
		}
		int *nextptr()
		{
			if (count == poolsize)
			{
				std::printf("extend\n");
				if (!next)
					next = new mpool;
				return next->nextptr();
			}
			for (int i = current; i < poolsize*2; i += 2)
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
			PSL_PRINTF(("throw,%d,%d\n", count, current));
			throw std::bad_alloc();
			#else
			return NULL;
			#endif
		}
		void release(int *p)
		{
			if (p < ptr || p >= ptr+poolsize*2)
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
		int ptr[poolsize*2];
		mpool *next;
		int current;
		int count;
	};
	static mpool &MemoryPool()
	{
		static mpool p;
		return p;
	}
};

// 準汎用固定サイズメモリマネージャ
// 最初の4バイトが0にならないオブジェクト限定
// 仮想関数テーブルがあるからvirtualなら大丈夫、なはず
template<size_t S, int poolsize = 256> class MemoryManager
{
public:
	static void *Next()				{return MemoryPool().nextptr();}
	static void Release(void *ptr)	{MemoryPool().release(ptr);}
	class mpool
	{
	public:
		mpool()		{next = NULL;current = count = 0;std::memset(ptr, 0, poolsize*S);}
		~mpool()	{delete next;}
		void *nextptr()
		{
			if (count == poolsize)
			{
				if (!next)
					next = new mpool;
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
		union DATA
		{
			char s[S];
			int x;
		} ptr[poolsize];
		int current;
		int count;
		mpool *next;
	};
	static mpool &MemoryPool()
	{
		static mpool p;
		return p;
	}
};
