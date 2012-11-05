/*  作者：		林东平                                                              
	创建时间：	2010/07/21                                                         
    功能说明：  随机数
*/
#include <stdlib.h>

int Random(double start, double end)
{
	return (int)(start+(end-start)*rand()/(RAND_MAX + 1.0));
}