#include "CCmInitPackets.h"
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