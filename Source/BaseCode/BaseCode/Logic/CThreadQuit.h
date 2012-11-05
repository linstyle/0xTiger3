/*  作者：		林东平                                                              
	创建时间：	2010/07/29                                                         
    功能说明：  控制全局退出线程
*/

#include "Singleton.h"
#include "OSOther.h"
#include "MTASSERT.h"

class CThreadQuit:public Singleton<CThreadQuit>
{
public:
	CThreadQuit();
	~CThreadQuit();


private:
	void Init();

	static unsigned int WINAPI ThreadProc(void* pParam);


public:

private:
	HANDLE m_hThread;
	unsigned int   m_uThread;
};

#define g_ThreadQuit CThreadQuit::getSingleton()

