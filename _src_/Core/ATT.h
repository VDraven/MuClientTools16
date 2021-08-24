#ifndef ATT_H
#define ATT_H

typedef GraphicConvertor<EXT_ATT> AttConvertor;
class ATT : public AttConvertor
{
public:
	ATT() {};
	virtual ~ATT() {};

private:
	BOOL Decrypt();
	BOOL Encrypt();
};

#endif // !MAP_H
