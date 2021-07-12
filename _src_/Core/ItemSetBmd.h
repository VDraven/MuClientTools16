
#ifndef ITEM_SET_TYPE_BMD_H
#define ITEM_SET_TYPE_BMD_H

#define _ITEM_SET_TYPE_BMD_WKEY_ 0xE5F1

typedef TxtConvertor<ITEM_SET_TYPE> ItemSetTypeBmdConvert;
class ItemSetTypeBmd : public ItemSetTypeBmdConvert
{
public:
	ItemSetTypeBmd() : ItemSetTypeBmdConvert(_ITEM_SET_TYPE_BMD_WKEY_) {};
	virtual ~ItemSetTypeBmd() {};

private:
	BOOL Decrypt();
	BOOL Encrypt();
	void TxtOut(std::ofstream & os);
	void TxtIn(std::ifstream& is);
};

#endif

//=========================================================

#ifndef ITEM_SET_OPTION_BMD_H
#define ITEM_SET_OPTION_BMD_H

#define _ITEM_SET_OPTION_BMD_WKEY_ 0xA2F1

typedef TxtConvertor<ITEM_SET_OPTION> ItemSetOptionBmdConvert;
class ItemSetOptionBmd : public ItemSetOptionBmdConvert
{
public:
	ItemSetOptionBmd() : ItemSetOptionBmdConvert(_ITEM_SET_OPTION_BMD_WKEY_) {};
	virtual ~ItemSetOptionBmd() { };
	/*
	BOOL Unpack(const char *szSrcBmd, const char *szDestTxt)
	{
		sizeof(ITEM_SET_OPTION);
		return FileOpen(szSrcBmd)
			&& Decrypt()
			&& FileWrite(fs::path(szSrcBmd).replace_extension(".debug").string().c_str());
	};
	*/
private:
	BOOL Decrypt();
	BOOL Encrypt();
	void TxtOut(std::ofstream & os);
	void TxtIn(std::ifstream& is);
};

#endif

//=========================================================

#ifndef ITEM_SET_OPTION_TEXT_BMD_H
#define ITEM_SET_OPTION_TEXT_BMD_H

#define _ITEM_SET_OPTION_TEXT_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<ITEM_SET_OPTION_TEXT> ItemSetOptionTextBmdConvert;
class ItemSetOptionTextBmd : public ItemSetOptionTextBmdConvert
{
public:
	ItemSetOptionTextBmd() : ItemSetOptionTextBmdConvert(_ITEM_SET_OPTION_TEXT_BMD_WKEY_) {};
	virtual ~ItemSetOptionTextBmd() { };
};

#endif