#ifndef TXT_CONVERTOR_H
#define TXT_CONVERTOR_H

#define _COMMON_TXTCONVERTOR_WKEY_ 0xE2F1

template<typename T>
class TxtConvertor : public BaseIO
{
public:
	TxtConvertor(WORD w = _COMMON_TXTCONVERTOR_WKEY_) : _wkey(w), hasCounter(true), hasCRC(true) {};
	virtual ~TxtConvertor() {};

	virtual BOOL Unpack(const char *szSrcBmd, const char *szDestTxt);
	virtual BOOL Pack(const char *szSrcTxt, const char *szDestBmd);

protected:
	virtual int GetKey(T* ptr) { static int i = 0;  return i++; };
	virtual void MakeLabel(std::ofstream& os);
	virtual void MakeLabelEx(std::ofstream& os) {};
	virtual void OffsetOut(std::ofstream& os, T* ptr);
	virtual void InsertToMap(int key, T* ptr, int* err = nullptr);
	//----------------------

	virtual BOOL Decrypt();
	virtual BOOL Encrypt();
	virtual void TxtOut(std::ofstream& os);
	virtual void TxtIn(std::ifstream& is);
	virtual BOOL ComposeTxt(const char *szDestTxt);
	virtual BOOL ParseTxt(const char *szSrcTxt);

	std::map<int, T*> _map;
	WORD _wkey;
	bool hasCounter;
	bool hasCRC;
};

//--END HEADER PART

//------------------------------------------------------------------------
//--TxtConvertor
//------------------------------------------------------------------------

template<typename T>
inline BOOL TxtConvertor<T>::Unpack(const char * szSrcBmd, const char * szDestTxt)
{
	return FileOpen(szSrcBmd)
		&& Decrypt()
		&& ComposeTxt(szDestTxt ? szDestTxt : fs::path(szSrcBmd).replace_extension(".txt").string().c_str());
}

template<typename T>
inline BOOL TxtConvertor<T>::Pack(const char * szSrcTxt, const char * szDestBmd)
{
	return ParseTxt(szSrcTxt)
		&& Encrypt()
		&& FileWrite(szDestBmd ? szDestBmd : fs::path(szSrcTxt).replace_extension(".out.bmd").string().c_str());
}

template<typename T>
BOOL TxtConvertor<T>::Decrypt()
{
	assert(_buf.size());

	size_t size = sizeof(T);

	_map.clear();

	int err = -1; 
	for (size_t p = (hasCounter * 4); p + size <= _buf.size() - (hasCRC * 4); p += size)
	{
		Xor3Byte(&_buf[p], size);

		T *ptr = (T *)&_buf[p]; 
		int key = GetKey(ptr);
		InsertToMap(key, ptr, &err);
	}

	return TRUE;
}

template<typename T>
BOOL TxtConvertor<T>::Encrypt()
{
	assert(_buf.size());

	size_t size = sizeof(T);
	for (size_t p = (hasCounter * 4); p + size <= _buf.size() - (hasCRC * 4); p += size)
	{
		Xor3Byte(&_buf[p], size);
	}

	if (hasCRC)
	{
		DWORD CRC = CalculateCRC(&_buf[4], _buf.size() - 8, _wkey);
		*(DWORD*)&_buf[_buf.size() - 4] = CRC;
	}

	return TRUE;
}

template<typename T>
inline void TxtConvertor<T>::MakeLabel(std::ofstream & os)
{
	assert(os);

	static const std::string LABEL = T::GetLabel();
	os << LABEL << std::endl;
}

