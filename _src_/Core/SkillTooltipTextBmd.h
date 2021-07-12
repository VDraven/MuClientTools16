
#ifndef SKILL_TOOLTIP_TEXT_BMD_H
#define SKILL_TOOLTIP_TEXT_BMD_H

#define _SKILL_TOOLTIP_TEXT_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<SKILL_TOOLTIP_TEXT> SkillTooltipTextBmdConvert;
class SkillTooltipTextBmd : public SkillTooltipTextBmdConvert
{
public:
	SkillTooltipTextBmd() : SkillTooltipTextBmdConvert(_SKILL_TOOLTIP_TEXT_BMD_WKEY_) {};
	virtual ~SkillTooltipTextBmd() {};
};

#endif