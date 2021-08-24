#ifndef OZG_H
#define OZG_H

typedef GraphicConvertor<EXT_OZG> OzgConvertor;
class OZG : public OzgConvertor
{
public:
	OZG() {};
	virtual ~OZG() {};

private:
	BOOL Decrypt() { return ModulusDecrypt(_buf); };
	BOOL Encrypt() { return ModulusEncrypt(_buf); };
};

#endif // !OZG_H
