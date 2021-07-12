#include "Core.h"
#include "ItemAddOptionBmd.h"

BOOL ItemAddOptionBmd::Decrypt()
{
	size_t count = ITEM_CATEGORY_MAX * ITEM_INDEX_MAX;
	size_t size = sizeof(ITEM_ADD_OPTION);

	if (_buf.size() != size * count)
	{
		std::cout << "Error: File size != (ITEM_CATEGORY_MAX * ITEM_INDEX_MAX * 16)" << std::endl;
		return false;
	}

	_map.clear();

	Xor3Byte(&_buf[0], size * count);
	for (size_t i = 0; i < count; i++)
	{
		ITEM_ADD_OPTION *ptr = (ITEM_ADD_OPTION *)&_buf[i * size];
		if (ptr->Option_1 + ptr->Value_1 + ptr->Option_2 + ptr->Value_2 != 0)
		{
			int key = i;
			InsertToMap(key, ptr);
		}
	}

	return TRUE;
}

BOOL ItemAddOptionBmd::Encrypt()
{
	size_t count = ITEM_CATEGORY_MAX * ITEM_INDEX_MAX;
	size_t size = sizeof(ITEM_ADD_OPTION);

	assert(_buf.size() == count * size);

	Xor3Byte(&_buf[0], count * size);

	return TRUE;
}

void ItemAddOptionBmd::TxtOut(std::ofstream & os)
{
	assert(os);

	os << "//ItemCat\tItemIndex\tOption_1\tValue_1\tOption_2\tValue_2\tTime" << std::endl;

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		ITEM_ADD_OPTION* ptr = it->second;
		BYTE item_cat = it->first / 512;
		BYTE item_idx = it->first % 512;

		if (item_cat < ITEM_CATEGORY_MAX && item_idx < ITEM_INDEX_MAX)
		{
			os << (int)item_cat << '\t';
			os << (int)item_idx << '\t';
			os << ptr->Option_1 << '\t';
			os << ptr->Value_1 << '\t';
			os << ptr->Option_2 << '\t';
			os << ptr->Value_2 << '\t';
			os << ptr->Time << '\t';

			os << std::endl;
		}
	}
}

void ItemAddOptionBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	std::string line;
	size_t size = sizeof(ITEM_ADD_OPTION);
	size_t count = ITEM_CATEGORY_MAX * ITEM_INDEX_MAX;

	_buf.resize(count * size);
	memset(_buf.data(), 0x0, _buf.size());

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		ITEM_ADD_OPTION temp;
		BYTE item_cat;
		BYTE item_idx;

		sscanf(line.c_str(),
			"%hhd\t%hhd\t%hd\t%hd\t%hd\t%hd\t%d"
			, &item_cat, &item_idx, &temp.Option_1, &temp.Value_1, &temp.Option_2, &temp.Value_2, &temp.Time
		);
		temp.Idx = item_idx;

		if (item_cat < ITEM_CATEGORY_MAX && item_idx < ITEM_INDEX_MAX)
		{
			size_t pos = ITEM_MAKE(item_cat, item_idx) * size;
			memcpy(&_buf[pos], &temp, size);
		}
	}
}
