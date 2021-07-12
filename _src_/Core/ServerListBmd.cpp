#include "ServerListBmd.h"

BOOL ServerListBmd::Decrypt()
{
	assert(_buf.size() > 0);

	size_t size = sizeof(SERVER_LIST);
	size_t pos = 0;

	_map.clear();

	while(pos < _buf.size())
	{
		Xor3Byte(&_buf[pos], size);
			
		SERVER_LIST * ptr = (SERVER_LIST*)&_buf[pos];
		int len = ptr->MsgLen;

		pos += size;
		Xor3Byte(&_buf[pos], len);

		int key = GetKey(ptr);
		_map.insert(std::make_pair(key, ptr));

		pos += len;
	}

	return TRUE;
}

BOOL ServerListBmd::Encrypt()
{
	assert(_buf.size() > 0);

	size_t size = sizeof(SERVER_LIST);
	size_t pos = 0;

	while (pos < _buf.size())
	{
		SERVER_LIST * ptr = (SERVER_LIST*)&_buf[pos];
		int len = ptr->MsgLen;

		Xor3Byte(&_buf[pos], size);
		pos += size;
		Xor3Byte(&_buf[pos], len);
		pos += len;
	}

	return TRUE;
}

void ServerListBmd::TxtOut(std::ofstream & os)
{
	assert(os);

	os << "//ID\tName\tType\tIndex\tPvp\tMsg" << std::endl;

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		SERVER_LIST* ptr = it->second;

		os << ptr->ID << '\t';
		os << ptr->Name << '\t';
		os << (DWORD)ptr->PosType << '\t';
		os << (DWORD)ptr->Index << '\t';
		os << (DWORD)ptr->Type << '\t';
		os << ((SERVER_LIST_EX*)ptr)->Msg << std::endl;
	}
}

void ServerListBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	std::string line;
	size_t size = sizeof(SERVER_LIST);
	size_t pos = 0;

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		_buf.resize(pos + size);
		SERVER_LIST* ptr = (SERVER_LIST*)&_buf[pos];
		char msg[1000];
		sscanf(line.c_str(),
			"%hd\t%[^\t]%*c\t%hhd\t%hhd\t%hhd\t%[^\t]%*c"
			, &ptr->ID, &ptr->Name, &ptr->PosType, &ptr->Index, &ptr->Type, msg
		);

		ptr->MsgLen = strlen(msg) + 1;
		pos = _buf.size();
		_buf.resize(pos + ptr->MsgLen);
		memcpy(&_buf[pos], msg, ptr->MsgLen);
		pos = _buf.size();
	}
}
