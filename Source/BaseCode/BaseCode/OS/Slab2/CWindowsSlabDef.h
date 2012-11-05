#pragma  once

//lindp
//2010.7.17 2:51

/*
	CONFIG_SLAB_DEBUG	:总开关
	DEBUG_DEFAULT_FALGS :需要跟踪的具体DEBUG标签
*/
#define CONFIG_SLAB_DEBUG


#ifdef CONFIG_SLAB_DEBUG

	#define DEBUG_SLAB_FALGS (SLAB_RED_ZONE | SLAB_CPU_RUN_ID | SLAB_GC_PARTIAL )

#else
	
	#define DEBUG_DEFAULT_FALGS 0

#endif 

/*
	需要调试的DEBUG标签，递增顺序为
	0x1,0x2,0x4,0x8
	0x10,0x20,0x40,0x80
	0x100,0x200,0x300,0x400
	...
*/
#define SLAB_RED_ZONE 0x0			//0x1	开启坏区检测
#define SLAB_CPU_RUN_ID 0x0		//0x2	开启多核CPU缓存,即尽量保证切换时，在使用上不会出现脏的页面需要重刷
#define SLAB_GC_PARTIAL 0x0			//0x4	开启多余页面回收

//  |red|haed address|[list]content|
/*
	配置变量
*/
struct VIR_PAGE_HEAD;
namespace name_slab
{
	//kmem分配对象的大小,单位byte.最小4byte,需要的话可以在这里添加。
	const int KmemMallocByte[]=
	{8,16,32,64,128,512,1024, 2048,8192,16384,32768};

	//坏区检测的魔术数字
	const unsigned long HEAD_BAD_ALIEN_MAGIC = 0x01020304;

	//预留的page地址大小
	const long HEAD_PAGE_ADDR_SIZE = sizeof(VIR_PAGE_HEAD*);

	//partial要保留的页面数，该参数可以调节
	const int FREE_KEEP_PARTIAL=30;

	//计算对象头大小
	long CalcSingleObjectHeadSize();
	//计算对象大小
	long CalcSingleObjectSize(long nUseObjectSize);

	//访问到页面头地址
	VIR_PAGE_HEAD* OffsetPageHead(void *p);

	//访问到页面魔术数字
	unsigned int OffsetBadMagic(void *p);

	//访问当前CPUID
	int GetCPURunId();
};


