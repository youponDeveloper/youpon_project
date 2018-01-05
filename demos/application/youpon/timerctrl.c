#include <time.h>

#include "timerctrl.h"
#include "timer_task.h"
#include "xlink_MXCHIP_mico.h"

#define Timerctrl(M, ...) custom_log("Timerctrl", M, ##__VA_ARGS__)


 struct timer_list
{
	struct timer_node node;
	
	void (*old_sigfunc)(int);			/**< save previous signal handler */
	void (*new_sigfunc)(int);		/**< our signal handler	*/

	uint32_t  timer_interval;
	mico_timer_t  timer;
};

// 执行队列节点的数据结构
struct exc_queue
{
	void *pData;
	int dataSize;
	void (*callBackFun)(void *pData, int dataSize);
	LIST_ENTRY(exc_queue) entries;
};

struct exc_queue_list
{
	LIST_HEAD(queueheader, exc_queue) header;
	mico_mutex_t lock;
};


static mico_mutex_t s_mutexTimerNodes;
static int s_nMaxTimerNodes = 0;
static struct timer_node**	s_apTimerNodes = NULL;

static struct timer_list s_tTimerNodeLst;
static mico_mutex_t s_mutexTimerNodeLst;

static struct exc_queue_list exc_Queue;


#define INSERNODETOTIMERLIST(pNode) do{  \
	s_tTimerNodeLst.node.le_next->le_prev=(pNode); \
	(pNode)->le_next=s_tTimerNodeLst.node.le_next; \
	s_tTimerNodeLst.node.le_next=(pNode); \
	(pNode)->le_prev=&s_tTimerNodeLst.node; \
	}while(0)


#define RMNODEFROMTIMERLIST(node) do{  \
	(node)->le_prev->le_next=(node)->le_next; \
	(node)->le_next->le_prev=(node)->le_prev; \
	}while(0)


struct timer_node * getTimerNode(int nodeFd)
{
	struct timer_node *pNode = NULL;
	
	mico_rtos_lock_mutex(&s_mutexTimerNodes);

	if(nodeFd >= 0  &&  nodeFd < s_nMaxTimerNodes)
	{
		pNode = s_apTimerNodes[nodeFd];
	}

	mico_rtos_unlock_mutex(&s_mutexTimerNodes);
	return pNode;
}

static int timerctrl_func_exec_start(struct timer_node *pTimerNode)
{	
	struct exc_queue *pExecNode;
	//加入执行队列					
	pExecNode = (struct exc_queue *)malloc(sizeof(struct exc_queue));
	if(pExecNode)
	{
		pExecNode->callBackFun = pTimerNode->callBackFun;
		pExecNode->pData = (char *)malloc(pTimerNode->dataSize);
		if(NULL == pExecNode->pData)
		{
			free(pExecNode);
		}
		else
		{
			pExecNode->dataSize = pTimerNode->dataSize;
			memcpy(pExecNode->pData, pTimerNode->pData, pTimerNode->dataSize);
		
			mico_rtos_lock_mutex(&exc_Queue.lock);
			LIST_INSERT_HEAD(&(exc_Queue.header), pExecNode, entries);
			mico_rtos_unlock_mutex(&exc_Queue.lock);
		}
	}
	
	return 0;
}

