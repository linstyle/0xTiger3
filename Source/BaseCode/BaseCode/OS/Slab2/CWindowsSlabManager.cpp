#include "stdafx.h"
#include "CWindowsSlabManager.h"

initialiseSingleton(CWindowsSlabManager);



void* name_slab::kmem_malloc(int nSizeByte)
{
	return g_KmemSlab.KmemMalloc(nSizeByte);
}

void name_slab::kmem_free(void *p)
{
	g_KmemSlab.KmemFree(p);
}

void name_slab::kemem_traceme()
{
	g_KmemSlab.TraceMe();
}


CWindowsSlabManager::CWindowsSlabManager()
{
	Init();
}

void CWindowsSlabManager::Init()
{
	int nKmemCount = sizeof(name_slab::KmemMallocByte)/sizeof(*name_slab::KmemMallocByte);

	for (int i=0; i<nKmemCount; ++i)
	{
		m_Kmem[i].Init(name_slab::KmemMallocByte[i]);
	}	
}


int CWindowsSlabManager::GetKmemIndex(int nSizeByte)
{
	int nKmemCount = sizeof(name_slab::KmemMallocByte)/sizeof(*name_slab::KmemMallocByte);

	for (int i=0; i<nKmemCount; ++i)
	{
		if (name_slab::KmemMallocByte[i]>=nSizeByte)
		{
			return i;
		}
	}

	return -1;
}

void* CWindowsSlabManager::KmemMalloc(int nSizeByte)
{
	int nIndex = GetKmemIndex(nSizeByte);
	if(-1==nIndex)
	{
		return NULL;
	}

	CKmem *pKmem = &m_Kmem[nIndex];
	if (NULL==pKmem)
	{
		return NULL;
	}

	return pKmem->Kmalloc();
}

void CWindowsSlabManager::KmemFree(void *p)
{
	VIR_PAGE_HEAD *pPage = name_slab::OffsetPageHead(p);
	if (NULL==pPage)
	{
		LOGME("Err, name_slab::kmem_free,NULL==pPage. p address:%x \n", p);
		return;
	}

	CKmem *pKmem = pPage->m_pKmem;
	if(NULL==pKmem)
	{
		LOGME("Err, name_slab::kmem_free,NULL==pKmem, pPage address:%x\n", pPage);
		return ;
	}

	pKmem->Kfree(p);
}

void CWindowsSlabManager::TraceMe()
{
	int nKmemCount = sizeof(name_slab::KmemMallocByte)/sizeof(*name_slab::KmemMallocByte);

	LOGMN("		CWindowsSlabManager TraceMe:\n");
	for (int i=0; i<nKmemCount; ++i)
	{
		LOGMN("KmemID:%d\n", i);
		m_Kmem[i].TraceMe();
	}
}