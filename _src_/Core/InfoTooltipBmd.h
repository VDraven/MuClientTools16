
#ifndef INFO_TOOLTIP_BMD_H
#define INFO_TOOLTIP_BMD_H

#define _INFO_TOOLTIP_BMD_WKEY_ 0xA4C6

typedef TxtConvertor<INFO_TOOLTIP> InfoTooltipBmdConvert;
class InfoTooltipBmd : public InfoTooltipBmdConvert
{
public:
	InfoTooltipBmd() : InfoTooltipBmdConvert(_INFO_TOOLTIP_BMD_WKEY_) {};
	virtual ~InfoTooltipBmd() {};

private:
	int GetKey(INFO_TOOLTIP* ptr) { return ptr->ID; };
};

#endif