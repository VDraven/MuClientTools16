#include "ATT.h"

BOOL ATT::Decrypt()
{
	bool dec = false;
	if (_buf.size() > 4 && _buf[0] == 'A' && _buf[1] == 'T' && _buf[2] == 'T' && _buf[3] == 1)
	{
		_buf.erase(_buf.begin(), _buf.begin() + 4);
		dec = ModulusDecrypt(_buf);
	}
	else
	{
		MapFileDecrypt(_buf.data(), _buf.size());
		dec = true;
	}
	if (!dec) return FALSE;

	Xor3Byte(_buf.data(), _buf.size());

	if (_buf.size() != 0x10004 && _buf.size() != 0x20004) return FALSE;

	BYTE Version = _buf[0];
	BYTE Width = _buf[2];
	BYTE Height = _buf[3];
	if (Version != 0 || Width != 255 || Height != 255) return FALSE;

	return TRUE;
};

BOOL ATT::Encrypt()
{
	if (_buf.size() != 0x10004 && _buf.size() != 0x20004) return FALSE;

	BYTE Version = _buf[0];
	BYTE Width = _buf[2];
	BYTE Height = _buf[3];
	if (Version != 0 || Width != 255 || Height != 255) return FALSE;

	Xor3Byte(_buf.data(), _buf.size());
	MapFileEncrypt(_buf.data(), _buf.size());
	return TRUE;

	//or ModulusEncrypt
	//Xor3Byte(_buf.data(), _buf.size());
	//ModulusEncrypt(_buf);
	//BYTE header[4] = { 'A' , 'T', 'T', 1 };
	//_buf.insert(_buf.begin(), std::begin(header), std::end(header));
	//return TRUE;
};