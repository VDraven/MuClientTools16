#include "Core.h"
#include "SocketItemBmd.h"

BOOL SocketItemBmd::Decrypt()
{
	assert(_buf.size() > 0);

	size_t size = sizeof(SOCKET_ITEM_INFO);
	size_t kind = 3;
	size_t count = 50;
	size_t pos = 0;

	_map.clear();

	for (size_t i = 0; i < kind; i++)
	{
		for (size_t j = 0; j < count; j++)
		{
			Xor3Byte(&_buf[pos], size);
			SOCKET_ITEM_INFO* ptr = (SOCKET_ITEM_INFO*)&_buf[pos];

			int key = (i << 8) | j;
			_map.insert(std::make_pair(key, ptr));

			pos += size;
		}
	}

	return TRUE;
}

BOOL SocketItemBmd::Encrypt()
{
	assert(_buf.size() > 0);

	size_t size = sizeof(SOCKET_ITEM_INFO);

	for (size_t p = 0; p + size <= _buf.size(); p += size)
		Xor3Byte(&_buf[p], size);

	return TRUE;
}

void SocketItemBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	static const std::vector<OffsetInfo> OFFSET = SOCKET_ITEM_INFO::GetOffset();

	std::string line;
	size_t size = sizeof(SOCKET_ITEM_INFO);
	size_t kind = 3;
	size_t count = 50;
	size_t n = 0;

	_buf.resize(size * kind * count);

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		SOCKET_ITEM_INFO* ptr = (SOCKET_ITEM_INFO*)&_buf[n * size];
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
				sscanf(s.c_str(), OFFSET[i].Format.c_str(), (void*)pos);
			}

			i++;
			a = b + 1;
			b = line.find('\t', a);
		} while (b != std::string::npos && i < OFFSET.size());

		n++;
	}
}
