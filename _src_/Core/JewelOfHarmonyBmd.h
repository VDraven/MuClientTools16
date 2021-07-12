
#ifndef JEWEL_OF_HARMONY_OPTION_BMD_H
#define JEWEL_OF_HARMONY_OPTION_BMD_H

typedef TxtConvertor<JEWEL_OF_HARMONY_OPTION> JewelOfHarmonyOptionBmdConvert;
class JewelOfHarmonyOptionBmd : public JewelOfHarmonyOptionBmdConvert
{
public:
	JewelOfHarmonyOptionBmd() : JewelOfHarmonyOptionBmdConvert() { hasCounter = false; hasCRC = false; };
	virtual ~JewelOfHarmonyOptionBmd() {};

	//BOOL Unpack(const char *szSrcBmd, const char *szDestTxt)
	//{	
	//	return FileOpen(szSrcBmd)
	//		&& Decrypt()
	//		&& FileWrite(fs::path(szSrcBmd).replace_extension(".debug").string().c_str());
	//};

private:
	BOOL Decrypt();
	void TxtIn(std::ifstream& is);
};

#endif