
#ifndef MIX_BMD_H
#define MIX_BMD_H

typedef TxtConvertor<MIX_INFO> MixBmdConvert;
class MixBmd : public MixBmdConvert
{
public:
	MixBmd() : MixBmdConvert() { hasCounter = false; hasCRC = false; };
	virtual ~MixBmd() { sizeof(MIX_INFO); sizeof(MIX_ITEM); };

private:

	BOOL Decrypt();
	BOOL Encrypt();
	void TxtOut(std::ofstream & os);
	void TxtIn(std::ifstream& is);
};

#endif