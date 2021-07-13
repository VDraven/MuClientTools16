#pragma once

#ifndef RUUD_SHOP_VIEW_INFO_BMD_H
#define RUUD_SHOP_VIEW_INFO_BMD_H

#define _RUUD_SHOP_VIEW_INFO_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<RUUD_SHOP_VIEW_INFO> RuudShopViewInfoBmdConvert;
class RuudShopViewInfoBmd : public RuudShopViewInfoBmdConvert
{
public:
	RuudShopViewInfoBmd() : RuudShopViewInfoBmdConvert(_RUUD_SHOP_VIEW_INFO_BMD_WKEY_) {};
	virtual ~RuudShopViewInfoBmd() {};
};

#endif