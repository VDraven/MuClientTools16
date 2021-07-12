
#ifndef MONSTER_SKILL_BMD_H
#define MONSTER_SKILL_BMD_H

typedef TxtConvertor<MONSTER_SKILL> MonsterSkillBmdConvert;
class MonsterSkillBmd : public MonsterSkillBmdConvert
{
public:
	MonsterSkillBmd() : MonsterSkillBmdConvert() { hasCRC = false; };
	virtual ~MonsterSkillBmd() {};
};

#endif