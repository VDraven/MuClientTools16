#include "OBJ.h"

BOOL OBJ::Decrypt()
{
	MapFileDecrypt(_buf.data(), _buf.size());

	//BYTE Version = _buf[0];
	//BYTE Map = _buf[1];
	//WORD Count = *(WORD*)&_buf[2];
	//PRINT_DEBUG("Version : " << (int)Version << " , Map : " << (int)Map << " , Count : " << (int)Count);

	return TRUE;
}

BOOL OBJ::Encrypt()
{
	MapFileEncrypt(_buf.data(), _buf.size());
	return TRUE;
}
