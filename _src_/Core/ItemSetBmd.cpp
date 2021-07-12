#include "ItemSetBmd.h"

//------------------------------------------------------------------------
//--ItemSetTypeBmd
//------------------------------------------------------------------------

BOOL ItemSetTypeBmd::Decrypt()
{
	assert(_buf.size());

	size_t size = sizeof(ITEM_SET_TYPE);
	size_t count = ITEM_CATEGORY_MAX * ITEM_INDEX_MAX;
	size_t pos = 0;

	//assert(_buf.size() == count * size + 4);

	_map.clear();

	for (size_t i = 0; i < count; i++)
	{
		if (pos + size > _buf.size()) return TRUE; //ignore error

		Xor3Byte(&_buf[pos], size);
		ITEM_SET_TYPE* ptr = (ITEM_SET_TYPE*)&_buf[pos];
		if (ptr->Tier1 | ptr->Tier2 | ptr->Tier3 | ptr->Tier4 | ptr->Unk1 | ptr->Unk2 > 0)
		{
			int key = i;
			InsertToMap(key, ptr);
		}

		pos += size;
	}

	return TRUE;
}

BOOL ItemSetTypeBmd::Encrypt()
{
	size_t size = sizeof(ITEM_SET_TYPE);
	size_t count = ITEM_CATEGORY_MAX * ITEM_INDEX_MAX;
	size_t pos = 0;

	assert(_buf.size() == count * size + 4);

	for (size_t i = 0; i < count; i++)
	{
		Xor3Byte(&_buf[pos], size);
		pos += size;
	}

	*(DWORD*)&_buf[_buf.size() - 4] = CalculateCRC(&_buf[0], count * size, _wkey);
	return TRUE;
}

void ItemSetTypeBmd::TxtOut(std::ofstream & os)
{
	assert(os);

	os << "//ItemCat\tItemIndex\tTier1\tTier2\tTier3\tTier4\tUnk1\tUnk2" << std::endl;

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		ITEM_SET_TYPE* ptr = it->second;
		BYTE item_cat = it->first / 512;
		BYTE item_idx = it->first % 512;

		if (item_cat < ITEM_CATEGORY_MAX && item_idx < ITEM_INDEX_MAX)
		{
			os << (int)item_cat << '\t';
			os << (int)item_idx << '\t';
			os << (int)ptr->Tier1 << '\t';
			os << (int)ptr->Tier2 << '\t';
			os << (int)ptr->Tier3 << '\t';
			os << (int)ptr->Tier4 << '\t';
			os << (int)ptr->Unk1 << '\t';
			os << (int)ptr->Unk2 << '\t';
			os << std::endl;
		}
	}
}

void ItemSetTypeBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	std::string line;
	size_t size = sizeof(ITEM_SET_TYPE);
	size_t count = ITEM_CATEGORY_MAX * ITEM_INDEX_MAX;

	_buf.resize((count * size) + 4);
	memset(_buf.data(), 0x0, _buf.size());

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		ITEM_SET_TYPE temp;
		BYTE item_cat;
		BYTE item_idx;

		sscanf(line.c_str(),
			"%hhd\t%hhd\t%hhd\t%hhd\t%hhd\t%hhd\t%hhd\t%hhd"
			, &item_cat, &item_idx, &temp.Tier1, &temp.Tier2, &temp.Tier3, &temp.Tier4, &temp.Unk1, &temp.Unk2
		);

		if (item_cat < ITEM_CATEGORY_MAX && item_idx < ITEM_INDEX_MAX)
		{
			size_t pos = ITEM_MAKE(item_cat, item_idx) * size;
			memcpy(&_buf[pos], &temp, size);
		}
	}
}

//------------------------------------------------------------------------
//--ItemSetOptionBmd
//------------------------------------------------------------------------

BOOL ItemSetOptionBmd::Decrypt()
{
	assert(_buf.size());

	size_t size = sizeof(ITEM_SET_OPTION);
	size_t count = 255;
	size_t pos = 0;

	//assert(_buf.size() == count * size + 4);

	_map.clear();

	for (size_t i = 0; i < count; i++)
	{
		if (pos + size > _buf.size()) return TRUE; //ignore error

		Xor3Byte(&_buf[pos], size);
		ITEM_SET_OPTION* ptr = (ITEM_SET_OPTION*)&_buf[pos];

		int key = i;
		InsertToMap(key, ptr);

		pos += size;
	}

	return TRUE;
}

BOOL ItemSetOptionBmd::Encrypt()
{
	size_t size = sizeof(ITEM_SET_OPTION);
	size_t count = 255;
	size_t pos = 0;

	assert(_buf.size() == count * size + 4);

	for (int i = 0; i < count; i++)
	{
		Xor3Byte(&_buf[pos], size);
		pos += size;
	}

	*(DWORD*)&_buf[_buf.size() - 4] = CalculateCRC(&_buf[0], count * size, _wkey);
	return TRUE;
}

void ItemSetOptionBmd::TxtOut(std::ofstream & os)
{
	assert(os);

	static const std::string LABEL = "//ID\t" + ITEM_SET_OPTION::GetLabel();
	os << LABEL << std::endl;

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		ITEM_SET_OPTION* ptr = it->second;
		os << it->first << '\t';
		OffsetOut(os, ptr);
		os << std::endl;
	}
}

void ItemSetOptionBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	static const std::vector<OffsetInfo> OFFSET = ITEM_SET_OPTION::GetOffset();

	std::string line;
	size_t size = sizeof(ITEM_SET_OPTION);
	size_t count = 255;

	_buf.resize(size * count + 4);

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		line += '\t';
		size_t a = 0;
		size_t b = line.find('\t', a);
		if (b <= a)
		{
			//1st column (ID) must not be empty
			continue;
		}
		BYTE id;
		sscanf(line.substr(a, b - a).c_str(), "%hhd", &id);

		ITEM_SET_OPTION* ptr = (ITEM_SET_OPTION*)&_buf[id * size];

		size_t i = 0;
		do
		{
			if (b > a)
			{
				std::string s = line.substr(a, b - a);
				size_t pos = OFFSET[i].Offset + (size_t)ptr;
				sscanf(s.c_str(), OFFSET[i].Format.c_str(), (void*)pos);
			}

			i++;
			a = b + 1;
			b = line.find('\t', a);
		} while (b != std::string::npos && i < OFFSET.size());
	}
}
