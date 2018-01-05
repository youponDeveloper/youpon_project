
#ifndef _H_TIMER_TASK_H_
#define _H_TIMER_TASK_H_

#include "mico.h"
#include "user_main.h"


//定时任务记录
struct timer_task_struct {
	char ctrl_name[32];
	char mac[DEVICE_MAC_ADDRESS_LENGHT];
	unsigned int control_id;
	int timer_inter;
	unsigned int timer_week;
	int action_times;
	unsigned int status;  		//0启动该任务，1停止该任务
	//unsigned char time_type;	//每天执行 0，单次执行1
	unsigned long timer_time_exe;
	int control_type;
	int  func_value;
	int  func_command;
	int  obj_ctrl_id;
};

struct task_struct{
	int tId;
	struct timer_task_struct *tsk;
	struct task_struct *next;
	struct task_struct *prev;
};

void IDMapping(int fd, struct timer_task_struct *pTsk);

void TimerCtrlCallBack(void * pData,int dataSize);

int calcInterval(void *pData,int dataSize);

void loadingTimerTsk(void);

void setTaskFlag(int control_id,char flag);

void delTask(int control_id);

void addTask(struct timer_task_struct  *tsk);

void alterTask(struct timer_task_struct  *tsk);

#endif

