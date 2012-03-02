
// Variable::operator new専用(8バイト固定)メモリマネージャ
// Variableはメンバが0にならない、という仕様を利用しているので
// 他に応用するには使用中・未使用に関する判定の修正が必要

class MemoryManager
{
public:
	const static int poolsize = 256;
	static int *Next()				{return MemoryPool().nextptr();}
	static void Release(void *ptr)	{MemoryPool().release((int*)ptr);}
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
		int current;
		int count;
	public:
		mpool(size_t t)	{current = count = 0;ptr = new int[t];std::memset(ptr, 0, t*sizeof(int));}
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
		int *nextptr()
		{
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
			std::printf("throw,%d,%d\n", count, current);
			throw std::bad_alloc();
		}
		void release(int *p)
		{
			current = p - ptr;
			count--;
			ptr[current] = 0;
		}
	};
	static mpool &MemoryPool()
	{
		static mpool p(poolsize);
		return p;
	}
};

