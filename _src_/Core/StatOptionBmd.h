
#ifndef STAT_OPTION_BMD_H
#define STAT_OPTION_BMD_H

#define _STAT_OPTION_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<STAT_OPTION> StatOptionBmdConvert;
class StatOptionBmd : public StatOptionBmdConvert
{
public:
	StatOptionBmd() : StatOptionBmdConvert(_STAT_OPTION_BMD_WKEY_) {};
	virtual ~StatOptionBmd() {};

private:
	int GetKey(STAT_OPTION* ptr) { return (ptr->Class << 16) | (ptr->StatType << 8) | (ptr->TextID); };
};

#endif

//=================================================================

#ifndef STAT_OPTION_TEXT_BMD_H
#define STAT_OPTION_TEXT_BMD_H

#define _STAT_OPTION_TEXT_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<STAT_OPTION_TEXT> StatOptionTextBmdConvert;
class StatOptionTextBmd : public StatOptionTextBmdConvert
{
public:
	StatOptionTextBmd() : StatOptionTextBmdConvert(_STAT_OPTION_TEXT_BMD_WKEY_) {};
	virtual ~StatOptionTextBmd() {};

private:
	int GetKey(STAT_OPTION_TEXT* ptr) { return ptr->ID; };
};

#endif