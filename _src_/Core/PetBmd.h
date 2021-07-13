#pragma once

//=======================================================================

#ifndef PET_BMD_H
#define PET_BMD_H

#define _PET_BMD_WKEY_ 0x7F1D

typedef TxtConvertor<PET_BMD> PetBmdConvert;
class PetBmd : public PetBmdConvert
{
public:
	PetBmd() : PetBmdConvert(_PET_BMD_WKEY_) { };
	virtual ~PetBmd() {};
	
	/*
	BOOL Unpack(const char *szSrcBmd, const char *szDestTxt)
	{
		sizeof(PET_DATA);
		return FileOpen(szSrcBmd)
			&& Decrypt()
			&& FileWrite(fs::path(szSrcBmd).replace_extension(".debug").string().c_str());
	};
	*/
private:
	BOOL Decrypt();
	BOOL Encrypt();
	//void TxtOut(std::ofstream & os);
	//void TxtIn(std::ifstream& is);
};

#endif

//=======================================================================

#ifndef PET_DATA_BMD_H
#define PET_DATA_BMD_H

#define _PET_DATA_BMD_WKEY_ 0x3BB2

typedef TxtConvertor<PET_DATA> PetDataBmdConvert;
class PetDataBmd : public PetDataBmdConvert
{
public:
	PetDataBmd() : PetDataBmdConvert(_PET_DATA_BMD_WKEY_) { hasCounter = false; };
	virtual ~PetDataBmd() {};
	/*
	BOOL Unpack(const char *szSrcBmd, const char *szDestTxt)
	{
		sizeof(PET_DATA);
		return FileOpen(szSrcBmd)
			&& Decrypt()
			&& FileWrite(fs::path(szSrcBmd).replace_extension(".debug").string().c_str());
	};
	*/
};

#endif