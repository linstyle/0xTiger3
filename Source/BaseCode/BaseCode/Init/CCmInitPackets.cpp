
#include "CCmInitPackets.h"
//#include "PInnerTransfer.h"


initialiseSingleton(CCmInitPackets);

CCmInitPackets::CCmInitPackets()
{
	//new PInnerTransfer;   //内部协议
	//new PLoginGLObject;               //登陆包
}


CCmInitPackets::~CCmInitPackets()
{
	//delete PInnerTransfer::getSingletonPtr();
	//delete PLoginGLObject::getSingletonPtr();
}