#ifndef OZJ_H
#define OZJ_H

typedef GraphicConvertor<EXT_OZJ, 24> OzjConvertor;
class OZJ : public OzjConvertor
{
public:
	OZJ() {};
	virtual ~OZJ() {};
};

#endif // !OZJ_H
