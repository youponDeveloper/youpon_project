#include <stdlib.h>
#include <time.h>

#include "timer_task.h"
#include "queue.h"
#include "timerctrl.h"
#include "xlink_MXCHIP_mico.h"
#include "linkagemanager.h"
#include "timercontrol.h"
#include "user_config.h"


#define Timer_task(M, ...) custom_log("Timer_task", M, ##__VA_ARGS__)


static mico_mutex_t taskLock;
struct task_struct *tskHead=NULL;


#define INSERTTSKMAPPING(tsk) do { \
	if(tskHead==NULL){ \
		tsk->next=tsk;\
		tsk->prev=tsk;\
		tskHead=tsk;   \
	}\
	else { \
		tsk->next=tskHead->next; \
		tskHead->next->prev=tsk; \
		tsk->prev=tskHead; \
		tskHead->next=tsk; \
	}\
	}while(0)
	
#define RMTASKRECORD(tsk) do { \
	if(tsk->next==tsk) tskHead=NULL;\
	else { \
		tsk->prev->next=tsk->next;\
		tsk->next->prev=tsk->prev;\
		if(tsk==tskHead) tskHead=tsk->next;\
	}\
	}while(0)


extern app_context_t*  get_app_context();
extern void setTimerStatus(int nodeFd, int status);		
extern int timer_single_control(struct timer_task_struct * pTsk);

void IDMapping(int fd, struct timer_task_struct *pTsk)
{
	struct task_struct *tsk;
	
	if(pTsk==NULL) return ;
	
	tsk=(struct task_struct *)malloc(sizeof(struct task_struct));
	
	tsk->tId=fd;
	tsk->tsk=pTsk;
	
	INSERTTSKMAPPING(tsk);
}

void TimerCtrlCallBack(void * pData,int dataSize)
{
	struct timer_task_struct * pTsk=(struct timer_task_struct *)pData;

	if(NULL == pTsk)
	{
		Timer_task("TimerCtrlCallBack data is NULL.\n");
		return;
	}

	Timer_task("--------------------TimerCtrlCallBack: %d control_id: %d  pTsk->obj_ctrl_id:%d ---pTsk->timer_inter:%d ------------\n", pTsk->control_type, pTsk->control_id, pTsk->obj_ctrl_id, pTsk->timer_inter);
	if(pTsk->control_type == 1)
	{
		timer_single_control(pTsk);
	
	
	}
	else if(pTsk->control_type == 2)
	{
		Timer_task("scene control here =============");
		timer_scene_control(pTsk);
	}
	
	if(pTsk->timer_week == 0) //stop timer
	{
			Timer_task(" timer_week= 0, stop ===pTsk->control_id:%d==========",pTsk->control_id);
			LINKAGE_CTRL_INFO_T linkageinfo;
			TIMER_CTRL_PARAM_INFO_T timerctrlInfo;
			
			memset(&linkageinfo, 0, sizeof(LINKAGE_CTRL_INFO_T));
			linkageinfo.ctrl_id = pTsk->control_id;
			linkageinfo.status = TIMER_LINKAGE_STATUS_STOP;

			memset(&timerctrlInfo, 0, sizeof(TIMER_CTRL_PARAM_INFO_T));
			timerctrlInfo.ctrl_id = pTsk->control_id;
			timerctrlInfo.status = TIMER_LINKAGE_STATUS_STOP;
			
			//linkage_control_update_status_request(&linkageinfo);
			timer_control_update_status_request(&timerctrlInfo);	

	}
        

}

unsigned long getCurrentTime()    
{    
	struct  tm  *t_tm; 
	unsigned long  hourTimes=0;
	
	mico_utc_time_t utc_time;
	mico_time_get_utc_time( &utc_time );
	t_tm = localtime( (const time_t *)&utc_time);	

	if(t_tm)
	{
		hourTimes =  (t_tm->tm_hour*3600+t_tm->tm_min*60+t_tm->tm_sec);
   		Timer_task("*********** %d %d %d  tv.tv_sec=% ld     ****************\n", t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec, hourTimes);
	}
	
   	return hourTimes;
}

