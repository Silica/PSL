class bytecode
{
public:
	typedef unsigned char byte;
	bytecode(){}
	bytecode(size_t t):code(t){code.resize(t);}
	byte *get()			{return &code[0];}
	size_t size()		{return code.size();}
	void push(byte b)	{code.push_back(b);}
	void push(const void *p, size_t l)
	{
		int size = code.size();
		code.resize(size + l);
		std::memcpy(&code[size], p, l);
	}
	void resize(size_t t)	{code.resize(t);}
private:
	#ifdef PSL_USE_STL_VECTOR
	std::
	#endif
	vector<byte> code;
};
class bcreader
{
	static string readString(bytecode::byte *&byte)
	{
		string s = (char*)byte;
		byte += s.length()+1;
		return s;
	}
	template<typename T>
	static T read(bytecode::byte *&byte)
	{
		T x = *(T*)byte;
		byte += sizeof(T);
		return x;
	}
	static bool readbyte(bytecode::byte *&byte, bytecode::byte *end, variable &v)
	{
		switch (*(byte++))
		{
		case OpCode::MNEMONIC::BEGIN:
			{
				variable x = v[readString(byte)];
				readcode(byte, end, x);
			}
			break;
		case OpCode::MNEMONIC::END:			return true;
		case OpCode::MNEMONIC::PUSH_HEX:	v.pushcode(new PUSH_HEX(read<hex>(byte)));break;
		case OpCode::MNEMONIC::PUSH_FLOAT:	v.pushcode(new PUSH_FLOAT(read<double>(byte)));break;
		#define OPC_I(n) case OpCode::MNEMONIC::n:{v.pushcode(new n(read<int>(byte)));}break;
			OPC_I(PUSH_INT)
			OPC_I(JMP)
			OPC_I(JT)
			OPC_I(JF)
			OPC_I(JR)
			OPC_I(JRT)
			OPC_I(JRF)
			OPC_I(LOCALINDEX)
		#undef OPC_I
		#define OPC_S(n) case OpCode::MNEMONIC::n:{string s = readString(byte);v.pushcode(new n(s));}break;
			OPC_S(PUSH_STRING)
			OPC_S(VARIABLE)
			OPC_S(GOTO)
			OPC_S(LOCAL)
			OPC_S(GLOBAL)
			OPC_S(STATIC)
			OPC_S(DECLARATION)
			OPC_S(MEMBER)
		#undef OPC_S
		#define OPCODE(n) case OpCode::MNEMONIC::n:v.pushcode(new n());break;
			OPCODE(POP)
			OPCODE(PUSH_NULL)
			OPCODE(SUBSTITUTION)
			OPCODE(ASSIGNMENT)
			OPCODE(ARGUMENT)
			OPCODE(PLUS)
			OPCODE(MINUS)
			OPCODE(NOT)
			OPCODE(COMPL)
			OPCODE(INC)
			OPCODE(PINC)
			OPCODE(DEC)
			OPCODE(PDEC)
			OPCODE(DEREF)
			OPCODE(REF)
			OPCODE(CLOSURE)
			OPCODE(SADD)
			OPCODE(SSUB)
			OPCODE(SMUL)
			OPCODE(SDIV)
			OPCODE(SMOD)
			OPCODE(SAND)
			OPCODE(SOR)
			OPCODE(SXOR)
			OPCODE(SSHL)
			OPCODE(SSHR)
//			OPCODE(MOV)
			OPCODE(ADD)
			OPCODE(SUB)
			OPCODE(MUL)
			OPCODE(DIV)
			OPCODE(MOD)
			OPCODE(AND)
			OPCODE(OR)
			OPCODE(XOR)
			OPCODE(SHL)
			OPCODE(SHR)
			OPCODE(EQ)
			OPCODE(NEQ)
			OPCODE(LE)
			OPCODE(GE)
			OPCODE(LT)
			OPCODE(GT)
			OPCODE(BAND)
			OPCODE(BOR)
			OPCODE(LIST)
			OPCODE(PARENTHESES)
			OPCODE(CALL)
			OPCODE(RETURN)
			OPCODE(BREAK)
			OPCODE(CONTINUE)
			OPCODE(YIELD)
			OPCODE(INSTANCE)
			OPCODE(INDEX)
		#undef OPCODE
		case OpCode::MNEMONIC::PUSH_CODE:
			{
				variable x;
				byte += 2;
				readcode(byte, end, x);
				v.pushcode(new PUSH_CODE(x));
			}
			break;
		case OpCode::MNEMONIC::IF:
		#ifdef PSL_DEBUG
			{
				variable x;
				readcode(byte, end, x);
				v.pushcode(new IF(x.getcode()));
			}
			break;
		#endif
		case OpCode::MNEMONIC::SCOPE:
			{
				variable x;
				readcode(byte, end, x);
				v.pushcode(new SCOPE(x.getcode()));
			}
			break;
		case OpCode::MNEMONIC::LOOP:
			{
				int l = read<int>(byte);
				variable x;
				readcode(byte, end, x);
				v.pushcode(new LOOP(x.getcode(), l));
			}
			break;
		case OpCode::MNEMONIC::LABELBEGIN:
			while (*byte != OpCode::MNEMONIC::END)
			{
				int l = read<int>(byte);
				string s = readString(byte);
				if (Code *c = v.getcode())
					c->pushlabel(s, l);
			}
			++byte;
			break;
		}
		return false;
	}
	static void readcode(bytecode::byte *&byte, bytecode::byte *end, variable &v)
	{
		while (byte != end)
			if (readbyte(byte, end, v))
				break;
	}
public:
	static void read(bytecode &b, variable &v)
	{
		bytecode::byte *byte = b.get();
		bytecode::byte *end = byte + b.size();
		if (b.size() < 2 || byte[0] != OpCode::MNEMONIC::BEGIN || byte[1] != 0)
			return;
		byte += 2;
		readcode(byte, end, v);
	}
};
