#pragma once

//=========================================================

#ifndef ITEM_TOOLTIP_BMD_H
#define ITEM_TOOLTIP_BMD_H

#define _ITEM_TOOLTIP_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<ITEM_TOOLTIP> ItemToolTipBmdConvert;
class ItemToolTipBmd : public ItemToolTipBmdConvert
{
public:
	ItemToolTipBmd() : ItemToolTipBmdConvert(_ITEM_TOOLTIP_BMD_WKEY_) { hasCounter = false; };
	virtual ~ItemToolTipBmd() { };
	
private:
	BOOL Decrypt();
	BOOL Encrypt();
	//void TxtOut(std::ofstream & os);
	void TxtIn(std::ifstream& is);
};

#endif

//=========================================================

#ifndef ITEM_TOOLTIP_TEXT_BMD_H
#define ITEM_TOOLTIP_TEXT_BMD_H

#define _ITEM_TOOLTIP_TEXT_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<ITEM_TOOLTIP_TEXT> ItemToolTipTextBmdConvert;
class ItemToolTipTextBmd : public ItemToolTipTextBmdConvert
{
public:
	ItemToolTipTextBmd() : ItemToolTipTextBmdConvert(_ITEM_TOOLTIP_TEXT_BMD_WKEY_) { };
	virtual ~ItemToolTipTextBmd() { };

	/*
	BOOL Unpack(const char *szSrcBmd, const char *szDestTxt)
	{
		sizeof(ITEM_TOOLTIP_TEXT);
		return FileOpen(szSrcBmd)
			&& Decrypt()
			&& FileWrite(fs::path(szSrcBmd).replace_extension(".debug").string().c_str());
	};
	*/

private:
	BOOL Decrypt();
	BOOL Encrypt();
	int GetKey(ITEM_TOOLTIP_TEXT* ptr) { return ptr->ID; };
};

#endif