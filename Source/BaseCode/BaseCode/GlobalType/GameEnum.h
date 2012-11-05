/*  作者：		林东平                                                              
	创建时间：	2011/02/28                                                         
    功能说明：  整个游戏共同用到的枚举值
*/

#pragma  once

//游戏类的大种类,保证序号的递增（从0开始）
enum GAME_IDENTI_TYPE
{
	GAME_IDENTI_HUMAN,				//玩家
	GAME_IDENTI_ITEM,					//物品
	GAME_IDENTI_MONSTER,			//怪物


	GAME_IDENTI_COUNT_MAX		//
};