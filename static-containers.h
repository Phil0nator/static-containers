#ifndef STATIC_CONTAINERS_H
#define STATIC_CONTAINERS_H

#include <stdlib.h>

/* VEC */


#define __STC_EXPAND_ITEMS  (32)

/**
 * @brief Static vector capacity
 */
#define STC_VEC_CAP(vect)     vect##_cap

/**
 * @brief Static vector item count
 */
#define STC_VEC_CNT(vect)     vect##_cnt

#define __STC_VEC_EXPAND(vect)\
    STC_VEC_CAP(vect) += __STC_EXPAND_ITEMS;\
    vect = realloc( vect, sizeof(*vect) * STC_VEC_CAP(vect)  )

#define __STC_VEC_ENSURE_SIZE(vect)\
    if ((STC_VEC_CNT(vect) == STC_VEC_CAP(vect))) {  __STC_VEC_EXPAND(vect); }


/**
 * @brief Declare a static vector
 * @param type data type to store
 * @param name name of the static vector
 */
#define STC_VEC_DECL(type, name)\
    type* name = NULL;\
    size_t STC_VEC_CNT(name) = 0;\
    size_t STC_VEC_CAP(name) = 0


/**
 * @brief Push to a static vector
 * @param value value to push
 */
#define STC_VEC_PUSH(vect, value)\
    __STC_VEC_ENSURE_SIZE(vect);\
    vect[STC_VEC_CNT(vect)] = (value);\
    STC_VEC_CNT(vect)++;


/**
 * @brief Free resources held by unused capacity
 *  of a static vector. This will cause the capcity
 *  to be truncated to the item count.
 */
#define STC_VEC_CLEANUP(vect)\
    vect = realloc( vect, sizeof(*vect) * STC_VEC_CNT(vect) );\
    STC_VEC_CAP(vect) = STC_VEC_CNT(vect)


/**
 * @brief Pop from a static vector
 * @returns Either the popped item, or if the vector 
 * is empty, the uninitialized 0th item (Unsafe).
 */
#define STC_VEC_POP(vect)\
    (STC_VEC_CNT(vect)) ? (vect[--STC_VEC_CNT(vect)]) : (vect[0])
    
/**
 * @brief Remove an item from a static vector
 * @param index the index of the item to be removed
 */
#define STC_VEC_REMOVE(vect, index) do {\
        for (size_t __stc_vec_rmv_i = (index); (__stc_vec_rmv_i + 1) < STC_VEC_CNT(vect);__stc_vec_rmv_i++ )\
            vect[__stc_vec_rmv_i] = vect[__stc_vec_rmv_i+1];\
        STC_VEC_CNT(vect) --;\
    } while(0);


/**
 * @brief For loop header for a 'for-each' structure 
 *  over a static vector
 * @param iname name for index iterator
 * @example
 * 
 *  STC_VEC_FOREACH(i, my_vec) {
 *      printf("%d\n", my_vec[i]);
 *  }
 * 
 */
#define STC_VEC_FOREACH(iname, vect)    for(size_t iname = 0; iname < STC_VEC_CNT(vect); iname++)


#define STC_VEC_FREE(vect) free(vect); vect = NULL;\
                            STC_VEC_CAP(vect) = 0; \
                            STC_VEC_CNT(vect) = 0



/* LL */


typedef struct stc_ll_node {
    struct stc_ll_node* next, *prev;
} stc_ll_node_t;

#define STC_LL_CNT(name)   name ## _cnt
#define __STC_LL_HEAD(name) name ## _head
#define __STC_LL_TYPE(name) struct name##_node_t
#define __STC_LL_NODE_CONTENT(name, nodeptr) (((__STC_LL_TYPE(name)*)(nodeptr))->content)
#define __STC_LL_CONTENT_TYPE(name) name##_content_t


#define STC_LL_DECL(type, name) \
    size_t STC_LL_CNT(name) = 0;\
    typedef type __STC_LL_CONTENT_TYPE(name);\
    __STC_LL_TYPE(name) \
        { __STC_LL_TYPE(name) *next, *prev; type content; }* __STC_LL_HEAD(name) = NULL


static stc_ll_node_t* __stc_ll_push( stc_ll_node_t** head, size_t size ) {
    stc_ll_node_t* new_node = (stc_ll_node_t*) calloc(1, size);
    if (*head) {
        stc_ll_node_t* last;
        if ((*head)->prev) last = (*head)->prev;
        else last = *head;
        last->next = new_node;
        new_node->prev = last;
        (*head)->prev = new_node;
    } else *head = new_node;
    return new_node;
}

#define STC_LL_PUSH(name, item)\
        __STC_LL_NODE_CONTENT(name, __stc_ll_push((stc_ll_node_t**) &__STC_LL_HEAD(name), sizeof(item))) = item


static stc_ll_node_t* __stc_ll_pop( stc_ll_node_t** head ) {
    if (*head) {
        stc_ll_node_t* popped;
        if ((*head)->prev) {
            popped = (*head)->prev;
            popped->prev->next = NULL;
            if (popped->prev == (*head)) (*head)->prev = NULL;
            else (*head)->prev = popped->prev;
        } else {
            popped = *head;
            *head = NULL;
        }
        return popped;
    } else return NULL;
}


#define STC_LL_POP(name) free(__stc_ll_pop( (stc_ll_node_t**) &__STC_LL_HEAD(name)))

#define STC_LL_IS_EMPTY(name) (__STC_LL_HEAD(name) ? (1) : (0))

#define STC_LL_FOREACH(name, iname) \
    for (__STC_LL_CONTENT_TYPE(name)* __stc_ll_i = \
            (__STC_LL_CONTENT_TYPE(name)*) __STC_LL_HEAD(name), \
        *iname = &__STC_LL_NODE_CONTENT(name, __stc_ll_i); \
        __stc_ll_i; \
        __stc_ll_i = (__STC_LL_CONTENT_TYPE(name)*) ((stc_ll_node_t*)__stc_ll_i)->next, \
        iname = &__STC_LL_NODE_CONTENT(name,(stc_ll_node_t*) __stc_ll_i) )


#define STC_LL_FIRST(name) __STC_LL_HEAD(name)
#define STC_LL_LAST(name) __STC_LL_NODE_CONTENT(name, (__STC_LL_HEAD(name) ? \
        (__STC_LL_HEAD(name)->prev ? \
        (__STC_LL_HEAD(name)->prev) : (__STC_LL_HEAD(name)))\
    : NULL))
#define STC_LL_PEEK STC_LL_LAST

#define STC_LL_FREE(name) do { \
        stc_ll_node_t* prv = NULL;\
        STC_LL_FOREACH(name, i) { \
            free(prv); prv = i;\
        }\
        free(prv);\
    } while (0)



#endif 