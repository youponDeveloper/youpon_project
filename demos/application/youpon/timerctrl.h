#ifndef _H_TIMECTROL_H_
#define _H_TIMECTROL_H_

#include "mico.h"
#include "queue.h"

#define MAX_TIMER_TASK_NUM        1000 	//���1000����ʱ����

#define TIMER_START	1

#define START_TIMER	1
#define STOP_TIMER		2

// ��ʱ����������ݽṹ
struct timer_node
{
	unsigned int elapse;
	unsigned int interval;
	unsigned int timer_week;
	void *pData;
	int dataSize;
	int flag; 			//������ֹͣ��ʶ
	
	void (*callBackFun)(void *pData,int dataSize);
	int (*calcInterval)(void *pData,int dataSize);
	
	mico_mutex_t lock;
	
	struct timer_node *le_next;
	
	struct timer_node *le_prev;
};

// ��ʼ����ʱ���������л���
void initTimer();


// ��ʱ������غ���
int allocTimerNodeFd();
// ע�� pData �����malloc �����ģ�˭���룬˭�ͷ�
int  setTimerNodeParam(int nodeFd, void* pData, int dataSize, void (*callBackFun)(void *,int) , int (* calcInterval)(void *,int), int interval, int flag);
void setTimerInterval(int nodeFd, int interval);
int closeTimerNodeFd(int nodeFd);

// �����ڶ�ʱ���Ƶ��߳�
void runTask(mico_thread_arg_t arg);

#endif
