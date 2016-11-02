/*
 * =====================================================================================
 *
 *       Filename:  ordered_array.h
 *
 *    Description:  oreded_array.h
 *
 *        Version:  1.0
 *        Created:  2015年11月03日 22时34分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  GeekQ wq19911119@gmail.com
 *        Company:  UPC
 *
 * =====================================================================================
 */

#ifndef ORDEREN_ARRAY_H
#define ORDERED_ARRAY_H

#include "common.h"

// void型指针，可以兼容其它的指针类型
typedef void* type_t;

//参数小于后者时候，返回不为0的值，如果大于返回0
typedef int8_t (*lessthan_predicate_t)(type_t, type_t);

typedef struct
{
	type_t *array;
	uint32_t size;
	uint32_t max_size;
	lessthan_predicate_t less_than;
} ordered_array_t;

// a < b 则为1，否则为0
int8_t standard_lessthan_predicate(type_t a, type_t b);

// 创建有序队列
ordered_array_t create_ordered_array(uint32_t max_size, lessthan_predicate_t less_than);
ordered_array_t place_ordered_array(void *addr, uint32_t max_size, lessthan_predicate_t less_than);

// 插入到队列中
void insert_ordered_array(type_t item, ordered_array_t *array);

// 查找i位置的队列项
type_t lookup_ordered_array(uint32_t i, ordered_array_t *array);

// 删掉i位置的数据项
void remove_ordered_array(uint32_t i, ordered_array_t *array);

#endif //ORDERED_ARRAY_H
