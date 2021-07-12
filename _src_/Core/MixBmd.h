
#ifndef MIX_BMD_H
#define MIX_BMD_H

typedef TxtConvertor<MIX_INFO> MixBmdConvert;
class MixBmd : public MixBmdConvert
{
public:
	MixBmd() : MixBmdConvert() { hasCounter = false; hasCRC = false; };
	virtual ~MixBmd() { sizeof(MIX_INFO); sizeof(MIX_ITEM); };

	//BOOL Unpack(const char *szSrcBmd, const char *szDestTxt)
	//{	
	//	return FileOpen(szSrcBmd)
	//		&& Decrypt()
	//		&& FileWrite(fs::path(szSrcBmd).replace_extension(".debug").string().c_str());
	//};

private:

	BOOL Decrypt();
	BOOL Encrypt();
	void TxtOut(std::ofstream & os);
	void TxtIn(std::ifstream& is);
};

#endif