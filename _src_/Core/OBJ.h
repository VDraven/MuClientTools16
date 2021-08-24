#ifndef OBJ_H
#define OBJ_H

typedef GraphicConvertor<EXT_MAP> ObjConvertor;
class OBJ : public ObjConvertor
{
public:
	OBJ() {};
	virtual ~OBJ() {};

private:
	BOOL Decrypt();
	BOOL Encrypt();
};

#endif // !MAP_H
