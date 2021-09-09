#ifndef _MU_CRYPTO_H
#define _MU_CRYPTO_H

#define _MU_XOR3_KEY_  0xFC, 0xCF, 0xAB
#define _MU_MAP_FILE_KEY_  0xD1, 0x73, 0x52, 0xF6, 0xD2, 0x9A, 0xCB, 0x27, 0x3E, 0xAF, 0x59, 0x31, 0x37, 0xB3, 0xE7, 0xA2
#define _MU_MODULUS_KEY_ "webzen#@!01webzen#@!01webzen#@!0"

class AbstractCipher
{
	friend class MuCrypto;
public:
	AbstractCipher() {};
	virtual ~AbstractCipher() {};

protected:
	virtual bool Init(BYTE *key, DWORD length) = 0;
	virtual int Encrypt(BYTE *inBuf, size_t len, BYTE *outBuf) = 0;
	virtual int Decrypt(BYTE *inBuf, size_t len, BYTE *outBuf) = 0;

	int m_maxRunCount;
	int m_blockSize;
};


template <class CRYPTO, int MAX_RUN> 
class ConcreteCipher : public AbstractCipher
{
public:
	ConcreteCipher() {};
	virtual ~ConcreteCipher() {};

	bool Init(BYTE *key, DWORD length)
	{
		enc_.SetKey(key, enc_.DEFAULT_KEYLENGTH);
		dec_.SetKey(key, dec_.DEFAULT_KEYLENGTH);
		m_maxRunCount = MAX_RUN;
		m_blockSize = enc_.BLOCKSIZE;
		return true;

	}

	int Encrypt(BYTE *inBuf, size_t len, BYTE *outBuf)
	{
		if (inBuf && outBuf && len)
		{
			for (int i = 0; i < len; i += dec_.BLOCKSIZE)
			{
				enc_.ProcessBlock(&inBuf[i], &outBuf[i]);
			}
			return len;
		}

		return -1;
	}

	int Decrypt(BYTE *inBuf, size_t len, BYTE *outBuf)
	{
		if (inBuf && outBuf && len)
		{
			for (int i = 0; i < len; i += dec_.BLOCKSIZE)
			{
				dec_.ProcessBlock(&inBuf[i], &outBuf[i]);
			}
			return len;
		}

		return -1;
	}

	typename CRYPTO::Encryption enc_;
	typename CRYPTO::Decryption dec_;
};

class MuCrypto
{
public:
	MuCrypto() : m_cipher(nullptr) {};
	virtual ~MuCrypto() {};

	DWORD CalculateCRC(BYTE *buf, size_t len, WORD wkey);
	void Xor3Byte(BYTE *buf, size_t len);
	void Xor3Byte2(BYTE *buf, size_t len, WORD wkey);

	void MapFileEncrypt(BYTE* buf, size_t len);
	void MapFileDecrypt(BYTE* buf, size_t len);

	BOOL ModulusEncrypt(std::vector<BYTE>& buf);
	BOOL ModulusDecrypt(std::vector<BYTE>& buf);

private:
	BOOL InitModulusCrypto(DWORD algorithm, BYTE * key, size_t keyLength);
	int BlockEncrypt(BYTE *inBuf, size_t len, BYTE *outBuf);
	int BlockDecrypt(BYTE *inBuf, size_t len, BYTE *outBuf);
	int GetBlockSize() { return m_cipher ? m_cipher->m_blockSize : 0; }

private:
	AbstractCipher * m_cipher;
};

#endif