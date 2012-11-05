/*
	@lindp lin_style@foxmail.com   
	@2012/2/24
	@管理CWindowsSlab接口。主要是为了对象生命周期的控制
*/
#pragma  once

#include "Singleton.h"
#include "CWindowsSlab.h"
#include "CLog2.0.h"

/*
	对外暴露的接口
*/
namespace name_slab
{
	void* kmem_malloc(int nSizeByte);
	void kmem_free(void *p);
	void kemem_traceme();
};

class CWindowsSlabManager: public Singleton<CWindowsSlabManager>
{
public:
	CWindowsSlabManager();
	void Init();
	void* KmemMalloc(int nSizeByte);
	void KmemFree(void *p);
	void TraceMe();

private:
	int GetKmemIndex(int nSizeByte);

public:

private:
	CKmem m_Kmem[sizeof(name_slab::KmemMallocByte)/sizeof(*name_slab::KmemMallocByte)];

};

#define g_KmemSlab CWindowsSlabManager::getSingleton()