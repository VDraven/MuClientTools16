#pragma once
#ifndef CAMERA_ANGLE_POSITION_BMD_H
#define CAMERA_ANGLE_POSITION_BMD_H

class CameraAnglePositionBmd : public BaseIO
{
public:
	CameraAnglePositionBmd() {};
	virtual ~CameraAnglePositionBmd() {};

	
	BOOL Unpack(const char *szSrcBmd, const char *szDestIni)
	{
		return FileOpen(szSrcBmd)
			&& Decrypt()
			&& FileWrite(szDestIni ? szDestIni : fs::path(szSrcBmd).replace_extension(".ini").string().c_str());
	};

	BOOL Pack(const char * szSrcIni, const char * szDestBmd)
	{
		return FileOpen(szSrcIni)
			&& Encrypt()
			&& FileWrite(szDestBmd ? szDestBmd : fs::path(szSrcIni).replace_extension(".out.bmd").string().c_str());
	};

private:

	bool Decrypt()
	{
		assert(_buf.size() > 4);

		if (_buf[0] == 'M' && _buf[1] == 'A' && _buf[2] == 'P' && _buf[3] == 0x01)
		{
			// NEW CAP FILE
			_buf.erase(_buf.begin(), _buf.begin() + 4);
			ModulusDecrypt(_buf);
		}
		else
		{
			// OLD CAP FILE
			MapFileDecrypt(_buf.data(), _buf.size());
		}

		return true;
	};

	bool Encrypt()
	{
		assert(_buf.size() > 0);

		// NEW CAP FILE
		{
			ModulusEncrypt(_buf);
			_buf.insert(_buf.begin(), 4, 0x00);
			_buf[0] = 'M'; _buf[1] = 'A'; _buf[2] = 'P'; _buf[3] = 0x01;
		}
		
		// OLD CAP FILE
		//{
		//	MapFileDecrypt(_buf.data(), _buf.size());
		//}

		return true;
	};
};

#endif