/*  作者：		林东平                                                              
	创建时间：	2010/08/08                                                        
    功能说明：  linux下的链表，一些功能因为windows平台关系，有进行删改
				vc2008测试下通过
*/

#pragma once
#include <stddef.h> //offset

#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)
/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#ifndef CONFIG_DEBUG_LIST
static inline void __list_add(struct list_head *_new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = _new;
	_new->next = next;
	_new->prev = prev;
	prev->next = _new;
}
#else
extern void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next);
#endif

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head, head->next);
}


/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *_new, struct list_head *head)
{
	__list_add(_new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
#ifndef CONFIG_DEBUG_LIST
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = (list_head*)LIST_POISON1;
	entry->prev = (list_head*)LIST_POISON2;
}
#else
extern void list_del(struct list_head *entry);
#endif


static inline list_head* __list_get(struct list_head *head, struct list_head *pos)
{
	if( list_empty(head) )
	{
		return NULL;
	}
	
	list_del(pos);
	return pos;
}

/*
	取得头部第一个元素，并且删除
*/
static inline list_head* list_get(struct list_head *head)
{
	return __list_get(head, head->next);
}

/*
	取得尾部第一个元素，并且删除
*/
static inline list_head* list_get_tail(struct list_head *head)
{
	return __list_get(head, head->prev);
}

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace(struct list_head *old,
				struct list_head *_new)
{
	_new->next = old->next;
	_new->next->prev = _new;
	_new->prev = old->prev;
	_new->prev->next = _new;
}

static inline void list_replace_init(struct list_head *old,
					struct list_head *_new)
{
	list_replace(old, _new);
	INIT_LIST_HEAD(old);
}

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); \
        	pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)


/*
	VC下不支持typeof关键字和一些语法解析，另外封装个
*/
 #define list_entry_offset(ptr, type, member) (			\
				(type *)( (char *)ptr - offsetof(type,member) ) )


/*
	受不了了，随意封装个带计数的链表结构
*/
class CList
{
public:
	CList()
	{
		m_nListCount= 0 ;
		INIT_LIST_HEAD(&m_lHead);		
	}

	unsigned int GetCount()
	{
		return m_nListCount;
	}

	void Add(list_head *p)
	{
		++m_nListCount;
		list_add(p, &m_lHead);
	}

	void Del(list_head *p)
	{
		--m_nListCount;
		list_del(p);
	}

	/*
		如果你想要有计数功能，请在增、删时使用Add,Del接口
	*/
	struct list_head m_lHead;

private:
	unsigned int m_nListCount;	
};


/* demo
class T

public:
	T()
	{
		INIT_LIST_HEAD(&m_list);
	}
	int i;
	struct list_head m_list;
};
				  

int _tmain(int argc, _TCHAR* argv[])
{
	T tHead;
	T *pTDel;

	for(int i=1; i<10; ++i)
	{
		T *pT = new T;
		pT->i = i;
		list_add(&pT->m_list, &tHead.m_list);
		if( 5==i )
		{
			pTDel = pT;
		}
	}
	
	list_del(&pTDel->m_list);

	T *pT;
	list_head *pEach;
	list_for_each(pEach, &tHead.m_list)
	{
		pT = list_entry_offset(pEach, T, m_list);		
		printf("%d\n", pT->i);
	}

	return 0;
}


*/