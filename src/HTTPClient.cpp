#include "EtherSia.h"
#include "util.h"



HTTPClient::HTTPClient(EtherSia &ether) : TCPClient(ether)
{
}
