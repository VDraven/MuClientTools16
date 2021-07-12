
#ifndef MUUN_EXCHANGE_BMD_H
#define MUUN_EXCHANGE_BMD_H

#define _MUUN_EXCHANGE_BMD_WKEY_ _COMMON_TXTCONVERTOR_WKEY_

typedef TxtConvertor<MUUN_EXCHANGE> MuunExchangeConvert;
class MuunExchangeBmd : public MuunExchangeConvert
{
public:
	MuunExchangeBmd() : MuunExchangeConvert(_MUUN_EXCHANGE_BMD_WKEY_) {};
	virtual ~MuunExchangeBmd() {};

private:
	BOOL Decrypt();
	BOOL Encrypt();
	void TxtOut(std::ofstream& os);
	void TxtIn(std::ifstream& is);
};

#endif