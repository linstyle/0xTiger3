#include "CCmInitPackets.h"
#include <WinSock2.h>
#include "PInnerTransfer.h"


initialiseSingleton(CCmInitPackets);

CCmInitPackets::CCmInitPackets()
{
	new POInnerTransfer;   //�ڲ�Э��
	//new PLoginGLObject;               //��½��
}


CCmInitPackets::~CCmInitPackets()
{
	delete POInnerTransfer::getSingletonPtr();
	//delete PLoginGLObject::getSingletonPtr();
}