void sig_func1sec(void* arg)
{
	UNUSED_PARAMETER( arg );	

	struct timer_node *pTimerNode;
	//struct exc_queue *pExecNode;
	int current_week;
	struct tm *timenow;   

	mico_utc_time_t utc_time;

	mico_time_get_utc_time( &utc_time );

	timenow = localtime( (const time_t *)&utc_time);	
	current_week = timenow->tm_wday;

	mico_rtos_lock_mutex(&s_mutexTimerNodeLst);
	
	pTimerNode = s_tTimerNodeLst.node.le_next;

	while(pTimerNode!= &s_tTimerNodeLst.node)
	{
		mico_rtos_lock_mutex(&pTimerNode->lock);

		if(pTimerNode->flag == START_TIMER  &&  pTimerNode->callBackFun) 
		{
			pTimerNode->elapse++;
			//Timerctrl("elapse=%d, interval=%d pTimerNode->timer_week=%d \n", pTimerNode->elapse, pTimerNode->interval, pTimerNode->timer_week);
			
			if(pTimerNode->elapse >= pTimerNode->interval) 
			{
				if(pTimerNode->timer_week == 0)
				{
					timerctrl_func_exec_start(pTimerNode);
				}
				else
				{
					// for week exec function
					switch (current_week) {
					case 1:// week 1
						if ((pTimerNode->timer_week & 1) == 1) {
							timerctrl_func_exec_start(pTimerNode);
						}
						break;
					case 2://week 2
						if ((pTimerNode->timer_week >> 1 & 1) == 1) {
							timerctrl_func_exec_start(pTimerNode);
						}
						break;
					case 3:// week 3
						if ((pTimerNode->timer_week >> 2 & 1) == 1) {
							timerctrl_func_exec_start(pTimerNode);
						}
						break;
					case 4://week 4
						if ((pTimerNode->timer_week >> 3 & 1) == 1) {
							timerctrl_func_exec_start(pTimerNode);
						}
						break;
					case 5:// week 5
						if ((pTimerNode->timer_week >> 4 & 1) == 1) {
							timerctrl_func_exec_start(pTimerNode);
						}
						break;
					case 6://week 6
						if ((pTimerNode->timer_week >> 5 & 1) == 1) {
							timerctrl_func_exec_start(pTimerNode);
						}
						break;
					case 7:// week 7
						if ((pTimerNode->timer_week >> 6 & 1) == 1) {
							timerctrl_func_exec_start(pTimerNode);
						}
						break;

					}
				}

				if(pTimerNode->calcInterval) 
				{
					pTimerNode->elapse = 0;
					pTimerNode->interval = pTimerNode->calcInterval(pTimerNode->pData, pTimerNode->dataSize);						
				}
				else 
				{
					pTimerNode->flag = STOP_TIMER;
				}
				
			}
		}
		
		mico_rtos_unlock_mutex(&pTimerNode->lock);
		
		pTimerNode = pTimerNode->le_next;	
	}

	mico_rtos_unlock_mutex(&s_mutexTimerNodeLst);

	
}


void initTimer()
{
	//int ret;
	//int nSysTimerPrecisionInUs;

	mico_rtos_init_mutex( &s_mutexTimerNodes);
	mico_rtos_init_mutex( &s_mutexTimerNodeLst);
	LIST_INIT(&exc_Queue.header);
	mico_rtos_init_mutex( &exc_Queue.lock);
	
	s_tTimerNodeLst.node.le_next = s_tTimerNodeLst.node.le_prev = &s_tTimerNodeLst.node;

	s_tTimerNodeLst.timer_interval = 1000;
	mico_init_timer( &s_tTimerNodeLst.timer, s_tTimerNodeLst.timer_interval, sig_func1sec, NULL );
	mico_start_timer(&s_tTimerNodeLst.timer);
        
}


int allocTimerNodeFd() 
{
	int i, nFd;
	
	mico_rtos_lock_mutex(&s_mutexTimerNodes);
	nFd = -1;
	for(i = 0; i < s_nMaxTimerNodes; i++)
	{
		if(NULL == s_apTimerNodes[i])
		{
			nFd = i;
			break;
		}
	}

	if(nFd < 0)
	{
		if(s_nMaxTimerNodes < MAX_TIMER_TASK_NUM)
		{
			struct timer_node** apNew = (struct timer_node**)realloc(s_apTimerNodes, (s_nMaxTimerNodes + 100) * sizeof(struct timer_node*));

			if(apNew)
			{
				nFd = s_nMaxTimerNodes;
			
				s_apTimerNodes = apNew;
				for(i = 0; i < 100; i++)
				{
					s_apTimerNodes[s_nMaxTimerNodes++] = NULL;
				}
			}
		}		
	}

	if(nFd >= 0)
	{
		s_apTimerNodes[nFd] = (struct timer_node*)calloc(1, sizeof(struct timer_node));

		if(NULL == s_apTimerNodes[nFd])
		{
			nFd = -1;
		}
		else
		{

			s_apTimerNodes[nFd]->flag = STOP_TIMER;
			s_apTimerNodes[nFd]->pData = NULL;
			s_apTimerNodes[nFd]->calcInterval = NULL;
			s_apTimerNodes[nFd]->callBackFun = NULL;
			s_apTimerNodes[nFd]->interval = 0;
			s_apTimerNodes[nFd]->elapse = 0;
			mico_rtos_init_mutex(&s_apTimerNodes[nFd]->lock);			
			mico_rtos_lock_mutex(&s_mutexTimerNodeLst);
			INSERNODETOTIMERLIST(s_apTimerNodes[nFd]) ;
			mico_rtos_unlock_mutex(&s_mutexTimerNodeLst);
		}		
	}

	mico_rtos_unlock_mutex(&s_mutexTimerNodes);

	return	nFd;	
}


