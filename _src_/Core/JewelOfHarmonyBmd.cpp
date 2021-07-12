#include "JewelOfHarmonyBmd.h"

BOOL JewelOfHarmonyOptionBmd::Decrypt()
{
	size_t count = 750;
	size_t size = sizeof(JEWEL_OF_HARMONY_OPTION);

	if (_buf.size() != size * count)
	{
		std::cout << "Error: File size != (ITEM_CATEGORY_MAX * ITEM_INDEX_MAX * 16)" << std::endl;
		return false;
	}

	_map.clear();

	Xor3Byte(&_buf[0], size * count);
	for (size_t i = 0; i < count; i++)
	{
		JEWEL_OF_HARMONY_OPTION *ptr = (JEWEL_OF_HARMONY_OPTION *)&_buf[i * size];
		if (ptr->ID >= 0)
		{
			int key = i;
			InsertToMap(key, ptr);
		}
	}

	return TRUE;
}

void JewelOfHarmonyOptionBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	static const std::vector<OffsetInfo> OFFSET = JEWEL_OF_HARMONY_OPTION::GetOffset();

	std::string line;
	size_t count = 750;
	size_t size = sizeof(JEWEL_OF_HARMONY_OPTION);
	size_t n = 0;

	_buf.resize(count * size);
	memset(_buf.data(), 0xFF, _buf.size());
	for (size_t i = 0; i < count; i++)
	{
		JEWEL_OF_HARMONY_OPTION* ptr = (JEWEL_OF_HARMONY_OPTION*)&_buf[i * size];
		memset(ptr->Name, 0x00, sizeof(ptr->Name));
	}

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		JEWEL_OF_HARMONY_OPTION* ptr = (JEWEL_OF_HARMONY_OPTION*)&_buf[n * size];
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
					size_t len = sizeof(JEWEL_OF_HARMONY_OPTION);
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
}
