#include "CCmInitPackets.h"
#include "PInnerTransfer.h"


initialiseSingleton(CCmInitPackets);

CCmInitPackets::CCmInitPackets()
{
	new POInnerTransferToNet;   //�ڲ�Э��
	//new PLoginGLObject;               //��½��
}


CCmInitPackets::~CCmInitPackets()
{
	delete POInnerTransferToNet::getSingletonPtr();
	//delete PLoginGLObject::getSingletonPtr();
}