// 注意 pData 如果是malloc 出来的，谁申请，谁释放
int  setTimerNodeParam(int nodeFd, void * pData, int dataSize, void (*callBackFun)(void *,int) , int (* calcInterval)(void *,int) , int interval, int flag)
{
	struct timer_node *pNode;
	
	pNode = getTimerNode(nodeFd);
	
	if(pNode==NULL) return 1;

	mico_rtos_lock_mutex(&(pNode->lock));
	struct timer_task_struct *pTask = (struct timer_task_struct *)pData;

	pNode->pData = pData;
	pNode->dataSize = dataSize;
	pNode->callBackFun = callBackFun;
	pNode->calcInterval = calcInterval;
	pNode->timer_week = pTask->timer_week;
	pNode->interval = interval;
	pNode->elapse = 0;
	pNode->flag = flag;
	
	mico_rtos_unlock_mutex(&(pNode->lock));
	
	return 0;
}


int closeTimerNodeFd(int nodeFd)
{
	struct timer_node *pNode;

	mico_rtos_lock_mutex(&s_mutexTimerNodes);
	if(nodeFd < 0  ||  nodeFd >= s_nMaxTimerNodes)
	{
		mico_rtos_unlock_mutex(&s_mutexTimerNodes);

		return	-1;
	}

	pNode = s_apTimerNodes[nodeFd];
	s_apTimerNodes[nodeFd] = NULL;
	mico_rtos_unlock_mutex(&s_mutexTimerNodes);

	if(pNode)
	{
		mico_rtos_lock_mutex(&s_mutexTimerNodeLst);	
		RMNODEFROMTIMERLIST(pNode);
		mico_rtos_unlock_mutex(&s_mutexTimerNodeLst);

		free(pNode);
	}

	return	0;
}


void setTimerStatus(int nodeFd, int status)
{
	struct timer_node *pNode;

	pNode = getTimerNode(nodeFd);
	
	if(!pNode) return ;
	
	Timerctrl("status=%d\n", status);
	
	mico_rtos_lock_mutex(&pNode->lock);
	
	if(STOP_TIMER == status || START_TIMER == status) 
	{
		pNode->flag = status;

		Timerctrl("pNode->flag=%d\n", pNode->flag);

		if(status == START_TIMER) 
		{			
			if(pNode->calcInterval) 
			{
				pNode->elapse = 0;
				pNode->interval = pNode->calcInterval(pNode->pData, pNode->dataSize);
			}
		}
	}
	
	mico_rtos_unlock_mutex(&pNode->lock);
}


void setTimerInterval(int nodeFd, int interval)
{
	struct timer_node *pNode;
	
	pNode = getTimerNode(nodeFd);

	if(!pNode) return ;

	mico_rtos_lock_mutex(&pNode->lock);
	
	pNode->elapse = 0;
	pNode->interval=interval;
	
	mico_rtos_unlock_mutex(&pNode->lock);
}


int timerStatus(int fd)
{
	int	nRet = 0;
	
	struct timer_node *pNode;
	
	pNode = getTimerNode(fd);

	if(pNode)
	{
		nRet = pNode->flag;
	}

	return	nRet;
}

void runTask(mico_thread_arg_t arg)
{
	
	struct exc_queue *pNode = NULL;
	
	while(1) 
	{
		while(1) 
		{
			mico_rtos_lock_mutex(&exc_Queue.lock);
			
			if(exc_Queue.header.lh_first == NULL) 
			{
				//Timerctrl("runTask1 \n");
				mico_rtos_unlock_mutex(&exc_Queue.lock);
				break;
			}

				Timerctrl("runTask eee \n");
			
			pNode = exc_Queue.header.lh_first;
			
			LIST_REMOVE(pNode, entries);
			
			mico_rtos_unlock_mutex(&exc_Queue.lock);
			
			(pNode->callBackFun)(pNode->pData, pNode->dataSize);
			
			if(pNode->pData)
			{
				free(pNode->pData);
				pNode->pData = NULL;
			}	
			if(pNode){
				free(pNode);
				pNode = NULL;
			}
		}
		mico_rtos_thread_sleep(2);
	}

	mico_rtos_delete_thread(NULL);
}
