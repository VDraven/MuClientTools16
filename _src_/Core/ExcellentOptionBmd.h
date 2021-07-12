
#ifndef EXC_OPT_BMD_H
#define EXC_OPT_BMD_H

#define _EXC_OPT_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<EXCELLENT_OPTION> ExcellentOptionConvert;
class ExcellentOptionBmd : public ExcellentOptionConvert
{
public:
	ExcellentOptionBmd() : ExcellentOptionConvert(_EXC_OPT_BMD_WKEY_) {};
	virtual ~ExcellentOptionBmd() {};

private:
	int GetKey(EXCELLENT_OPTION* ptr) { return (ptr->Category << 8) | ptr->Number; };
};

#endif