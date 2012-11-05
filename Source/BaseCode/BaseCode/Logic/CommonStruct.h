/*  作者：		林东平                                                              
	创建时间：	2011/1/19                                                         
    功能说明：  commonlib里的一些公共结构体
*/

//#include "CNetObject.h"
/*
	网络层投放到任务的结构
	如果修改，注意PPutTaskQueueObject::Execute的拷贝次序
*/

struct NET_TASK_QUEUE
{
	void* pPlayLogic;
	void* pPlayNet;
	int GetPackHeadOffset()
	{
		return sizeof(void*)+sizeof(void*);
	}
	/*
		IPackHead* pPackHead
	*/
};