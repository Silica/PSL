#ifndef PSTRING_H
#define PSTRING_H

#define PSTRING_USE_DOUBLE

#include <cstdio>
#include <cstdarg>
#ifdef PSTRING_USE_DOUBLE
#include <cstdlib>
#endif

//#pragma warn -8027


class string
{
	typedef unsigned long size_t;
public:	/* static */
	static size_t len(const char *s)
	{
		int i = 0;
		for (; s[i] ; i++);
		return i;
	}
	static int min(int x, int y)
	{
		return (x < y) ? x : y;
	}
private:
	class SharedBuffer
	{
	public:
		/* constructor */
		SharedBuffer()
		{
			buffer = NULL;
			length = 0;
			reference = NULL;
		}
		SharedBuffer(size_t l)
		{
#ifdef _DEBUG
			create()++;
#endif
			buffer = new char[l];
			length = l;
			reference = new int(1);
		}
		SharedBuffer(size_t l, const char *s)
		{
#ifdef _DEBUG
			create()++;
#endif
			buffer = new char[l];
			Copy(l, s);
			length = l;
			reference = new int(1);
		}
		SharedBuffer(const SharedBuffer &s)
		{
			buffer = s.buffer;
			length = s.length;
			reference = s.reference;
			if (reference)	(*reference)++;
		}
		~SharedBuffer()
		{
			release();
		}
		/* operator */
		SharedBuffer &operator=(const SharedBuffer &s)
		{
			release();
			buffer = s.buffer;
			length = s.length;
			reference = s.reference;
			if (reference)	(*reference)++;
			return *this;
		}
		char &operator[](size_t i) const	{return buffer[i];}
	//	char &operator[](size_t i)			{return buffer[i];}
	//	char operator[](size_t i) const	{return buffer[i];}
		bool operator==(const SharedBuffer &s) const	{return buffer == s.buffer;}
		/* getter */
		size_t Length() const		{return length;}
		int Reference() const	{if (!reference) return 0;	return *reference;}
		/*  */
		void Add(const int c, const int l, const char *s)
		{
			for (int i = c; i < l; i++)
			{
				buffer[i] = s[i-c];
			}
		}
		void Add(const int c, const int l, const SharedBuffer &s)
		{
			Add(c, l, s.buffer);
		}
		void Copy(const int l, const SharedBuffer &s)
		{
			Copy(l, s.buffer);
		}
		void Copy(const int l, const char *s)
		{
			for (int i = 0; i < l; i++)
				buffer[i] = s[i];
		}
		void CopyTo(size_t l, char *&buf) const
		{
			for (size_t i = 0; i < l && i < length && buffer[i]; i++)
				buf[i] = buffer[i];
		}
		/* debug */
#ifdef _DEBUG
		static int &create()
		{
			static int c = 0;
			return c;
		}
		static int &del()
		{
			static int d = 0;
			return d;
		}
#endif
	private:
		char *buffer;
		size_t length;
		int  *reference;
		void release()
		{
			if (!buffer)
				return;
			if (!--(*reference))
			{
#ifdef _DEBUG
				del()++;
#endif
				delete reference;
				delete[] buffer;
			}
			buffer = NULL;
		}
	};
	const static char ZERO = '0';
	const static char MINUS = '-';
	const static char SPACE = ' ';
	const static char TAB = '\t';
	const static size_t DOUBLE_L = 64;	/* DOUBLE型の桁数以上の値にする */
	const static size_t SPARE = 11;		/* 最低限数値を表現出来る桁数で(>10) */
	string(size_t l, const SharedBuffer &b) : _len(l), buf(b){}
public:
	/* constructor */
	string(){_len = 0;};
	string(const string &s) : buf(s.buf){_len = s._len;}
	string(const char *s) : _len(len(s)), buf(_len, s){}
	string(const char *s, int i) : _len(i), buf(_len, s){}
	string(int i) : buf(SPARE)
	{
		int c = 0;
		if (i < 0)
		{
			buf[c++] = MINUS;
			i = -i;
		}
		if (i > 999999999)	buf[c++] = (char)(i/1000000000 %10 + ZERO);
		if (i > 99999999)	buf[c++] = (char)(i/100000000  %10 + ZERO);
		if (i > 9999999)	buf[c++] = (char)(i/10000000   %10 + ZERO);
		if (i > 999999)		buf[c++] = (char)(i/1000000    %10 + ZERO);
		if (i > 99999)		buf[c++] = (char)(i/100000     %10 + ZERO);
		if (i > 9999)		buf[c++] = (char)(i/10000      %10 + ZERO);
		if (i > 999)		buf[c++] = (char)(i/1000       %10 + ZERO);
		if (i > 99)			buf[c++] = (char)(i/100        %10 + ZERO);
		if (i > 9)			buf[c++] = (char)(i/10         %10 + ZERO);
							buf[c++] = (char)(i            %10 + ZERO);
		_len = c;
	}
	string(char c) : buf(SPARE)
	{
		buf[0] = c;
		_len = 1;
	};
#ifdef PSTRING_USE_DOUBLE
	string(double d) : buf(DOUBLE_L)
	{
		_len = std::sprintf(&buf[0], "%f", d);
	};
#endif
	~string(){};
	/* operator */
	string &operator=(const string &s)
	{
		_len = s._len;
		buf = s.buf;
		return *this;
	}
	string &operator+=(const string &s)
	{
		size_t l = _len + s._len;
		if (buf.Reference() == 1 && buf.Length() >= l)
		{
			buf.Add(_len, l, s.buf);
		}
		else
		{
			SharedBuffer nbuf(l+SPARE);
			nbuf.Copy(_len, buf);
			nbuf.Add(_len, l, s.buf);
			buf = nbuf;
		}
		_len = l;
		return *this;
	}
	string operator+(const string &s)
	{
		string n = *this;
		n += s;
		return n;
	}
	string &operator=(const char *s)
	{
		size_t l = len(s);
		if (buf.Reference() == 1 && buf.Length() >= l)
		{
			buf.Copy(l, s);
		}
		else
		{
			SharedBuffer nbuf(l, s);
			buf = nbuf;
		}
		_len = l;
		return *this;
	}
	string &operator+=(const char *s)
	{
		size_t l = _len + len(s);
		if (buf.Reference() == 1 && buf.Length() >= l)
		{
			buf.Add(_len, l, s);
		}
		else
		{
			SharedBuffer nbuf(l+SPARE);
			nbuf.Copy(_len, buf);
			nbuf.Add(_len, l, s);
			buf = nbuf;
		}
		_len = l;
		return *this;
	}
	string operator+(const char *s)
	{
		string n = *this;
		n += s;
		return n;
	}
	string &operator=(int i)
	{
		string s(i);
		*this = s;
		return *this;
	}
	string &operator+=(int i)
	{
		string s(i);
		*this += s;
		return *this;
	}
	string operator+(int i)
	{
		string s = *this;
		s += i;
		return s;
		// 上の方が無駄にstringを作らなくて済むが、
		// 結局のところ+=stringの実装の問題なので何とも言えず
		// もっとパーツを分割すべきなのだろうが
		// intの解析がint引数のコンストラクタにのみあるというのがまずまずい
/*		string s(i);
		return *this + s;*/
	}
	string &operator=(char c)
	{
		if (buf.Reference() != 1 || buf.Length() == 0)
		{
			SharedBuffer nbuf(SPARE);
			buf = nbuf;
		}
		buf[0] = c;
		_len = 1;
		return *this;
	}
	string &operator+=(char c)
	{
		if (buf.Reference() == 1 && buf.Length() > _len)
		{
			buf[_len++] = c;
		}
		else
		{
			SharedBuffer nbuf(_len+SPARE);
			nbuf.Copy(_len, buf);
			nbuf[_len++] = c;
			buf = nbuf;
/*			string s(c);
			*this += s;*/
		}
		return *this;
	}
	string operator+(char c)
	{
		string s = *this;
		s += c;
		return s;
/*		string s(c);
		return *this + s;*/
	}
	operator int() const
	{
		if (!_len)	return 0;
		bool minus = false;
		size_t c = 0;
		int r = 0;
		for (; c < _len; c++)
		{
			if (buf[c] != SPACE && buf[c] != TAB)
				break;
		}
		if (buf[c] == MINUS)
		{
			minus = true;
			c++;
		}
		for (; c < _len; c++)
		{
			if (!(buf[c] >= ZERO && buf[c] <= ZERO+9))
				break;
			r *= 10;
			r += buf[c] - ZERO;
		}
		if (minus)
			r = -r;
		return r;
	}
	operator char() const
	{
		if (_len > 0)
			return buf[0];
		return 0;
	}
	operator const char*() const
	{
		return c_str();
	}
/*	template <class T> operator T() const
	{
		return (int)(*this);
	}*/
#ifdef PSTRING_USE_DOUBLE
	string &operator=(double d)
	{
		string s(d);
		*this = s;
		return *this;
	}
	string &operator+=(double d)
	{
		string s(d);
		*this += s;
		return *this;
	}
	string operator+(double d)
	{
		string s(d);
		return *this + s;
	}
	operator double() const
	{
		return std::strtod(c_str(), NULL);
	}
#endif
	bool operator==(const string &s) const
	{
		if (buf == s.buf)			return true;
		if (_len != s._len)			return false;
		for (size_t i = 0; i < _len; i++)
			if (buf[i] != s.buf[i])	return false;
		return true;
	}
	bool operator!=(const string &s) const
	{
		return !(*this==s);
	}
	bool operator==(const char *s) const
	{
		for (size_t i = 0; i < _len; i++)
		{
			if (s[i] == 0)		return false;
			if (buf[i] != s[i])	return false;
		}
		if (s[_len] != 0)	return false;
		return true;
	}
	bool operator!=(const char *s) const
	{
		return !(*this==s);
	}
	bool operator<=(const string &s) const
	{
		if (buf == s.buf)			return true;
		size_t max = min(_len, s._len);
		for (size_t i = 0; i < max; i++)
		{
			if (buf[i] < s.buf[i])	return true;
			if (buf[i] > s.buf[i])	return false;
		}
		if (_len <= s._len)			return true;
		return false;
	}
	bool operator<(const string &s) const
	{
		return !(*this >= s);
/*		if (buf == s.buf)			return false;
		int max = min(_len, s._len);
		for (int i = 0; i < max; i++)
		{
			if (buf[i] < s.buf[i])	return true;
			if (buf[i] > s.buf[i])	return false;
		}
		if (_len < s._len)			return true;
		return false;*/
	}
	bool operator>=(const string &s) const
	{
		if (buf == s.buf)			return true;
		size_t max = min(_len, s._len);
		for (size_t i = 0; i < max; i++)
		{
			if (buf[i] < s.buf[i])	return false;
			if (buf[i] > s.buf[i])	return true;
		}
		if (_len >= s._len)			return true;
		return false;
	}
	bool operator>(const string &s) const
	{
		return !(*this <= s);
/*		if (buf == s.buf)			return false;
		int max = min(_len, s._len);
		for (int i = 0; i < max; i++)
		{
			if (buf[i] < s.buf[i])	return false;
			if (buf[i] > s.buf[i])	return true;
		}
		if (_len > s._len)			return true;
		return false;*/
	}
	const char *c_str(void) const
	{
		if (buf.Length() > _len)
		{	/* buf.Length()に余裕がある場合は単に0を追加して返す */
			buf[_len] = 0;
			return &buf[0];
		}
		if (c_s.l < _len+1)
			c_s.allocate(_len+SPARE);
		buf.CopyTo(_len, c_s.s);
		c_s.s[_len] = 0;
		return c_s.s;
	};
	size_t length() const	{return _len;};
	/* 操作 */
	int find(char c, size_t i = 0)	/* i位置から検索してcの文字を発見した位置を返す */
	{
		if (i >= 0)
		{
			for (; i < _len; i++)
			{
				if (buf[i] == c)
					return i;
			}
		}
		return -1;
	}
	int rfind(char c, size_t i = 0)	/* i位置から逆方向に検索してcの文字を発見した位置を返す */
	{
		if (i >= 0)
		{
			if (i == 0 || i >= _len) i = _len-1;
			for (; i >= 0; i--)
			{
				if (buf[i] == c)
					return i;
			}
		}
		return -1;
	}
	int copy(const char *s, size_t l, size_t i)	/* iからの位置にsをl文字分コピーする */
	{
		int r = 0;
		SharedBuffer nbuf(_len);
		for (size_t x = 0 ;x < _len; x++)
		{
			if (x >= i && x < i+l)
			{
				if (s[x-i])
				{
					nbuf[x] = s[x-i];
					r++;
					continue;
				}
				l = 0;
			}
			nbuf[x] = buf[x];
		}
		buf = nbuf;
		return r;
	}
	string substr(size_t i = 0, size_t l = 0)	/* iからl文字を切り出した文字列 */
	{
		if (!l || (i + l > _len))	l = _len - i;
		SharedBuffer nbuf(l, &buf[i]);
		string s(l, nbuf);
		return s;
	}
	string &reverse()	/* 反転させる */
	{
		SharedBuffer nbuf(_len);
		for (size_t i = 0; i < _len; i++)
			nbuf[i] = buf[_len-i-1];
		buf = nbuf;
		return *this;
	}
	string &sprintf(const char *format, ...)	/* 危険、使うな */
	{
		using namespace std;
		SharedBuffer nbuf(1024);
		va_list arg;
		va_start(arg, format);
		_len = vsprintf(&nbuf[0], format, arg);
		va_end(arg);
		buf = nbuf;
		return *this;
	}
	string &operator-=(int i)	/* 末尾からi文字を削る */
	{
		_len -= i;
		if (_len < 0)
			_len = 0;
		if (buf.Reference() != 1)
		{
			if (!_len)
			{
				SharedBuffer nbuf(SPARE);
				buf = nbuf;
			}
			else
			{
				SharedBuffer nbuf(_len, &buf[0]);
				buf = nbuf;
			}
		}
		return *this;
	}
	string operator-(int i)
	{
		string s = *this;
		s -= i;
		return s;
	}
	string &operator/=(size_t i)	/* 先頭からi文字までの文字列 */
	{
		if (i > _len)
			return *this;
		if (buf.Reference() != 1)
		{
			SharedBuffer nbuf(i+SPARE);
			nbuf.Copy(i, buf);
			buf = nbuf;
		}
		_len = i;
		return *this;
	}
	string operator/(int i)
	{
		string s = *this;
		s /= i;
		return s;
	}
	string &operator%=(int i)	/* 先頭i文字分を削る */
	{
		_len -= i;
		if (_len <= 0)
		{
			_len = 0;
			if (buf.Reference() == 1)
				return *this;
			SharedBuffer nbuf(SPARE);
			buf = nbuf;
		}
		else
		{
			SharedBuffer nbuf(_len, &buf[i]);
			buf = nbuf;
		}
		return *this;
	}
	string operator%(int i)
	{
		string s = *this;
		s %= i;
		return s;
	}
	unsigned long hash() const
	{
		union
		{
			struct
			{
				unsigned char c[2];
				unsigned short s;
			};
			unsigned long l;
		} u = {buf[0] , buf[_len-1], _len};
		return u.l;
	}
private:
	size_t _len;
	SharedBuffer buf;
	mutable struct CS
	{
		char *s;
		size_t l;
		CS()	{s = NULL;l = 0;}
		~CS()	{delete[] s;}
		void allocate(size_t i) {delete[] s;s = new char[i];l = i;}
	} c_s;
};

//#pragma warn +8027

#endif
