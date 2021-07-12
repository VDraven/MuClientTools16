
#ifndef FORMULA_DATA_BMD_H
#define FORMULA_DATA_BMD_H

#define _FORMULA_DATA_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<FORMULA_DATA> FormulaDataConvert;
class FormulaDataBmd : public FormulaDataConvert
{
public:
	FormulaDataBmd() : FormulaDataConvert(_FORMULA_DATA_BMD_WKEY_) {};
	virtual ~FormulaDataBmd() {};

private:
	BOOL Decrypt();
	BOOL Encrypt();
	void TxtOut(std::ofstream& os);
	void TxtIn(std::ifstream& is);
};

#endif