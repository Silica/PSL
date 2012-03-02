
// Variable::operator new��p(8�o�C�g�Œ�)�������}�l�[�W��

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
		����Ȃ��Ȃ����烊���N���X�g�Ŋg������
		Next��Release���Ǘ�����̂ł͂Ȃ�
		�����̃����o�ɒl��n���čċA�I�ɏ������ĖႤ
		*/
	public:
		mpool(size_t t)	{ptr = new int[t];std::memset(ptr, 0, t*sizeof(int));}
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

