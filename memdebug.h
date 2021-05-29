/* 一个简易c语言内存调试器
A simple C language memory debuger
by Bai_Tian at *** on May 26 2021 */
#ifndef _MEM_DEBUG_H
#define _MEM_DEBUG_H
/* 相关配置 */

//日志输出的文件
#define _MEM_LOG_PATH "memlog.html"
//调试开关
#define _MEM_DEBUG

//////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef unsigned long long u64;
typedef unsigned char byte;

//获取时间
char _MEM_TIME_buffer[20];
char *_MEM_TIME_FUNC() {
	time_t ct;
	struct tm *cts;
	time( &ct );
	cts = localtime( &ct );
	strftime(_MEM_TIME_buffer, 20, "%Y-%m-%d %H:%M:%S", cts);
	return _MEM_TIME_buffer;
}

#define __CURRENT_TIME__ _MEM_TIME_FUNC()

/*指针偏移计算
@param ptr 指针
@param ofs 偏移 */
#define ptrofs(ptr,ofs) (((u64)ptr)+(ofs))

/*指针转换为字节数组
@param ptr 指针 */
#define byte_array(ptr) ((byte*)ptr)

/*字节遍历器,
使用*b访问内存
@param ptr 内存地址
@param size 内存大小 */
#define byte_iterator(ptr,size) for(byte* b=(byte*)ptr;(u64)b<(u64)ptr+size;++b)

/*void*指针类型转换
@param ptr 指针
@param type 类型 */
#define voidcast(ptr,type) ((type*)ptr)

/* 内存调试 */
#ifdef _MEM_DEBUG
/* 初始化 */
FILE *_MEM_LOG_FILE;
#define memdebug_init() do{ \
		_MEM_LOG_FILE=fopen(_MEM_LOG_PATH,"w"); \
		if(!_MEM_LOG_FILE){ \
			printf("[memdebug]output error!\n"); \
			exit(-1); \
		} \
	}while(0)

/* HTML */

//日志输出
#define _MEM_LOG(content) fprintf(_MEM_LOG_FILE,"<summary>○ [%s] %s (line:%d)</summary>",__CURRENT_TIME__,content,__LINE__);
//表开始
#define _MEM_LOG_TABLE_BEGIN(content) fprintf(_MEM_LOG_FILE,"<details><summary>[%s] %s (line:%d)</summary><table border=\"0\">",__CURRENT_TIME__,content,__LINE__);
//表行开始
#define _MEM_LOG_TABLE_ROW_BEGIN() fprintf(_MEM_LOG_FILE,"<tr>");
//表头
#define _MEM_LOG_TABLE_HEAD(HEX) fprintf(_MEM_LOG_FILE,"<th>%X</th>",HEX);
//表元素
#define _MEM_LOG_TABLE_ELE(HEX) if(HEX<10) fprintf(_MEM_LOG_FILE,"<td>0%X</td>",HEX); else fprintf(_MEM_LOG_FILE,"<td>%X</td>",HEX);
//强调表元素
#define _MEM_LOG_TABLE_ELE_S(HEX) if(HEX<10) fprintf(_MEM_LOG_FILE,"<td bgcolor=\"#cccccc\">0%X</td>",HEX); else fprintf(_MEM_LOG_FILE,"<td bgcolor=\"#cccccc\">%X</td>",HEX);
//表行结束
#define _MEM_LOG_TABLE_ROW_END() fprintf(_MEM_LOG_FILE,"</tr>");
//表结束
#define _MEM_LOG_TABLE_END() fprintf(_MEM_LOG_FILE,"</table></details>");

/*结构体赋值
@param stc 结构体
@param mbr 成员
@param x 值 */
#define set_struct(stc,mbr,x) (stc.mbr)=(x); \
	_MEM_LOG_TABLE_BEGIN( #stc "." #mbr "=" #x) \
	_MEM_LOG_TABLE_ROW_BEGIN() \
	fprintf(_MEM_LOG_FILE,"<th>[%dbytes]</th>",sizeof(stc)); \
	for(int i=0;i<8;++i){ \
		_MEM_LOG_TABLE_HEAD(i) \
	} \
	_MEM_LOG_TABLE_ROW_END() \
	byte_iterator(&stc, sizeof(stc)) {	\
		if ((u64)b % 8 == 0) {	\
			_MEM_LOG_TABLE_ROW_BEGIN() _MEM_LOG_TABLE_HEAD((u64)b)	\
		} \
		if ((u64)b >= (u64) & (stc.mbr) && (u64)b <= (u64) & (stc.mbr) + sizeof(stc.mbr) - 1) {	\
			_MEM_LOG_TABLE_ELE_S(*b)	\
		} else {	\
			_MEM_LOG_TABLE_ELE(*b)	\
		} \
		if ((u64)b % 8 == 7) {	\
			_MEM_LOG_TABLE_ROW_END()	\
		} \
	} \
	_MEM_LOG_TABLE_END()

/*结构体指针赋值
@param stctype 结构体类型
@param ptr 内存指针
@param mbr 成员
@param x 值 */
#define set_structptr(stctype,ptr,mbr,x) (((stctype*)ptr)->mbr)=(x); \
	_MEM_LOG_TABLE_BEGIN( #ptr "->" #mbr "=" #x) \
	_MEM_LOG_TABLE_ROW_BEGIN() \
	fprintf(_MEM_LOG_FILE,"<th>[%dbytes]</th>",sizeof(stctype)); \
	for(int i=0;i<8;++i){ \
		_MEM_LOG_TABLE_HEAD(i) \
	} \
	_MEM_LOG_TABLE_ROW_END() \
	byte_iterator(ptr, sizeof(stctype)) {	\
		if ((u64)b % 8 == 0) {	\
			_MEM_LOG_TABLE_ROW_BEGIN() _MEM_LOG_TABLE_HEAD((u64)b)	\
		} \
		if ((u64)b >= (u64) & ((((stctype *)ptr)->mbr)) && (u64)b <= (u64) & ((((stctype *)ptr)->mbr)) + sizeof((((stctype *)ptr)->mbr)) - 1) {	\
			_MEM_LOG_TABLE_ELE_S(*b)	\
		} else {	\
			_MEM_LOG_TABLE_ELE(*b)	\
		} \
		if ((u64)b % 8 == 7) {	\
			_MEM_LOG_TABLE_ROW_END()	\
		} \
	} \
	_MEM_LOG_TABLE_END()

/*结构体指针输出信息
@param stctype 结构体类型
@param ptr 内存指针
@param mbr 成员 */
#define log_structptr(stctype,ptr,mbr) set_structptr(stctype,ptr,mbr,(stctype*)ptr->mbr)

/*打印字符串
@param ch 字符指针 */
#define log_char(ch) fprintf(_MEM_LOG_FILE,"<summary>○ [%s] " #ch ":%s (line:%d)</summary>",__CURRENT_TIME__,ch,__LINE__)

/*分配堆内存
@param size 分配大小*/
#define malloc(size) malloc(size); _MEM_LOG("malloc(" #size ")")

/*释放堆内存
@param ptr 内存指针*/
#define free(ptr) free(ptr); _MEM_LOG("free(" #ptr ")")

/*输出日志信息*/
#define memdebug_log(content) _MEM_LOG(content)

#else
#define memdebug_init()
#define set_struct(stc,mbr,x) (stc.mbr)=(x)
#define set_structptr(stctype,ptr,mbr,x) (((stctype*)ptr)->mbr)=(x)
#define malloc(size) malloc(size)
#define free(ptr) free(ptr)
#define memdebug_log(content)
#define log_structptr(stctype,ptr,mbr,x)
#endif
#endif
