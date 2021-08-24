#include "MAP.h"

BOOL MAP::Decrypt()
{
	if (_buf.size() > 4 && _buf[0] == 'M' && _buf[1] == 'A' && _buf[2] == 'P' && _buf[3] == 1)
	{
		_buf.erase(_buf.begin(), _buf.begin() + 4);
		return ModulusDecrypt(_buf);
	}

	//else
	MapFileDecrypt(_buf.data(), _buf.size());
	return TRUE;
}

BOOL MAP::Encrypt()
{
	MapFileEncrypt(_buf.data(), _buf.size());
	return TRUE;

	//or ModulusEncrypt
	//ModulusEncrypt(_buf);
	//BYTE header[4] = { 'M' , 'A', 'P', 1 };
	//_buf.insert(_buf.begin(), std::begin(header), std::end(header));
	//return TRUE;
};

