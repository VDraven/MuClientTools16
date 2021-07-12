
#ifndef LANG_MPR_H
#define LANG_MPR_H

#define _LANG_WKEY_ 0x12DC
#define _LANG_ZIP_PASSWORD_ L"q!h#P<xALqU*sxC@{eHr|N)$:"
// Main Season 16 1.19.46 offset lang.mpr password : 0x0163BEBC

class LangMpr : public BaseIO
{
public:
	LangMpr() : _wkey(_LANG_WKEY_) {};
	virtual ~LangMpr() {};

	BOOL Unpack(const char *szSrcFile, const char *szDestDir);
	BOOL Pack(const char *szSrcDir, const char *szDestFile);

private:
	BOOL FileOpen(const char *szSrcFile);
	BOOL FileWrite(const char *szDestFile);
	BOOL Decrypt();
	BOOL Encrypt();
	BOOL UnZip(const char *szDestDir);
	BOOL Zip(const char *szSrcDir);

	WORD _wkey;
};

#endif // !LANG_MPR_H