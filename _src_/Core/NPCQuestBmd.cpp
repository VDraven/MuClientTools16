#include "NPCQuestBmd.h"

BOOL QuestWordsBmd::Decrypt()
{
	assert(_buf.size() > 0);

	size_t size = sizeof(QUEST_WORDS);
	size_t pos = 0;

	_map.clear();

	while (pos + size <= _buf.size())
	{
		Xor3Byte(&_buf[pos], size);

		QUEST_WORDS * ptr = (QUEST_WORDS*)&_buf[pos];
		int len = ptr->Len;

		pos += size;
		Xor3Byte(&_buf[pos], len);

		int key = GetKey(ptr);
		_map.insert(std::make_pair(key, ptr));

		pos += len;
	}

	return TRUE;
}

BOOL QuestWordsBmd::Encrypt()
{
	assert(_buf.size() > 0);

	size_t size = sizeof(QUEST_WORDS);
	size_t pos = 0;

	while (pos < _buf.size())
	{
		QUEST_WORDS * ptr = (QUEST_WORDS*)&_buf[pos];
		int len = ptr->Len;

		Xor3Byte(&_buf[pos], size);
		pos += size;
		Xor3Byte(&_buf[pos], len);
		pos += len;
	}

	return TRUE;
}

void QuestWordsBmd::TxtOut(std::ofstream & os)
{
	assert(os);

	os << "//ID\tText" << std::endl;

	for (auto it = _map.begin(); it != _map.end(); it++)
	{
		QUEST_WORDS* ptr = it->second;

		os << ptr->ID << '\t';
		os << ((QUEST_WORDS_EX*)ptr)->Text << std::endl;
	}
}

void QuestWordsBmd::TxtIn(std::ifstream & is)
{
	assert(is);

	std::string line;
	size_t size = sizeof(QUEST_WORDS);
	size_t pos = 0;

	while (getline(is, line))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;

		_buf.resize(pos + size);
		QUEST_WORDS* ptr = (QUEST_WORDS*)&_buf[pos];
		char msg[1000];
		sscanf(line.c_str(),
			"%d\t%[^\t]%*c"
			, &ptr->ID, msg
		);

		ptr->Len = strlen(msg) + 1;
		pos = _buf.size();
		_buf.resize(pos + ptr->Len);
		memcpy(&_buf[pos], msg, ptr->Len);
		pos = _buf.size();
	}
}
