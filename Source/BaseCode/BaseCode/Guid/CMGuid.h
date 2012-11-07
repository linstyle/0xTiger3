/*  作者：			林东平                                                              
	创建时间：	2011/03/01                                                         
    功能说明：  GUID生成
	guid这东西其实是无限的最好。不用担忧不够用等等。有位数的限制上，一般都是有一些特殊需求。比如
	我在程序里经常会用到HASH，我希望是数字作为键值，还有各种内存拷贝等等。所以它更不倾向于一种验证
	的东西。
	那么在分配之前，我都会估算好应用的需求。因为这个东西后期改起来
	一是非常麻烦，在各种扩展和解析上，
	二是该应用发展得非常NB了，作为初创的你，已经不需要担心了^_^
*/

#pragma  once

#include "GlobalEnum.h"
#include "Singleton.h"
#include "GlobalMacro.h"
/*
	GUID 共8个字节，64位。划分如下：
	0  -11bit :服务器区号,		12bit, 支持 0-4096。
    12-18bit :第一大类，		7bit,  支持 0-127
	19-25bit :第二大类，		7bit,  支持0-127
	26-31bit :备用,					6bit,  支持0-64

	第5~8字节：序号。从配置文件中读取
*/
namespace name_mguid
{
	const int FIELD1_BIT = 12;
	const int FIELD2_BIT = 7;
	const int FIELD3_BIT = 7;
	const int FIELD4_BIT = 6;
	const int FIELD5_BIT = 32;

	//写入磁盘文件的路径名
	const int LEN_APP_NAME = 64;
	const int LEN_KEY_NAME = 64;
	const int LEN_VALUE_NAME = 24;
	const int LEN_FILE_NAME = 128;
};

struct GUID_SET
{
	GUID_SET()
	{
		m_nGuidBegin = m_nGuidEnd = 0;
	}
	/*
		m_nGuidBegin:		分配的起始基数,每次分配后都会自加
		m_nGuidEnd:			分配的截止数
	*/
	unsigned int m_nGuidBegin;
	unsigned int m_nGuidEnd;
};

class CMGuid:public Singleton<CMGuid>
{
public:
	/*
		Init
		nBatchFreeGuid:一次性申请多少空余的GUID
		pGuidSet:配置文件中的初始化数据,大小是IDENTI_COUNT_MAX
	*/
	void Init(int nBatchFreeGuid, wchar_t *pFileName, GUID_SET* pGuidSet);

	/*
		MallocGUID
		返回一个GUID值。对整个GUID字段的填充如下
		12-18bit :第一大类，即参数IdentiType
		19-25bit :第二大类, 即参数nSubType
		26-31bit :备用,默认0
		32-63bit :序号，内部写入
	*/
	MGUID MallocGUID(int _nQuID, GAME_IDENTI_TYPE  IdentiType, int _nSubType);

private:
	/*
		GetSerialNumber
		根据IdentiType,找到对应的m_nGuidSet,然后返回一个可用编号
		如果返回为0，表示编号出错（用光或者其他）
	*/
	unsigned int GetSerialNumber(GAME_IDENTI_TYPE  IdentiType);
	MGUID FormatGUID(int nField1, int nField2, int nField3, int nField4, unsigned int nSerialNumber);

	void UpdateGuidSet(GAME_IDENTI_TYPE IdentiType, GUID_SET *pGuidSet);
	GUID_SET *GetGuidSet(int nIndex);
public:

private:
	//一次性申请多少空余的GUID
	int m_nBatchFreeGuid;
	GUID_SET m_nGuidSet[GAME_IDENTI_COUNT_MAX];

	//要写入磁盘文件的路径
	wchar_t m_strFileName[name_mguid::LEN_FILE_NAME];
};

#define g_Guid CMGuid::getSingleton()