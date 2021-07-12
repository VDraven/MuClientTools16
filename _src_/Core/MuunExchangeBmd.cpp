#include "MuunExchangeBmd.h"

BOOL MuunExchangeBmd::Decrypt()
{
	if(_buf.size() < 8) return FALSE;

	size_t size = sizeof(MUUN_EXCHANGE);
	size_t count1 = *(int*)&_buf[0];
	size_t count2 = *(int*)&_buf[4];
	size_t pos = 8;

	for (size_t i = 0; i < count1; i++)
	{
		if (pos + size > _buf.size()) return TRUE; //ignore error

		Xor3Byte(&_buf[pos], size);
		MUUN_EXCHANGE* ptr = (MUUN_EXCHANGE*)&_buf[pos];
		int key = (0 << 8) | ptr->Column_4;
		_map.insert(std::make_pair(key, ptr));
		pos += size;
	}

	for (size_t i = 0; i < count2; i++)
	{
		if (pos + size > _buf.size()) return TRUE; //ignore error

		Xor3Byte(&_buf[pos], size);
		MUUN_EXCHANGE* ptr = (MUUN_EXCHANGE*)&_buf[pos];
		int key = (1 << 8) | ptr->Column_1;
		_map.insert(std::make_pair(key, ptr));
		pos += size;
	}

	return TRUE;
}

BOOL MuunExchangeBmd::Encrypt()
{
	assert(_buf.size() >= 12);

	size_t size = sizeof(MUUN_EXCHANGE);
	for (size_t p = 8; p + size <= _buf.size() - 4; p += size)
	{
		Xor3Byte(&_buf[p], size);
	}
	DWORD CRC = CalculateCRC(&_buf[8], _buf.size() - 12, _wkey);
	*(DWORD*)&_buf[_buf.size() - 4] = CRC;

	return TRUE;
}

void MuunExchangeBmd::TxtOut(std::ofstream & os)
{
	assert(os);

	os << "//Group0: Required Item Exchange" << std::endl;
	os << "//Group\tItemID\tCount\tInventoryType\tExchangeType" << std::endl;

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		if ((it->first >> 8) == 0)
		{
			MUUN_EXCHANGE* ptr = it->second;
			os << "0" << '\t';
			os << ptr->Column_1 << '\t';
			os << ptr->Column_2 << '\t';
			os << ptr->Column_3 << '\t';
			os << ptr->Column_4 << '\t';
			os << std::endl;
		}
	}

	os << "//Group1: Exchange Type" << std::endl;
	os << "//Group\tExchangeType\tEvoStoneID\tMuunEvoID\tInventoryType" << std::endl;

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		if ((it->first >> 8) == 1)
		{
			MUUN_EXCHANGE* ptr = it->second;
			os << "1" << '\t';
			os << ptr->Column_1 << '\t';
			os << ptr->Column_2 << '\t';
			os << ptr->Column_3 << '\t';
			os << ptr->Column_4 << '\t';
			os << std::endl;
		}
	}
}

void MuunExchangeBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	std::string line;
	int size = sizeof(MUUN_EXCHANGE);
	int n = 0;

	std::map<int, std::vector<MUUN_EXCHANGE>> temp;

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;
		MUUN_EXCHANGE mex;
		int group;
		sscanf(line.c_str(),
			"%d\t%d\t%d\t%d\t%d"
			, &group, &mex.Column_1, &mex.Column_2, &mex.Column_3, &mex.Column_4
		);
		temp[group].push_back(mex);
		n++;
	}

	_buf.resize(8 + (n * size) + 4);

	*(DWORD*)&_buf[0] = temp[0].size();
	*(DWORD*)&_buf[4] = temp[1].size();

	int pos = 8;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < temp[i].size(); j++)
		{
			MUUN_EXCHANGE* dst = (MUUN_EXCHANGE*)&_buf[pos];
			MUUN_EXCHANGE* src = &temp[i][j];
			memcpy(dst, src, size);
			pos += size;
		}
	}
}
