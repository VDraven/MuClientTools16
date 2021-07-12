
#ifndef INFO_TOOLTIP_TEXT_BMD_H
#define INFO_TOOLTIP_TEXT_BMD_H

#define _INFO_TOOLTIP_TEXT_BMD_WKEY_ 0xA4C6

typedef TxtConvertor<INFO_TOOLTIP_TEXT> InfoTooltipTextBmdConvert;
class InfoTooltipTextBmd : public InfoTooltipTextBmdConvert
{
public:
	InfoTooltipTextBmd() : InfoTooltipTextBmdConvert(_INFO_TOOLTIP_TEXT_BMD_WKEY_) {};
	virtual ~InfoTooltipTextBmd() {};

private:
	int GetKey(INFO_TOOLTIP_TEXT* ptr) { return ptr->ID; };
};

#endif