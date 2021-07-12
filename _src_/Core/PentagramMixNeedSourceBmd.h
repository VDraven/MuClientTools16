
#ifndef PENTAGRAM_MIX_SOURCE_NEED_BMD_H
#define PENTAGRAM_MIX_SOURCE_NEED_BMD_H

#define _PENTAGRAM_MIX_SOURCE_NEED_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<PENTAGRAM_MIX_SOURCE> PentagramMixSourceNeedBmdConvert;
class PentagramMixNeedSourceBmd : public PentagramMixSourceNeedBmdConvert
{
public:
	PentagramMixNeedSourceBmd() : PentagramMixSourceNeedBmdConvert(_PENTAGRAM_MIX_SOURCE_NEED_BMD_WKEY_) {};
	virtual ~PentagramMixNeedSourceBmd() {};

private:
	void MakeLabelEx(std::ofstream& os)
	{
		assert(os);
		os << "// Cat0: Material Combination" << std::endl;
		os << "// Cat1: Errtel" << std::endl;
		os << "// Cat2 : Errtel Level Upgrade" << std::endl;
		os << "// Cat3 : Errtel Rank Upgrade" << std::endl;
		os << "// Cat6: Wing Add Option" << std::endl;
		os << "// Cat7: Wing Option Level Upgrade" << std::endl;
	};
	int GetKey(PENTAGRAM_MIX_SOURCE* ptr) { return (ptr->Category << 8) | ptr->Index; };
};

#endif