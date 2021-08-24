#ifndef MAP_H
#define MAP_H

typedef GraphicConvertor<EXT_MAP> MapConvertor;
class MAP : public MapConvertor
{
public:
	MAP() {};
	virtual ~MAP() {};

private:
	BOOL Decrypt();
	BOOL Encrypt();
};

#endif // !MAP_H
