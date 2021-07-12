
#ifndef SOCKET_ITEM_BMD_H
#define SOCKET_ITEM_BMD_H

typedef TxtConvertor<SOCKET_ITEM_INFO> SocketItemBmdConvert;
class SocketItemBmd : public SocketItemBmdConvert
{
public:
	SocketItemBmd() : SocketItemBmdConvert() {};
	virtual ~SocketItemBmd() {};

private:
	BOOL Decrypt();
	BOOL Encrypt();
	void TxtIn(std::ifstream& is);
};

#endif