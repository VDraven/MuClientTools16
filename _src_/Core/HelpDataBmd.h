#pragma once

#ifndef HELP_DATA_BMD_H
#define HELP_DATA_BMD_H

#define _HELP_DATA_BMD_WKEY_ 0xC4F7

typedef TxtConvertor<HELP_DATA> HelpDataBmdConvert;
class HelpDataBmd : public HelpDataBmdConvert
{
public:
	HelpDataBmd() : HelpDataBmdConvert(_HELP_DATA_BMD_WKEY_) { hasCounter = false; };
	virtual ~HelpDataBmd() {};

};

#endif