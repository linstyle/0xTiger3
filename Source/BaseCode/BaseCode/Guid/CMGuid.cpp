#include "CMGuid.h"
#include "GlobalFunction.h"

initialiseSingleton(CMGuid);

void CMGuid::Init(int nBatchFreeGuid, wchar_t *pFileName, GUID_SET* pGuidSet)
{
	m_nBatchFreeGuid = nBatchFreeGuid;

	MEMSET(&m_strFileName, 0 , sizeof(m_strFileName));
	wcsncpy_s(m_strFileName, pFileName, name_mguid::LEN_FILE_NAME);

	memcpy(m_nGuidSet, pGuidSet, sizeof(m_nGuidSet));
}

MGUID CMGuid::MallocGUID(int _nQuID, GAME_IDENTI_TYPE  IdentiType, int _nSubType)
{
	//nField1~nField5
	int nQuID;
	int nIdentiType;
	int nSubType;
	int nFieldBack;
	unsigned int nSerialNumber;

	nQuID = _nQuID;
	nIdentiType = IdentiType;
	nSubType = _nSubType;
	nFieldBack = 0;
	nSerialNumber = GetSerialNumber(IdentiType);	
	if(0==nSerialNumber)
	{
		//表示值为空
		return 0;
	}
	

	return FormatGUID(nQuID, nIdentiType, nSubType, nFieldBack, nSerialNumber);
}

unsigned int CMGuid::GetSerialNumber(GAME_IDENTI_TYPE  IdentiType)
{
	GUID_SET *pGuidSet = GetGuidSet(IdentiType);
	if(NULL==pGuidSet)
	{
		printf("Err, CGuid::GetSerialNumber:GetGuidSet. IdentiType(%d)\n", IdentiType);
		return 0;
	}

	//判断是否有存余量
	if(pGuidSet->m_nGuidBegin==pGuidSet->m_nGuidEnd)
	{
		UpdateGuidSet(IdentiType, pGuidSet);		
	}

	//可能UpdateGuidSet失败
	if(pGuidSet->m_nGuidBegin==pGuidSet->m_nGuidEnd)
	{
		return 0;	
	}

	return pGuidSet->m_nGuidBegin++;
}


void DebugBit(MGUID mGuid)
{
	//for(int i = sizeof(mGuid)*8-1; i>= 0; --i) 
	//{ 
	//	printf( "%d ",   (mGuid>>i)&1); 
	//}
	//printf("\n");
}

MGUID CMGuid::FormatGUID(int nField1, int nField2, int nField3, int nField4, unsigned int nSerialNumber)
{
	/*
		32位机器下，有int64的数据类型，不支持32位以上的直接移位。只好通过int64的中间值操作
	*/
	MGUID Guid, Guid1, Guid2, Guid3, Guid4, Guid5;
	
	int nSumBit = name_mguid::FIELD1_BIT + name_mguid::FIELD2_BIT + name_mguid::FIELD3_BIT +
				  name_mguid::FIELD4_BIT + name_mguid::FIELD5_BIT;

	Guid1 = nField1;
	Guid1 = Guid1<<(nSumBit-name_mguid::FIELD1_BIT);
	DebugBit(Guid1);

	Guid2 = nField2;
	Guid2 = Guid2 << (nSumBit-name_mguid::FIELD1_BIT-name_mguid::FIELD2_BIT);
	DebugBit(Guid2);

	Guid3 = nField3;
	Guid3 = Guid3 << (nSumBit-name_mguid::FIELD1_BIT-name_mguid::FIELD2_BIT-name_mguid::FIELD3_BIT);
	DebugBit(Guid3);

	Guid4 = nField4;
	Guid4 = Guid4 << (nSumBit-name_mguid::FIELD1_BIT-name_mguid::FIELD2_BIT-name_mguid::FIELD3_BIT-name_mguid::FIELD4_BIT);
	DebugBit(Guid4);

	Guid5 = nSerialNumber;
	Guid5 = Guid5 << (nSumBit-name_mguid::FIELD1_BIT-name_mguid::FIELD2_BIT-name_mguid::FIELD3_BIT-name_mguid::FIELD4_BIT-name_mguid::FIELD5_BIT);
	DebugBit(Guid5);

	Guid = Guid1+Guid2+Guid3+Guid4+Guid5;
	DebugBit(Guid);

	return Guid;
}

GUID_SET *CMGuid::GetGuidSet(int nIndex)
{
	if(nIndex>=GAME_IDENTI_COUNT_MAX)
	{
		return NULL;
	}

	return &m_nGuidSet[nIndex];
}

void CMGuid::UpdateGuidSet(GAME_IDENTI_TYPE IdentiType, GUID_SET *pGuidSet)
{
	wchar_t strAppName[name_mguid::LEN_APP_NAME]; 
	wchar_t strKeyName[name_mguid::LEN_KEY_NAME];
	wchar_t strValueName[name_mguid::LEN_VALUE_NAME];

	MEMSET(&strAppName, 0, sizeof(strAppName));
	MEMSET(&strKeyName, 0, sizeof(strKeyName));
	MEMSET(&strValueName, 0, sizeof(strValueName));

	swprintf(strAppName, name_mguid::LEN_APP_NAME-4, L"GUID");
	switch(IdentiType)
	{
	case GAME_IDENTI_HUMAN:
		swprintf(strKeyName, name_mguid::LEN_KEY_NAME-4, L"IDENTI_HUMAN_BEGIN");
		break;
	case GAME_IDENTI_ITEM:
		swprintf(strKeyName, name_mguid::LEN_KEY_NAME-4, L"IDENTI_ITEM_BEGIN");
		break;
	case GAME_IDENTI_MONSTER:
		swprintf(strKeyName, name_mguid::LEN_KEY_NAME-4, L"IDENTI_MONSTER_BEGIN");
		break;
	default:
		printf("Err, CGuid::UpdateGuidSet, case IdentiType(%d) default\n", IdentiType);
	}

	//回写
	
	pGuidSet->m_nGuidEnd += m_nBatchFreeGuid;
	swprintf(strValueName, name_mguid::LEN_VALUE_NAME-4, L"%d", pGuidSet->m_nGuidEnd);
	if( 0==WritePrivateProfileString(strAppName, strKeyName, strValueName, m_strFileName) )
	{
		wprintf(L"Err, CGuid::UpdateGuidSet:WritePrivateProfileString(ErrID:%d), strAppName(%s), strKeyName(%s), strValueName(%s), m_strFileName(%s)\n", 
			GetLastError(),strAppName, strKeyName, strValueName, m_strFileName);
		//回退值
		pGuidSet->m_nGuidEnd -= m_nBatchFreeGuid;
	}
}