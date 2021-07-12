#include "EvoMonsterBoxBmd.h"

BOOL EvoMonsterBoxBmd::Decrypt()
{
	if (_buf.size() != 164)
	{
		std::cout << "Error: File size must equal 164." << std::endl;
		//return FALSE; //ignore

		while (_buf.size() < 164)
			_buf.push_back(0);
		if (_buf.size() > 164)
			_buf.resize(164);
	}
	size_t size = 156;

	_map.clear();

	Xor3Byte(&_buf[4], size);
	for (size_t i = 0; i < size; i++)
	{
		EVO_MONSTER_BOX* ptr = (EVO_MONSTER_BOX*)&_buf[4 + i];
		int key = GetKey(ptr);
		_map.insert(std::make_pair(key, ptr));
	}

	return TRUE;
}

BOOL EvoMonsterBoxBmd::Encrypt()
{
	while(_buf.size() < 164)
		_buf.push_back(0);
	if(_buf.size() > 164)
		_buf.resize(164);

	size_t size = 156;

	*(DWORD*)&_buf[0] = size;
	Xor3Byte(&_buf[4], size);
	*(DWORD*)&_buf[_buf.size()-4] = CalculateCRC(&_buf[4], size, _wkey);

	return TRUE;
}

void EvoMonsterBoxBmd::MakeLabelEx(std::ofstream & os)
{
	assert(os);
	os << "//This table relates to some chance of EvoMonster Reward." << std::endl;
	os << "//(the Drawing Cards Frame when using the box from killing golden evo monster)" << std::endl;
	os << "//I didn't check what it's used for. What if we set it all to 1 ???" << std::endl;
	os << "//www.youtube.com/watch?v=cvtJXclmj6E" << std::endl;

}
