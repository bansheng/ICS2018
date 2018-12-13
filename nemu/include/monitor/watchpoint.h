#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define MAX_EXPR_LENGTH 128

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	char expr[MAX_EXPR_LENGTH];
	int value; //存储表达式的值

} WP;

// 初始化监视点池
void init_wp_pool();

// 新建监视点
WP* new_wp(char* args);

// 删除监视点接口
void del_wp(int index);

// 删除监视点
void free_wp(WP* wp);

// 扫描监视点变化,注意.c文件特意添加了expr.h
bool check_wp();

// 扫描打印监视点
void print_wp();

#endif
