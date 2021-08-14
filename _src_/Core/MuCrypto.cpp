#include "MuCrypto.h"

#include "cryptopp/tea.h"
#include "cryptopp/3way.h"
#include "cryptopp/cast.h"
#include "cryptopp/rc5.h"
#include "cryptopp/rc6.h"
#include "cryptopp/mars.h"
#include "cryptopp/idea.h"
#include "cryptopp/gost.h"

DWORD MuCrypto::CalculateCRC(BYTE * buf, size_t len, WORD wkey)
{
	assert(buf);

	DWORD CRC = wkey << 9;
	for (size_t i = 0; i <= len - 4; i += 4)
	{
		DWORD temp = *(DWORD*)&buf[i];
		//memcpy(&temp, &buf[i], 4);
		if ((wkey + (i >> 2)) % 2 == 1)
			CRC += temp;
		else
			CRC ^= temp;
		if (i % 16 == 0)
			CRC ^= (CRC + wkey) >> ((i >> 2) % 8 + 1);
	}
	return CRC;
}

void MuCrypto::Xor3Byte(BYTE * buf, size_t len)
{
	constexpr static BYTE _xor3key[] = { _MU_XOR3_KEY_ };

	for (size_t i = 0; i < len; ++i)
	{
		buf[i] ^= _xor3key[i % 3];
	}
}

void MuCrypto::Xor3Byte2(BYTE * buf, size_t len, WORD wkey)
{
	constexpr static BYTE _xor3key[] = { _MU_XOR3_KEY_ };

	for (size_t i = 0; i < len; ++i)
	{
		buf[i] ^= _xor3key[i % 3];
		buf[i] ^= wkey & 0xFF;
	}
}

void MuCrypto::MapFileEncrypt(BYTE * buf, size_t len)
{
	constexpr static BYTE key[]{ _MU_MAP_FILE_KEY_ };
	BYTE seed = 0x5E;

	for (size_t i = 0; i < len; i++)
	{
		buf[i] += seed;
		buf[i] ^= key[i % 16];
		seed = buf[i] + 0x3D;
	}
}

void MuCrypto::MapFileDecrypt(BYTE * buf, size_t len)
{
	constexpr static BYTE key[] { _MU_MAP_FILE_KEY_ };
	BYTE seed = 0x5E;

	for (size_t i = 0; i < len; i++)
	{
		BYTE temp = buf[i];
		buf[i] ^= key[i % 16];
		buf[i] -= seed;
		seed = temp + 0x3D;
	}
}

BOOL MuCrypto::InitModulusCrypto(DWORD algorithm, BYTE * key, size_t keyLength)
{
	typedef ConcreteCipher < CryptoPP::TEA, 1024 * 8 >			Cipher0;
	typedef ConcreteCipher < CryptoPP::ThreeWay, 1024 * 8 >		Cipher1;
	typedef ConcreteCipher < CryptoPP::CAST128, 1024 * 8 >		Cipher2;
	typedef ConcreteCipher < CryptoPP::RC5, 1024 * 8 >			Cipher3;
	typedef ConcreteCipher < CryptoPP::RC6, 1024 * 8 >			Cipher4;
	typedef ConcreteCipher < CryptoPP::MARS, 1024 * 8 >			Cipher5;
	typedef ConcreteCipher < CryptoPP::IDEA, 1024 * 8 >			Cipher6;
	typedef ConcreteCipher < CryptoPP::GOST, 1024 * 8 >			Cipher7;

	//FIX for multi threads (parallel algorithms C++17)
	if (this->m_cipher) delete this->m_cipher;

	switch (algorithm & 7)	// algorithm % 8
	{
	case 0:
		this->m_cipher = new Cipher0();
		break;
	case 1:
		this->m_cipher = new Cipher1();
		break;
	case 2:
		this->m_cipher = new Cipher2();
		break;
	case 3:
		this->m_cipher = new Cipher3();
		break;
	case 4:
		this->m_cipher = new Cipher4();
		break;
	case 5:
		this->m_cipher = new Cipher5();
		break;
	case 6:
		this->m_cipher = new Cipher6();
		break;
	case 7:
		this->m_cipher = new Cipher7();
		break;
	default:	//should never reach here
		this->m_cipher = new Cipher7();
		break;
	}

	return this->m_cipher->Init(key, keyLength);

/*
	switch (algorithm & 7)	// algorithm % 8
	{
	case 0:
		this->m_cipher = sInstance(Cipher0);
		break;
	case 1:
		this->m_cipher = sInstance(Cipher1);
		break;
	case 2:
		this->m_cipher = sInstance(Cipher2);
		break;
	case 3:
		this->m_cipher = sInstance(Cipher3);
		break;
	case 4:
		this->m_cipher = sInstance(Cipher4);
		break;
	case 5:
		this->m_cipher = sInstance(Cipher5);
		break;
	case 6:
		this->m_cipher = sInstance(Cipher6);
		break;
	case 7:
		this->m_cipher = sInstance(Cipher7);
		break;
	default:	//should never reach here
		this->m_cipher = sInstance(Cipher7);
		break;
	}

	return this->m_cipher->Init(key, keyLength);
*/
}