template<typename T>
inline void TxtConvertor<T>::OffsetOut(std::ofstream & os, T* ptr)
{
	assert(os && ptr);

	static const std::vector<OffsetInfo> OFFSET = T::GetOffset();

	for (size_t i = 0; i < OFFSET.size(); i++)
	{
		int type = OFFSET[i].Type;
		size_t pos = OFFSET[i].Offset;
		if (pos < 0 || pos >= sizeof(T)) continue;

		pos += (size_t)ptr;
		switch (type)
		{
		case LAZY_TYPE_FLAG::_CSTR_:
			os << (const char*)pos << '\t';
			break;
		case LAZY_TYPE_FLAG::_1BYTE_:
			os << (DWORD)(*(BYTE*)pos) << '\t';
			break;
		case LAZY_TYPE_FLAG::_1BYTE_ | LAZY_TYPE_FLAG::_SIGNED_:
			os << (int)(*(char*)pos) << '\t';
			break;
		case LAZY_TYPE_FLAG::_2BYTE_:
			os << *(WORD*)pos << '\t';
			break;
		case LAZY_TYPE_FLAG::_2BYTE_ | LAZY_TYPE_FLAG::_SIGNED_:
			os << *(short*)pos << '\t';
			break;
		case LAZY_TYPE_FLAG::_4BYTE_:
			os << *(DWORD*)pos << '\t';
			break;
		case LAZY_TYPE_FLAG::_4BYTE_ | LAZY_TYPE_FLAG::_SIGNED_:
			os << *(int*)pos << '\t';
			break;
		case LAZY_TYPE_FLAG::_FLOAT_:
			os << *(float*)pos << '\t';
			break;
		case LAZY_TYPE_FLAG::_DOUBLE_:
			os << *(double*)pos << '\t';
			break;
		default:
			break;
		}
	}
}

template<typename T>
inline void TxtConvertor<T>::InsertToMap(int key, T * ptr, int* err)
{
	if (err)
	{
		while (_map.find(key) != _map.end())	//check duplicated keys
		{
			key = *err;
			*err--;
		}
	}
	_map.insert(std::make_pair(key, ptr));
}

template<typename T>
void TxtConvertor<T>::TxtOut(std::ofstream & os)
{
	assert(os);

	MakeLabelEx(os);
	MakeLabel(os);

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		T* ptr = it->second;
		OffsetOut(os, ptr);
		os << std::endl;
	}
}

template<typename T>
void TxtConvertor<T>::TxtIn(std::ifstream & is)
{
	assert(is);

	static const std::vector<OffsetInfo> OFFSET = T::GetOffset();

	std::string line;
	size_t size = sizeof(T);
	size_t head = hasCounter * 4;
	size_t n = 0;

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;
		_buf.resize(head + ((n + 1) * size));

		T* ptr = (T*)&_buf[head + (n * size)];
		memset(ptr, 0x00, size);

		line += '\t';
		size_t a = 0;
		size_t b = line.find('\t', a);
		size_t i = 0;
		do
		{
			if (b > a)
			{
				std::string s = line.substr(a, b - a);
				size_t pos = OFFSET[i].Offset + (size_t)ptr;

				if (OFFSET[i].Type == LAZY_TYPE_FLAG::_CSTR_)
				{
					size_t len = sizeof(T);
					if (i < OFFSET.size() - 1)
						len = OFFSET[i + 1].Offset;
					len -= OFFSET[i].Offset;
					if (len - 1 < s.length())
						s.resize(len - 1);
				}

				sscanf(s.c_str(), OFFSET[i].Format.c_str(), (void*)pos);
			}

			i++;
			a = b + 1;
			b = line.find('\t', a);
		} while (b != std::string::npos && i < OFFSET.size());
		
		n++;
	}
	if(hasCounter)
		*(DWORD*)&_buf[0] = n;
	if(hasCRC)
		_buf.resize(4 + (n * size) + 4);	//last 4 BYTES for CRC
}

template<typename T>
BOOL TxtConvertor<T>::ComposeTxt(const char * szDestTxt)
{
	assert(_map.size() && szDestTxt);

	fs::path pFile = Utls::BackupPath(szDestTxt);
	Utls::CreateParentDir(pFile);

	std::ofstream os(pFile);
	if (!os.is_open())
	{
		PRINT_DEBUG("[ERROR] Failed to write the txt file: " << szDestTxt);
		return FALSE;
	}

	TxtOut(os);

	os.close();

	return TRUE;
}

template<typename T>
BOOL TxtConvertor<T>::ParseTxt(const char * szSrcTxt)
{
	if(!szSrcTxt) return FALSE;

	std::ifstream is(szSrcTxt);
	if (!is.is_open())
	{
		PRINT_DEBUG("[ERROR] Failed to read the txt file: " << szSrcTxt);
		return FALSE;
	}

	_map.clear();

	TxtIn(is);

	is.close();

	return TRUE;
}
#endif