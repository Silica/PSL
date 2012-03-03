
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
		mpool(size_t t)	{next = NULL;current = count = 0;ptr = new int[t];std::memset(ptr, 0, t*sizeof(int));}
		~mpool()		{
			// ����Ń��������[�N������o����
			// ���A�J������Ȃ��܂܏I������̂�����o���邾����
			// �J�������̂��I�����ł͌��ǈӖ����Ȃ�����
			// vBase�h���̃f�X�g���N�^�ŉ��炩��IO���������s���Ȃ�b�͕ʂ���
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

// ���ėp�Œ�T�C�Y�������}�l�[�W��
// �ŏ���4�o�C�g��0�ɂȂ�Ȃ��I�u�W�F�N�g����
// ���z�֐��e�[�u�������邩��virtual�Ȃ���v�A�Ȃ͂�
template<size_t S> class MemoryManager
{
public:
	const static int poolsize = 256;
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