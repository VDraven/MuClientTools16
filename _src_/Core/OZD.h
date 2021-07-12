#ifndef OZD_H
#define OZD_H

typedef GraphicConvertor<EXT_OZD, 34> OzdConvertor;
class OZD : public OzdConvertor
{
public:
	OZD() {};
	virtual ~OZD() {};

private:
	BOOL Decrypt() { return ModulusDecrypt(_buf); };
	BOOL Encrypt() { return ModulusEncrypt(_buf); };
};

#endif // !OZD_H
