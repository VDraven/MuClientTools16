
#ifndef ITEM_BMD_H
#define ITEM_BMD_H

#define _ITEM_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<ITEM_ATTRIBUTE> ItemBmdConvert;
class ItemBmd : public ItemBmdConvert
{
public:
	ItemBmd() : ItemBmdConvert(_ITEM_BMD_WKEY_) {};
	virtual ~ItemBmd() {};

	/*
	BOOL Unpack(const char *szSrcBmd, const char *szDestTxt)
	{	
		sizeof(ITEM_ATTRIBUTE);
	return FileOpen(szSrcBmd)
		&& Decrypt()
		&& FileWrite(fs::path(szSrcBmd).replace_extension(".debug").string().c_str());
	};
	*/
private:

	int GetKey(ITEM_ATTRIBUTE* ptr)
	{
		sizeof(ITEM_ATTRIBUTE);
		int check_index = (ptr->ItemSubGroup * 512) + ptr->ItemSubIndex;
		if (ptr->ItemIndex != check_index)
		{
			return -1;	//it will push the problematic row to the top of the out.txt file
		}

		return ptr->ItemIndex;
	};
};

#endif