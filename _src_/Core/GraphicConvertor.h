#ifndef IMG_CONVERTOR_H
#define IMG_CONVERTOR_H

constexpr char ToLower(char c)
{
	if (c >= 'A' && c <= 'Z') return 'a' + (c - 'A');
	return c;
}

constexpr DWORD Ext2Int(const char* e)
{
	DWORD n = 0;
	for (int i = 0; i < 4; i++)
		n |= ToLower(e[i]) << ((3 - i) * 8);
	return n;
}

//	Valid extensions:
constexpr const char EXT_OZJ[] = ".ozj";
constexpr const char EXT_OZT[] = ".ozt";
constexpr const char EXT_OZB[] = ".ozb";
constexpr const char EXT_OZP[] = ".ozp";
constexpr const char EXT_OZD[] = ".ozd";
constexpr const char EXT_OZG[] = ".ozg";

constexpr const char EXT_JPG[] = ".jpg";
constexpr const char EXT_TGA[] = ".tga";
constexpr const char EXT_BMP[] = ".bmp";
constexpr const char EXT_PNG[] = ".png";
constexpr const char EXT_DDS[] = ".dds";
constexpr const char EXT_GFX[] = ".gfx";

constexpr DWORD INT_OZJ = Ext2Int(EXT_OZJ);
constexpr DWORD INT_OZT = Ext2Int(EXT_OZT);
constexpr DWORD INT_OZB = Ext2Int(EXT_OZB);
constexpr DWORD INT_OZP = Ext2Int(EXT_OZP);
constexpr DWORD INT_OZD = Ext2Int(EXT_OZD);
constexpr DWORD INT_OZG = Ext2Int(EXT_OZG);

constexpr DWORD INT_JPG = Ext2Int(EXT_JPG);
constexpr DWORD INT_TGA = Ext2Int(EXT_TGA);
constexpr DWORD INT_BMP = Ext2Int(EXT_BMP);
constexpr DWORD INT_PNG = Ext2Int(EXT_PNG);
constexpr DWORD INT_DDS = Ext2Int(EXT_DDS);
constexpr DWORD INT_GFX = Ext2Int(EXT_GFX);

template <const char* EXT, size_t PAD>
class GraphicConvertor : public BaseIO
{
public:
	GraphicConvertor() {};
	virtual ~GraphicConvertor() {};

	constexpr const char* ExtReplace();
	virtual BOOL Unpack(const char *szSrc, const char *szDest);
	virtual BOOL Pack(const char *szSrc, const char *szDest);

private:
	virtual BOOL Decrypt();
	virtual BOOL Encrypt();
};

//--END HEADER PART

//------------------------------------------------------------------------
//--GraphicConvertor
//------------------------------------------------------------------------

template<const char* EXT, size_t PAD>
constexpr const char * GraphicConvertor<EXT, PAD>::ExtReplace()
{
	switch (Ext2Int(EXT))
	{
	case INT_OZJ:
		return EXT_JPG;
	case INT_OZT:
		return EXT_TGA;
	case INT_OZB:
		return EXT_BMP;
	case INT_OZP:
		return EXT_PNG;
	case INT_OZD:
		return EXT_DDS;
	case INT_OZG:
		return EXT_GFX;
	default:
		return "";
	}
}

template<const char* EXT, size_t PAD>
inline BOOL GraphicConvertor<EXT, PAD>::Unpack(const char * szSrc, const char * szDest)
{
	return FileOpen(szSrc)
		&& Decrypt()
		&& FileWrite(szDest ? szDest : fs::path(szSrc).replace_extension(ExtReplace()).string().c_str());
}

template<const char* EXT, size_t PAD>
inline BOOL GraphicConvertor<EXT, PAD>::Pack(const char * szSrc, const char * szDest)
{
	const char* original_ext = EXT;
	return FileOpen(szSrc)
		&& Encrypt()
		&& FileWrite(szDest ? szDest : fs::path(szSrc).replace_extension(original_ext).string().c_str());
}

template<const char* EXT, size_t PAD>
inline BOOL GraphicConvertor<EXT, PAD>::Decrypt()
{
	if (_buf.size() < PAD) return FALSE;

	_buf.erase(_buf.begin(), _buf.begin() + PAD);

	return TRUE;
}

template<const char* EXT, size_t PAD>
inline BOOL GraphicConvertor<EXT, PAD>::Encrypt()
{
	if (_buf.size() < PAD) return FALSE;

	BYTE dummy[PAD];
	memcpy(dummy, &_buf[0], PAD);

	_buf.insert(_buf.begin(), std::begin(dummy), std::end(dummy));

	return TRUE;
}

#endif