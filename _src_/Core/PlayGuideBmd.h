
#ifndef PLAY_GUIDE_BMD_H
#define PLAY_GUIDE_BMD_H

#define _PLAY_GUIDE_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<PLAY_GUIDE_INFO> PlayGuideBmdConvert;
class PlayGuideBmd : public PlayGuideBmdConvert
{
public:
	PlayGuideBmd() : PlayGuideBmdConvert(_PLAY_GUIDE_BMD_WKEY_) {};
	virtual ~PlayGuideBmd() {};

private:
	int GetKey(PLAY_GUIDE_INFO* ptr) { return ptr->ID; };
};

#endif
