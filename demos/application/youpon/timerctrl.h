#ifndef _H_TIMECTROL_H_
#define _H_TIMECTROL_H_

#include "mico.h"
#include "queue.h"

#define MAX_TIMER_TASK_NUM        1000 	//最大1000个定时任务

#define TIMER_START	1

#define START_TIMER	1
#define STOP_TIMER		2

// 定时控制相关数据结构
struct timer_node
{
	unsigned int elapse;
	unsigned int interval;
	unsigned int timer_week;
	void *pData;
	int dataSize;
	int flag; 			//启动或停止标识
	
	void (*callBackFun)(void *pData,int dataSize);
	int (*calcInterval)(void *pData,int dataSize);
	
	mico_mutex_t lock;
	
	struct timer_node *le_next;
	
	struct timer_node *le_prev;
};

// 初始化定时器所需运行环境
void initTimer();


// 定时控制相关函数
int allocTimerNodeFd();
// 注意 pData 如果是malloc 出来的，谁申请，谁释放
int  setTimerNodeParam(int nodeFd, void* pData, int dataSize, void (*callBackFun)(void *,int) , int (* calcInterval)(void *,int), int interval, int flag);
void setTimerInterval(int nodeFd, int interval);
int closeTimerNodeFd(int nodeFd);

// 处理到期定时控制的线程
void runTask(mico_thread_arg_t arg);

#endif
