#include "PetBmd.h"

BOOL PetBmd::Decrypt()
{
	assert(_buf.size() > 12);

	size_t size = sizeof(PET_BMD);
	size_t pos = 0;

	_map.clear();

	size_t n_1 = *(size_t*)&_buf[pos];
	pos += 4;

	size_t n_2 = *(size_t*)&_buf[pos];
	pos += 4;

	size_t n_3 = *(size_t*)&_buf[pos];
	pos += 4;

	size_t check_size = n_2 * 8 + 16;
	assert(size == check_size);
	assert(_buf.size() == n_3 * check_size + 16);

	for (size_t i = 0; i < n_3; i++)
	{
		Xor3Byte(&_buf[pos], size);
		PET_BMD* ptr = (PET_BMD*)&_buf[pos];

		int key = GetKey(ptr);
		InsertToMap(key, ptr);
		
		pos += size;
	}

	return TRUE;
}

BOOL PetBmd::Encrypt()
{
	size_t size = sizeof(PET_BMD);
	size_t n_1 = 100;
	size_t n_2 = 50;
	size_t n_3;
	size_t pos = 0;

	_buf.insert(_buf.begin(), 8, 0);	//insert 2 integers at begin()

	*(size_t*)&_buf[pos] = n_1;
	pos += 4;
	*(size_t*)&_buf[pos] = n_2;
	pos += 4;
	n_3 = *(size_t*)&_buf[pos];
	pos += 4;


	for (int i = 0; i < n_3; i++)
	{
		Xor3Byte(&_buf[pos], size);
		pos += size;
	}

	*(DWORD*)&_buf[_buf.size() - 4] = CalculateCRC(&_buf[12], n_3 * size, _wkey);
	return TRUE;
}
