/*
 * Generic list data type
 * Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.16 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_LIST_H
#define GEEKOS_LIST_H

#include <geekos/ktypes.h>
#include <geekos/kassert.h>

/*
 * Define a list type.
 */
#define DEFINE_LIST(listTypeName, nodeTypeName)		\
struct listTypeName {					\
    struct nodeTypeName *head, *tail;			\
}

/*
 * Define members of a struct to be used as link fields for
 * membership in given list type.
 */
#define DEFINE_LINK(listTypeName, nodeTypeName) \
    struct nodeTypeName * prev##listTypeName, * next##listTypeName

/*
 * Define inline list manipulation and access functions.
 */
#define IMPLEMENT_LIST(LType, NType)								\
static __inline__ void Clear_##LType(struct LType *listPtr) {					\
    listPtr->head = listPtr->tail = 0;								\
}												\
static __inline__ bool Is_Member_Of_##LType(struct LType *listPtr, struct NType *nodePtr) {	\
    struct NType *cur = listPtr->head;								\
    while (cur != 0) {										\
	if (cur == nodePtr)									\
	    return true;									\
	cur = cur->next##LType;									\
    }												\
    return false;										\
}												\
static __inline__ struct NType * Get_Front_Of_##LType(struct LType *listPtr) {			\
    return listPtr->head;									\
}												\
static __inline__ struct NType * Get_Back_Of_##LType(struct LType *listPtr) {			\
    return listPtr->tail;									\
}												\
static __inline__ struct NType * Get_Next_In_##LType(struct NType *nodePtr) {			\
    return nodePtr->next##LType;								\
}												\
static __inline__ void Set_Next_In_##LType(struct NType *nodePtr, struct NType *value) {	\
    nodePtr->next##LType = value;								\
}												\
static __inline__ struct NType * Get_Prev_In_##LType(struct NType *nodePtr) {			\
    return nodePtr->prev##LType;								\
}												\
static __inline__ void Set_Prev_In_##LType(struct NType *nodePtr, struct NType *value) {	\
    nodePtr->prev##LType = value;								\
}												\
static __inline__ void Add_To_Front_Of_##LType(struct LType *listPtr, struct NType *nodePtr) {	\
    KASSERT(!Is_Member_Of_##LType(listPtr, nodePtr));						\
    nodePtr->prev##LType = 0;									\
    if (listPtr->head == 0) {									\
	listPtr->head = listPtr->tail = nodePtr;						\
	nodePtr->next##LType = 0;								\
    } else {											\
	listPtr->head->prev##LType = nodePtr;							\
	nodePtr->next##LType = listPtr->head;							\
	listPtr->head = nodePtr;								\
    }												\
}												\
static __inline__ void Add_To_Back_Of_##LType(struct LType *listPtr, struct NType *nodePtr) {	\
    KASSERT(!Is_Member_Of_##LType(listPtr, nodePtr));						\
    nodePtr->next##LType = 0;									\
    if (listPtr->tail == 0) {									\
	listPtr->head = listPtr->tail = nodePtr;						\
	nodePtr->prev##LType = 0;								\
    }												\
    else {											\
	listPtr->tail->next##LType = nodePtr;							\
	nodePtr->prev##LType = listPtr->tail;							\
	listPtr->tail = nodePtr;								\
    }												\
}												\
static __inline__ void Append_##LType(struct LType *listToModify, struct LType *listToAppend) {	\
    if (listToAppend->head != 0) {								\
	if (listToModify->head == 0) {								\
	    listToModify->head = listToAppend->head;						\
	    listToModify->tail = listToAppend->tail;						\
	} else {										\
	    KASSERT(listToAppend->head != 0);							\
	    KASSERT(listToModify->tail != 0);							\
	    listToAppend->head->prev##LType = listToModify->tail;				\
	    listToModify->tail->next##LType = listToAppend->head;				\
	    listToModify->tail = listToAppend->tail;						\
	}											\
    }												\
    listToAppend->head = listToAppend->tail = 0;						\
}												\
static __inline__ struct NType * Remove_From_Front_Of_##LType(struct LType *listPtr) {		\
    struct NType *nodePtr;									\
    nodePtr = listPtr->head;									\
    KASSERT(nodePtr != 0);									\
    listPtr->head = listPtr->head->next##LType;							\
    if (listPtr->head == 0)									\
	listPtr->tail = 0;									\
    else											\
	listPtr->head->prev##LType = 0;								\
    return nodePtr;										\
}												\
static __inline__ void Remove_From_##LType(struct LType *listPtr, struct NType *nodePtr) {	\
    KASSERT(Is_Member_Of_##LType(listPtr, nodePtr));						\
    if (nodePtr->prev##LType != 0)								\
	nodePtr->prev##LType->next##LType = nodePtr->next##LType;				\
    else											\
	listPtr->head = nodePtr->next##LType;							\
    if (nodePtr->next##LType != 0)								\
	nodePtr->next##LType->prev##LType = nodePtr->prev##LType;				\
    else											\
	listPtr->tail = nodePtr->prev##LType;							\
}												\
static __inline__ bool Is_##LType##_Empty(struct LType *listPtr) {				\
    return listPtr->head == 0;									\
}

#endif  /* GEEKOS_LIST_H */
