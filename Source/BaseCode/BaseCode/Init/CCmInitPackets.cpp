#include "CCmInitPackets.h"
#include "PInnerTransfer.h"


initialiseSingleton(CCmInitPackets);

CCmInitPackets::CCmInitPackets()
{
	new POInnerTransfer;   //内部协议
	//new PLoginGLObject;               //登陆包
}


CCmInitPackets::~CCmInitPackets()
{
	delete POInnerTransfer::getSingletonPtr();
	//delete PLoginGLObject::getSingletonPtr();
}