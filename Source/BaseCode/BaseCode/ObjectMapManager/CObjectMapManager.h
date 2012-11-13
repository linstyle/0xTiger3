/*
	@lindp lin_style@foxmail.com   
	@2012/2/28
	@管理网络连接。
	    比如一个GS登陆上来，则添加接口SendByGS。如果是一对多的发送，则
	   SendByRole等。

   @2012/3/22更新
   更明确定义为对象的转接层。
   维护了 userid->NetObject, userid->LogicPlayer
   发包时，从userid找到NetObject,
   收包时，从userid找到LogicPlayer
   这样不依赖具体的套接字连接查询
*/
#include "Singleton.h"
#include "CBaseLogicPlayer.h"
#include "CMyStackWalker.h"
#include "PPackets.h"
#include <map>

using namespace std;

namespace name_objectmap
{
	struct OBJECT_VALUE
	{
		OBJECT_VALUE(void* pNetObject, CBaseLogicPlayer* pBaseLogicPlayer)
		{
			m_pNetObject=pNetObject;
			m_pBaseLogicPlayer=pBaseLogicPlayer;
		}

		OBJECT_VALUE()
		{
			m_pNetObject=NULL;
			m_pBaseLogicPlayer=NULL;
		}
		void* m_pNetObject;
		CBaseLogicPlayer* m_pBaseLogicPlayer;
	};

	enum IDENT
	{
		GS=0,
		HS=1,
		LS=2
	};

/*
	序列号暂时这么分
	1-100:gs
	101-200:hs
	201-300:ls
	300-1000:空闲
	1001-xxxxxxxx:player
*/
	bool IsGS(INT64 nUserID);
	bool IsLG(INT64 nUserID);
	bool IsHS(INT64 nUserID); 
};

class CObjectMapManager: public Singleton<CObjectMapManager>
{
public:
	CObjectMapManager();

	/*
		这里的player指普通的客户端连接
	*/
	void AddPlayer(INT64 nUserID, void *pNetObject, CBaseLogicPlayer* pBaseLogicPlayer);
	CBaseLogicPlayer* GetPlayerByUserID();
	void* GetNetObjectByUserID();

	void SendByUser(INT64 nUserID, const char* pBuffer, int nBufferLen);
	void CloseByUser(INT64 nUserID);

	//提权
	bool ChangeIdent(INT64 nUserID, name_objectmap::IDENT nIdent);
	
	/*
		方便使用，也可以抽象出一些高级接口
	*/
	void SendToLS(const char *pBuffer, int nBufferLen);


	//void TracerMe();
private:
	name_objectmap::OBJECT_VALUE* GetObjectValueByUserID(INT64 nUserID);
	bool VerifyUserValid(INT64 nUserID);
	//检查包大小正确性
	bool VerifySendPacket(const IPackHead *pPackHead, int nBufferLen);
	int PutPacketStream(const char *pBuffer,  int nBufferLen, void *pNetObject);

public:

private:
	typedef map<INT64,  name_objectmap::OBJECT_VALUE*> MAP_USERID_OBJECT;

	MAP_USERID_OBJECT m_mapUserIDObject;
};

#define g_NetObjectManager CObjectMapManager::getSingleton()