int MuCrypto::BlockEncrypt(BYTE *inBuf, size_t len, BYTE *outBuf)
{
	if (this->m_cipher)
		return this->m_cipher->Encrypt(inBuf, len, outBuf);

	return -1;
}

int MuCrypto::BlockDecrypt(BYTE *inBuf, size_t len, BYTE *outBuf)
{
	if (this->m_cipher)
		return this->m_cipher->Decrypt(inBuf, len, outBuf);

	return -1;
}

BOOL MuCrypto::ModulusEncrypt(std::vector<BYTE>& buf)
{
	if (!buf.size()) return FALSE;

	BYTE key_1[33] = _MU_MODULUS_KEY_;
	BYTE key_2[33] = _MU_MODULUS_KEY_;	//able to pick any random key_2
	DWORD algorithm_1 = std::rand() % 8;
	DWORD algorithm_2 = std::rand() % 8;

	size_t data_size = buf.size();
	size_t size = data_size + 34;

	buf.insert(buf.begin(), std::begin(key_2), std::begin(key_2) + 32);
	buf.insert(buf.begin(), algorithm_1);
	buf.insert(buf.begin(), algorithm_2);

	InitModulusCrypto(algorithm_2, key_2, 32);
	size_t block_size = data_size - (data_size % GetBlockSize());

	BYTE * block = &buf[34];
	BlockEncrypt(block, block_size, block);

	InitModulusCrypto(algorithm_1, key_1, 32);
	block_size = 1024 - (1024 % GetBlockSize());

	if (data_size > block_size)
	{
		block = &buf[2];
		BlockEncrypt(block, block_size, block);
		block = &buf[size - block_size];
		BlockEncrypt(block, block_size, block);
	}

	if (data_size > (4 * block_size))
	{
		block = &buf[2 + (data_size >> 1)];
		BlockEncrypt(block, block_size, block);
	}

	return TRUE;
}

BOOL MuCrypto::ModulusDecrypt(std::vector<BYTE>& buf)
{
	if (buf.size() < 34) return FALSE;

	BYTE key_1[33] = _MU_MODULUS_KEY_;
	BYTE key_2[33] = { 0 };
	DWORD algorithm_1 = buf[1];
	DWORD algorithm_2 = buf[0];

	size_t size = buf.size();
	size_t data_size = size - 34;

	InitModulusCrypto(algorithm_1, key_1, 32);
	size_t block_size = 1024 - (1024 % GetBlockSize());

	BYTE * block;

	if (data_size > (4 * block_size))
	{
		block = &buf[2 + (data_size >> 1)];
		BlockDecrypt(block, block_size, block);
	}

	if (data_size > block_size)
	{
		block = &buf[size - block_size];
		BlockDecrypt(block, block_size, block);
		block = &buf[2];
		BlockDecrypt(block, block_size, block);
	}

	memcpy(key_2, &buf[2], 32);

	InitModulusCrypto(algorithm_2, key_2, 32);
	block_size = data_size - (data_size % GetBlockSize());

	block = &buf[34];
	BlockDecrypt(block, block_size, block);

	buf.erase(buf.begin(), buf.begin() + 34);

	return TRUE;
}



