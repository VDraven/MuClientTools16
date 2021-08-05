#include "ItemToolTipBmd.h"

BOOL ItemToolTipBmd::Decrypt()
{
	assert(_buf.size());

	size_t size = sizeof(ITEM_TOOLTIP);
	size_t count = ITEM_CATEGORY_MAX * ITEM_INDEX_MAX;
	size_t pos = 0;

	assert(_buf.size() == count * size + 4);

	_map.clear();

	for (size_t i = 0; i < count; i++)
	{
		if (pos + size > _buf.size()) return TRUE; //ignore error

		Xor3Byte(&_buf[pos], size);
		ITEM_TOOLTIP* ptr = (ITEM_TOOLTIP*)&_buf[pos];

		if (ptr->Category != 0 || ptr->Index != 0 || Utls::IsEmptyCStr(ptr->Name) == false)
		{
			int key = (ptr->Category * 512) + ptr->Index;
			InsertToMap(key, ptr);
		}

		pos += size;
	}

	return TRUE;
}

BOOL ItemToolTipBmd::Encrypt()
{
	size_t size = sizeof(ITEM_TOOLTIP);
	size_t count = ITEM_CATEGORY_MAX * ITEM_INDEX_MAX;
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

void ItemToolTipBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	static const std::vector<OffsetInfo> OFFSET = ITEM_TOOLTIP::GetOffset();

	std::string line;
	size_t size = sizeof(ITEM_TOOLTIP);
	size_t count = ITEM_CATEGORY_MAX * ITEM_INDEX_MAX;
	size_t row_id = 0;

	_buf.resize(size * count + 4);
	memset(&_buf[0], 0, _buf.size());

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		line += '\t';
		size_t a = 0;
		size_t b = line.find('\t', a);

		ITEM_TOOLTIP temp;
		memset(&temp, 0, sizeof(temp));

		size_t i = 0;
		do
		{
			if (b > a)
			{
				std::string s = line.substr(a, b - a);
				size_t pos = OFFSET[i].Offset + (size_t)&temp;
				sscanf(s.c_str(), OFFSET[i].Format.c_str(), (void*)pos);
			}

			i++;
			a = b + 1;
			b = line.find('\t', a);
		} while (b != std::string::npos && i < OFFSET.size());

		if (temp.Category != 0 || temp.Index != 0 || Utls::IsEmptyCStr(temp.Name) == false)
		{
			memcpy(&_buf[size * (row_id++)], &temp, size);
		}
	}
}

//=========================================================

BOOL ItemToolTipTextBmd::Decrypt()
{
	assert(_buf.size());

	size_t size = sizeof(ITEM_TOOLTIP_TEXT);

	_map.clear();

	int err = -1;
	for (size_t p = (hasCounter * 4); p + size <= _buf.size() - (hasCRC * 4); p += size)
	{
		//Xor3Byte(&_buf[p], size);
		Xor3Byte2(&_buf[p], size, _wkey);

		ITEM_TOOLTIP_TEXT *ptr = (ITEM_TOOLTIP_TEXT *)&_buf[p];
		int key = GetKey(ptr);
		InsertToMap(key, ptr, &err);
	}

	return TRUE;
}

BOOL ItemToolTipTextBmd::Encrypt()
{
	assert(_buf.size());

	size_t size = sizeof(ITEM_TOOLTIP_TEXT);
	for (size_t p = (hasCounter * 4); p + size <= _buf.size() - (hasCRC * 4); p += size)
	{
		//Xor3Byte(&_buf[p], size);
		Xor3Byte2(&_buf[p], size, _wkey);
	}

	if (hasCRC)
	{
		DWORD CRC = CalculateCRC(&_buf[4], _buf.size() - 8, _wkey);
		*(DWORD*)&_buf[_buf.size() - 4] = CRC;
	}

	return TRUE;
}
