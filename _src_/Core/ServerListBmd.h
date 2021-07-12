
#ifndef SERVER_LIST_BMD_H
#define SERVER_LIST_BMD_H

typedef TxtConvertor<SERVER_LIST> ServerListBmdConvert;
class ServerListBmd : public ServerListBmdConvert
{
public:
	ServerListBmd() : ServerListBmdConvert() {};
	virtual ~ServerListBmd() {};

private:
	BOOL Decrypt();
	BOOL Encrypt();
	void TxtOut(std::ofstream& os);
	void TxtIn(std::ifstream& is);
};

#endif