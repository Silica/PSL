
// Variable::operator new��p(8�o�C�g�Œ�)�������}�l�[�W��
// Variable�̓����o��0�ɂȂ�Ȃ��A�Ƃ����d�l�𗘗p���Ă���̂�
// ���ɉ��p����ɂ͎g�p���E���g�p�Ɋւ��锻��̏C�����K�v

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
			// ����Ń��������[�N������o����
			// ���A�J������Ȃ��܂܏I������̂�����o���邾����
			// �J�������̂��I�����ł͌��ǈӖ����Ȃ�����
			// vBase�h���̃f�X�g���N�^�ŉ��炩��IO���������s���Ȃ�b�͕ʂ���
			for (int i = 0; i < poolsize*2; i += 2)
			{
				if (ptr[i])
				{
					Variable *v = (Variable*)(ptr+i);
					v->safedelete();
					// �O���[�o���I�u�W�F�N�g�̊J�������͕s���c�܂�
					// ����������ɉ��̔ėp������ł�\��������
					// ����ǂ��납���static rsv�Ƃ�������
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

// ���ėp�Œ�T�C�Y�������}�l�[�W��
// �ŏ���4�o�C�g��0�ɂȂ�Ȃ��I�u�W�F�N�g����
// ���z�֐��e�[�u�������邩��virtual�Ȃ���v�A�Ȃ͂�
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