int calcInterval(void *pData,int dataSize)
{
	#define DAY_SECS    (24*60*60)		//һ�������
	
	int interval;
	struct timer_task_struct *pTsk;
	pTsk=(struct timer_task_struct *)pData;

	Timer_task("vvvvvvvvvvvvv calcInterval pTsk->timer_time_exe:%d vvvvvvvvvvvvvvvvv\n", pTsk->timer_time_exe);

	interval=(pTsk->timer_time_exe-(getCurrentTime()%DAY_SECS)+DAY_SECS)%DAY_SECS;

	Timer_task("______   interval: %d %d ____________________\n", interval, pTsk->timer_time_exe);

	if(interval>=0 && interval<=2) interval=DAY_SECS;
	
	return interval;
}

void loadingTimerTsk(void)
{
	int timerFd;
	int i,ret;
	int interval;
	int status;
	uint8_t ctrl_id;
	struct timer_task_struct *pTsk;
	app_context_t* app_context = get_app_context();
	
	mico_rtos_init_mutex( &taskLock);

	if(NULL == app_context)
	{
		Timer_task("app_context is NULL.\n");
		return ;
	}

	for(i=0; i < TIMER_CTRL_LIST_MAX; i++)
	{
		mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);

		status = app_context->userDataStore->userData.timerinfo[i].status;
		ctrl_id = app_context->userDataStore->userData.timerinfo[i].ctrl_id;
		mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

		//Timer_task("ctrl_id=%d status=%d\n", ctrl_id, status);
			
		if((status == STOP_TIMER ||status == START_TIMER ) )
		{
			pTsk=(struct timer_task_struct *)malloc(sizeof(struct timer_task_struct));

			pTsk->control_id=ctrl_id;

			mico_rtos_lock_mutex(&app_context->userDataStore->timerinfo_mutex);
			pTsk->timer_inter=app_context->userDataStore->userData.timerinfo[i].timer_inter;
			
			pTsk->action_times=app_context->userDataStore->userData.timerinfo[i].action_times;
			pTsk->timer_week=app_context->userDataStore->userData.timerinfo[i].timer_week;
			pTsk->control_type = app_context->userDataStore->userData.timerinfo[i].ctrl_type;
			pTsk->obj_ctrl_id = app_context->userDataStore->userData.timerinfo[i].obj_id;
			pTsk->func_command = app_context->userDataStore->userData.timerinfo[i].cmd;
			pTsk->func_value = app_context->userDataStore->userData.timerinfo[i].value;
			pTsk->timer_time_exe=app_context->userDataStore->userData.timerinfo[i].timer_time_exe;
			pTsk->status=app_context->userDataStore->userData.timerinfo[i].status;
			memcpy(pTsk->ctrl_name, app_context->userDataStore->userData.timerinfo[i].ctrl_name, sizeof(app_context->userDataStore->userData.timerinfo[i].ctrl_name));
			memcpy(pTsk->mac, app_context->userDataStore->userData.timerinfo[i].mac, sizeof(app_context->userDataStore->userData.timerinfo[i].mac));
			mico_rtos_unlock_mutex(&app_context->userDataStore->timerinfo_mutex);

			timerFd=allocTimerNodeFd();
			if(timerFd<0) {
				free(pTsk);
				return ;
			}
			else {
				interval=calcInterval(pTsk, sizeof(struct timer_task_struct));
				status=pTsk->status;//(pTsk->status==0 ? START_TIMER : STOP_TIMER);
				Timer_task("timerFd=%d interval=%d  status=%d \n",timerFd, interval, status);
				ret=setTimerNodeParam(timerFd, (void *)pTsk, sizeof(struct timer_task_struct),TimerCtrlCallBack, calcInterval,interval,status);
				if(!ret) {
					IDMapping(timerFd,pTsk);
				}
			}
		}
	}
	
}

