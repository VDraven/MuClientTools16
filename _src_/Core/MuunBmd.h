
#ifndef MUUN_OPTION_BMD_H
#define MUUN_OPTION_BMD_H

#define _MUUN_OPTION_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<MUUN_OPTION> MuunOptionBmdConvert;
class MuunOptionBmd : public MuunOptionBmdConvert
{
public:
	MuunOptionBmd() : MuunOptionBmdConvert(_MUUN_OPTION_BMD_WKEY_) {};
	virtual ~MuunOptionBmd() {};

private:
	int GetKey(MUUN_OPTION* ptr) { return ptr->ID; };
};

#endif

//=======================================================================

#ifndef MUUN_INFO_BMD_H
#define MUUN_INFO_BMD_H

#define _MUUN_INFO_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<MUUN_INFO> MuunInfoBmdConvert;
class MuunInfoBmd : public MuunInfoBmdConvert
{
public:
	MuunInfoBmd() : MuunInfoBmdConvert(_MUUN_INFO_BMD_WKEY_) {};
	virtual ~MuunInfoBmd() {};

private:
	int GetKey(MUUN_INFO* ptr) { return ptr->ID; };
};

#endif

//=======================================================================

#ifndef MUUN_TOOLTIP_INFO_BMD_H
#define MUUN_TOOLTIP_INFO_BMD_H

#define _MUUN_TOOLTIP_INFO_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<MUUN_TOOLTIP_INFO> MuunTooltipInfoBmdConvert;
class MuunTooltipInfoBmd : public MuunTooltipInfoBmdConvert
{
public:
	MuunTooltipInfoBmd() : MuunTooltipInfoBmdConvert(_MUUN_TOOLTIP_INFO_BMD_WKEY_) {};
	virtual ~MuunTooltipInfoBmd() {};

private:
	int GetKey(MUUN_TOOLTIP_INFO* ptr) { return ptr->MuunID; };
};

#endif

//=======================================================================

#ifndef MUUN_RENDER_INFO_BMD_H
#define MUUN_RENDER_INFO_BMD_H

#define _MUUN_RENDER_INFO_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<MUUN_RENDER_INFO> MuunRenderInfoBmdConvert;
class MuunRenderInfoBmd : public MuunRenderInfoBmdConvert
{
public:
	MuunRenderInfoBmd() : MuunRenderInfoBmdConvert(_MUUN_RENDER_INFO_BMD_WKEY_) {};
	virtual ~MuunRenderInfoBmd() {};

private:
	int GetKey(MUUN_RENDER_INFO* ptr) { return ptr->ID; };
};

#endif


//=======================================================================

#ifndef MUUN_MESH_INFO_BMD_H
#define MUUN_MESH_INFO_BMD_H

#define _MUUN_MESH_INFO_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<MUUN_MESH_INFO> MuunMeshInfoBmdConvert;
class MuunMeshInfoBmd : public MuunMeshInfoBmdConvert
{
public:
	MuunMeshInfoBmd() : MuunMeshInfoBmdConvert(_MUUN_MESH_INFO_BMD_WKEY_) {};
	virtual ~MuunMeshInfoBmd() {};

private:
	int GetKey(MUUN_MESH_INFO* ptr) { return ptr->MuunID; };
};

#endif