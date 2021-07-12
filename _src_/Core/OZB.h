#ifndef OZB_H
#define OZB_H

typedef GraphicConvertor<EXT_OZB, 4> OzbConvertor;
class OZB : public OzbConvertor
{
public:
	OZB() {};
	virtual ~OZB() {};
};

#endif // !OZB_H
