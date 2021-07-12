
#ifndef EVO_MONSTER_BOX_BMD_H
#define EVO_MONSTER_BOX_BMD_H

#define _EVO_MONSTER_BOX_BMD_WKEY_ 0xC4A2

typedef TxtConvertor<EVO_MONSTER_BOX> EvoMonsterBoxBmdConvert;
class EvoMonsterBoxBmd : public EvoMonsterBoxBmdConvert
{
public:
	EvoMonsterBoxBmd() : EvoMonsterBoxBmdConvert(_EVO_MONSTER_BOX_BMD_WKEY_) {};
	virtual ~EvoMonsterBoxBmd() {};

private:
	BOOL Decrypt();
	BOOL Encrypt();
	void MakeLabelEx(std::ofstream& os);
};

#endif