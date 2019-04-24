/***********************************************************
*  File: uni_pointer.h
*  Author: nzy
*  Date: 120427
***********************************************************/
#ifndef _UNI_POINTER_H
#define _UNI_POINTER_H

#include "com_def.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _UNI_POINTER_GLOBAL
    #define _UNI_POINTER_EXT 
#else
    #define _UNI_POINTER_EXT extern
#endif

/***********************************************************
*************************micro define***********************
***********************************************************/
typedef struct list_head 
{
    struct list_head *next, *prev;
}LIST_HEAD,*P_LIST_HEAD;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

// 定义LIST并静态初始化一个空的通用双向链表
#define LIST_HEAD(name) \
LIST_HEAD name = LIST_HEAD_INIT(name)

// 动态初始化一个空的通用双向链表
#define INIT_LIST_HEAD(ptr) do { \
(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

// 动态分配一个包含通用双向链表的结构体
#define NEW_LIST_NODE(type, node) \
{\
    node = (type *)Malloc(sizeof(type));\
}

// 释放链表中的所有节点，使该链表成为空链表
#define FREE_LIST(type, p, list_name)\
{\
    type *posnode;\
    while(!list_empty(&(p)->list_name)) {\
    posnode = list_entry((&(p)->list_name)->next, type, list_name);\
    list_del((&(p)->list_name)->next);\
    Free(posnode);\
    }\
}

#define FREE_RAW_LIST(type,p,list_name)\
{\
    type *posnode;\
    while(!list_empty((p))) {\
    posnode = list_entry((p)->next, type, list_name);\
    list_del((p)->next);\
    Free(posnode);\
    }\
}

// 获取链表中第一个节点地址(该地址指向其主结构)
#define GetFirstNode(type,p,list_name,pGetNode) \
{\
    pGetNode = list_entry((&(p)->list_name)->next, type, list_name);\
}

#define GetFirstNodeRaw(type,p,list_name,pGetNode) \
{\
    pGetNode = list_entry((p)->next, type, list_name);\
}

#define GetLastNode(type,p,list_name,pGetNode)\
{\
    pGetNode = list_entry((&(p)->list_name)->prev, type, list_name);\
}

#define GetLastNodeRaw(type,p,list_name,pGetNode)\
{\
    pGetNode = list_entry((p)->prev, type, list_name);\
}

// 从链中删除某节点，并释放该节点所在结构占用的内存
#define DeleteNodeAndFree(pDelNode,list_name)\
{\
    list_del(&(pDelNode->list_name));\
    Free(pDelNode);\
}

// 仅从链中删除某节点
#define DeleteNode(pDelNode,list_name)\
{\
    list_del(&(pDelNode->list_name));\
}

// 获取包含该通用链表节点的结构体的首址
#define list_entry(ptr, type, member) \
((type *)((char *)(ptr)-(size_t)(&((type *)0)->member)))

// 遍历链表
#define list_for_each(pos, head) \
for (pos = (head)->next; pos != (head); pos = pos->next)

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/
_UNI_POINTER_EXT \
INT list_empty(IN CONST P_LIST_HEAD pHead);

_UNI_POINTER_EXT \
VOID list_add(IN CONST P_LIST_HEAD pNew, IN CONST P_LIST_HEAD pHead);

_UNI_POINTER_EXT \
VOID list_add_tail(IN CONST P_LIST_HEAD pNew, IN CONST P_LIST_HEAD pHead);

_UNI_POINTER_EXT \
VOID list_splice(IN CONST P_LIST_HEAD pList, IN CONST P_LIST_HEAD pHead);

_UNI_POINTER_EXT \
VOID list_del(IN CONST P_LIST_HEAD pEntry);

_UNI_POINTER_EXT \
VOID list_del_init(IN CONST P_LIST_HEAD pEntry);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
