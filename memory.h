
// Variable::operator new��p(8�o�C�g�Œ�)�������}�l�[�W��
// Variable�̓����o��0�ɂȂ�Ȃ��A�Ƃ����d�l�𗘗p���Ă���̂�
// ���ɉ��p����ɂ͎g�p���E���g�p�Ɋւ��锻��̏C�����K�v

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
		����Ȃ��Ȃ����烊���N���X�g�Ŋg������
		Next��Release���Ǘ�����̂ł͂Ȃ�
		�����̃����o�ɒl��n���čċA�I�ɏ������ĖႤ
		*/
		int current;
		int count;
	public:
		mpool(size_t t)	{current = count = 0;ptr = new int[t];std::memset(ptr, 0, t*sizeof(int));}
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