void setTaskFlag(int control_id,char flag)
{
	struct task_struct *ptsk;
	
	mico_rtos_lock_mutex(&taskLock);

	flag=(flag==START_TIMER ? START_TIMER : STOP_TIMER);
	
	ptsk=tskHead;

	if(!ptsk) {
		mico_rtos_unlock_mutex(&taskLock);
		return ;
	}

	for( ; ; ) {
		if(ptsk->tsk->control_id==control_id) {
			ptsk->tsk->status=flag;
			setTimerStatus(ptsk->tId,flag);
			break;
		}

		if(ptsk->next==tskHead) break;

		ptsk=ptsk->next;
	}
	
	mico_rtos_unlock_mutex(&taskLock);
}


void delTask(int control_id)
{
	struct task_struct *ptsk;

	mico_rtos_lock_mutex(&taskLock);

	if(!tskHead)  {
		mico_rtos_unlock_mutex(&taskLock);
		return ;
	}
	
	ptsk=tskHead;
	
	Timer_task("!ptsk=%d\n",!ptsk);
	if(!ptsk) {
		mico_rtos_unlock_mutex(&taskLock);
		return ;
	}
	
	for( ; ; ) {
		Timer_task("control_id=%d,ptsk->tsk->control_id=%d\n",control_id,ptsk->tsk->control_id);
		if(ptsk->tsk->control_id==control_id) {
			RMTASKRECORD(ptsk);
			closeTimerNodeFd(ptsk->tId);
			free(ptsk->tsk);
			free(ptsk);
			break;
		}
		
		if(ptsk->next==tskHead) break;

		ptsk = ptsk->next;
	}
	
	mico_rtos_unlock_mutex(&taskLock);
}


void addTask(struct timer_task_struct  *tsk)
{
	int tmfd,status,ret,interval;
	struct timer_task_struct *pTsk;
	
	tmfd=allocTimerNodeFd();
	
	pTsk=(struct timer_task_struct *)malloc(sizeof(struct timer_task_struct));
	
	memcpy(pTsk,tsk,sizeof(struct timer_task_struct));

	Timer_task("control_id=%d,time=%d\n",tsk->control_id,tsk->timer_time_exe);
	
	if(tmfd<0) return;
	else {
		status=pTsk->status;//(pTsk->status==0 ? START_TIMER : STOP_TIMER);
		interval=calcInterval(pTsk, sizeof(struct timer_task_struct));
		ret=setTimerNodeParam(tmfd, (void *)pTsk, sizeof(struct timer_task_struct),TimerCtrlCallBack, calcInterval,interval,status);
		if(!ret) {
			IDMapping(tmfd, pTsk);
		}
	}
}

void alterTask(struct timer_task_struct  *tsk)
{
	int status,interval;
	struct task_struct *pTsk;

	pTsk=tskHead;

	if(!pTsk) return;

	for( ; ; ) {

		if(pTsk->tsk->control_id==tsk->control_id) {

			Timer_task("dddddddddddd  tsk->control_id :%d %d  dddddddddddddd\n", tsk->control_id, tsk->status);
			
			setTimerStatus(pTsk->tId, STOP_TIMER);
			memcpy(pTsk->tsk,tsk,sizeof(struct timer_task_struct));
			status=(tsk->status==START_TIMER ? START_TIMER : STOP_TIMER);
			interval=calcInterval(pTsk->tsk, sizeof(struct timer_task_struct));

			setTimerNodeParam(pTsk->tId, pTsk->tsk, sizeof(struct timer_task_struct), TimerCtrlCallBack,  calcInterval, interval, status);
		}
		
		if(pTsk->next==tskHead) {
			return ;
		}
		else {
			pTsk=pTsk->next;
		}
	}

	return ;
}

void reCalcTime()
{
	struct task_struct *ptsk;

	mico_rtos_lock_mutex(&taskLock);

	if(!tskHead)  {
		mico_rtos_unlock_mutex(&taskLock);
		return ;
	}
	
	ptsk=tskHead;
	
	Timer_task("!ptsk=%d\n",!ptsk);
	if(!ptsk) {
		mico_rtos_unlock_mutex(&taskLock);
		return ;
	}
	
	for( ; ; ) {
		setTimerInterval(ptsk->tId,calcInterval(ptsk->tsk, sizeof(struct timer_task_struct)));
		if(ptsk->next==tskHead) break;
		else {
			ptsk=ptsk->next;
		}
	}
	
	mico_rtos_unlock_mutex(&taskLock);
}


