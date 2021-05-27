/* һ������c�����ڴ������
A simple C language memory debuger
by Bai_Tian at *** on May 26 2021 */
#ifndef _MEM_DEBUG_H 
#define _MEM_DEBUG_H
/* ������� */

//��־������ļ�
#define _MEM_LOG_PATH "memlog.html"
//���Կ���
#define _MEM_DEBUG

//////////////////////////////

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
typedef unsigned long long u64;
typedef unsigned char byte;

//��ȡʱ��
char _MEM_TIME_buffer[20];
char* _MEM_TIME_FUNC(){
    time_t ct;
    struct tm *cts;
    time( &ct );
    cts = localtime( &ct );
    strftime(_MEM_TIME_buffer,20, "%Y-%m-%d %H:%M:%S", cts);
    return _MEM_TIME_buffer;
}

#define __CURRENT_TIME__ _MEM_TIME_FUNC()

/*ָ��ƫ�Ƽ���
@param ptr ָ��
@param ofs ƫ�� */
#define ptrofs(ptr,ofs) (((u64)ptr)+(ofs))

/*ָ��ת��Ϊ�ֽ�����
@param ptr ָ�� */
#define byte_array(ptr) ((byte*)ptr)

/*�ֽڱ�����,
ʹ��*b�����ڴ�
@param ptr �ڴ��ַ
@param size �ڴ��С */
#define byte_iterator(ptr,size) for(byte* b=(byte*)ptr;(u64)b<(u64)ptr+size;++b)

/*void*ָ������ת��
@param ptr ָ��
@param type ���� */
#define voidcast(ptr,type) ((type*)ptr)

/* �ڴ���� */
#ifdef _MEM_DEBUG
    /* ��ʼ�� */
    FILE *_MEM_LOG_FILE;
    #define memdebug_init() do{ \
        _MEM_LOG_FILE=fopen(_MEM_LOG_PATH,"w"); \
        if(!_MEM_LOG_FILE){ \
            printf("[memdebug]output error!\n"); \
            exit(-1); \
        } \
    }while(0)

    /* HTML */
    
    //��־���
    #define _MEM_LOG(content) fprintf(_MEM_LOG_FILE,"<summary>�� [%s] %s at line:%d</summary>",__CURRENT_TIME__,content,__LINE__);
    //��ʼ
    #define _MEM_LOG_TABLE_BEGIN(content) fprintf(_MEM_LOG_FILE,"<details><summary>[%s] %s</summary><table border=\"0\">",__CURRENT_TIME__,content);
    //���п�ʼ
    #define _MEM_LOG_TABLE_ROW_BEGIN() fprintf(_MEM_LOG_FILE,"<tr>"); 
    //��ͷ
    #define _MEM_LOG_TABLE_HEAD(HEX) fprintf(_MEM_LOG_FILE,"<th>%X</th>",HEX); 
    //��Ԫ��
    #define _MEM_LOG_TABLE_ELE(HEX) if(HEX<10) fprintf(_MEM_LOG_FILE,"<td>0%X</td>",HEX); else fprintf(_MEM_LOG_FILE,"<td>%X</td>",HEX); 
    //ǿ����Ԫ��
    #define _MEM_LOG_TABLE_ELE_S(HEX) if(HEX<10) fprintf(_MEM_LOG_FILE,"<td bgcolor=\"#cccccc\">0%X</td>",HEX); else fprintf(_MEM_LOG_FILE,"<td bgcolor=\"#cccccc\">%X</td>",HEX); 
    //���н���
    #define _MEM_LOG_TABLE_ROW_END() fprintf(_MEM_LOG_FILE,"</tr>");
    //�����
    #define _MEM_LOG_TABLE_END() fprintf(_MEM_LOG_FILE,"</table></details>");

    /*�ṹ�帳ֵ
    @param stc �ṹ��
    @param mbr ��Ա
    @param x ֵ */
    #define set_struct(stc,mbr,x) (stc.mbr)=(x); \
        _MEM_LOG_TABLE_BEGIN( #stc "." #mbr "=" #x) \
        _MEM_LOG_TABLE_ROW_BEGIN() \
        fprintf(_MEM_LOG_FILE,"<th>[%dbytes]</th>",sizeof(stc)); \
        for(int i=0;i<8;++i){ \
            _MEM_LOG_TABLE_HEAD(i) \
        } \
        _MEM_LOG_TABLE_ROW_END() \
        byte_iterator(&stc,sizeof(stc)){ \
            if((u64)b%8==0){ _MEM_LOG_TABLE_ROW_BEGIN() _MEM_LOG_TABLE_HEAD((u64)b)} \
            if((u64)b>=(u64)&(stc.mbr)&&(u64)b<=(u64)&(stc.mbr)+sizeof(stc.mbr)-1){_MEM_LOG_TABLE_ELE_S(*b)}else{_MEM_LOG_TABLE_ELE(*b)} \
            if((u64)b%8==7){  _MEM_LOG_TABLE_ROW_END()} \
        } \
        _MEM_LOG_TABLE_END()

    /*�ṹ��ָ�븳ֵ
    @param stctype �ṹ������
    @param ptr �ڴ�ָ��
    @param mbr ��Ա
    @param x ֵ */
    #define set_structptr(stctype,ptr,mbr,x) (((stctype*)ptr)->mbr)=(x); \
        _MEM_LOG_TABLE_BEGIN( #ptr "->" #mbr "=" #x) \
        _MEM_LOG_TABLE_ROW_BEGIN() \
        fprintf(_MEM_LOG_FILE,"<th>[%dbytes]</th>",sizeof(stctype)); \
        for(int i=0;i<8;++i){ \
            _MEM_LOG_TABLE_HEAD(i) \
        } \
        _MEM_LOG_TABLE_ROW_END() \
        byte_iterator(ptr,sizeof(stctype)){ \
            if((u64)b%8==0){ _MEM_LOG_TABLE_ROW_BEGIN() _MEM_LOG_TABLE_HEAD((u64)b)} \
            if((u64)b>=(u64)&((((stctype*)ptr)->mbr))&&(u64)b<=(u64)&((((stctype*)ptr)->mbr))+sizeof((((stctype*)ptr)->mbr))-1){_MEM_LOG_TABLE_ELE_S(*b)}else{_MEM_LOG_TABLE_ELE(*b)} \
            if((u64)b%8==7){  _MEM_LOG_TABLE_ROW_END()} \
        } \
        _MEM_LOG_TABLE_END()

    /*������ڴ�
    @param size �����С*/
    #define malloc(size) malloc(size); _MEM_LOG("malloc(" #size ")")

    /*�ͷŶ��ڴ�
    @param ptr �ڴ�ָ��*/
    #define free(ptr) free(ptr); _MEM_LOG("free(" #ptr ")")
#else
    #define memdebug_init()
    #define set_struct(stc,mbr,x) (stc.mbr)=(x)
    #define set_structptr(stctype,ptr,mbr,x) (((stctype*)ptr)->mbr)=(x)
    #define malloc(size) malloc(size)
    #define free(ptr) free(ptr)
#endif
#endif
