/*  作者：		林东平                                                              
	创建时间：	2011/03/03                                                         
    功能说明：  服务器共同用到的枚举值
*/

//游戏类的大种类,保证序号的递增（从0开始）
enum GAME_IDENTI_TYPE
{
	GAME_IDENTI_HUMAN,				//玩家
	GAME_IDENTI_ITEM,					//物品
	GAME_IDENTI_MONSTER,			//怪物


	GAME_IDENTI_COUNT_MAX		//
};

//服务器标志
enum
{
	SERVER_IDENTI_CLIENT,
	SERVER_IDENTI_GATEWAY,
	SERVER_IDENTI_CHAT,
	SERVER_IDENTI_DATA
};