#include "CCmInitPackets.h"
#include "PInnerTransfer.h"


initialiseSingleton(CCmInitPackets);

CCmInitPackets::CCmInitPackets()
{
	new POInnerTransferToNet;   //内部协议
	//new PLoginGLObject;               //登陆包
}


CCmInitPackets::~CCmInitPackets()
{
	delete POInnerTransferToNet::getSingletonPtr();
	//delete PLoginGLObject::getSingletonPtr();
}