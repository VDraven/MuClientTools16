
#ifndef MAP_CHARACTERS_BMD_H
#define MAP_CHARACTERS_BMD_H

typedef TxtConvertor<MAP_CHARACTERS> MapCharactersBmdConvert;
class MapCharactersBmd : public MapCharactersBmdConvert
{
public:
	MapCharactersBmd() : MapCharactersBmdConvert() {};
	virtual ~MapCharactersBmd() {};

private:
	int GetKey(MAP_CHARACTERS* ptr) { return ptr->Map; };
	BOOL Decrypt();
	BOOL Encrypt();
	void TxtOut(std::ofstream & os);
	void TxtIn(std::ifstream& is);
};

#endif