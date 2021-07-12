
#ifndef MURUMMY_OPTION_BMD_H
#define MURUMMY_OPTION_BMD_H

#define _MURUMMY_OPTION_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<MURUMMY_OPTION> MuRummyOptionBmdConvert;
class MuRummyOptionBmd : public MuRummyOptionBmdConvert
{
public:
	MuRummyOptionBmd() : MuRummyOptionBmdConvert(_MURUMMY_OPTION_BMD_WKEY_) {};
	virtual ~MuRummyOptionBmd() {};
};

#endif

//=======================================================================

#ifndef MURUMMY_CARD_BMD_H
#define MURUMMY_CARD_BMD_H

#define _MURUMMY_CARD_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<MURUMMY_CARD> MuRummyCardBmdConvert;
class MuRummyCardBmd : public MuRummyCardBmdConvert
{
public:
	MuRummyCardBmd() : MuRummyCardBmdConvert(_MURUMMY_CARD_BMD_WKEY_) {};
	virtual ~MuRummyCardBmd() {};
};

#endif

//=======================================================================

#ifndef MURUMMY_SLOT_BMD_H
#define MURUMMY_SLOT_BMD_H

#define _MURUMMY_SLOT_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<MURUMMY_SLOT> MuRummySlotBmdConvert;
class MuRummySlotBmd : public MuRummySlotBmdConvert
{
public:
	MuRummySlotBmd() : MuRummySlotBmdConvert(_MURUMMY_SLOT_BMD_WKEY_) {};
	virtual ~MuRummySlotBmd() {};
};

#endif
