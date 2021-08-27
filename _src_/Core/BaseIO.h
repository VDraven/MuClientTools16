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

public:
	static void SetOverwrite(BOOL b) { _overwrite = b; };
private:
	static BOOL _overwrite;
};

#endif
