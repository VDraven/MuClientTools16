#ifndef BASEIO_H
#define BASEIO_H

class BaseIO : public MuCrypto
{
public:
	BaseIO() {};
	virtual ~BaseIO() {};

protected:
	virtual BOOL FileOpen(const char *szSrcFile);
	virtual BOOL FileWrite(const char *szDestFile);

	std::vector<BYTE> _buf;
};

#endif
