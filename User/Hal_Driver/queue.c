/*
 * queue.c
 *
 * Created: 2011/10/8 9:35:59
 *  Author: AMS-FZC
 */ 

#include "queue.h"
#include <string.h>
#include <stdint.h>

/*
 * 函数名:HasData
 * 功能:检测队列中是否的数据
 * 输入:
 *		queue:要检测的队列
 * 输出:
 *		队列中有数据返回1,否则返回0
 */
static uint8_t HasData(queueP_t queue)
{
#ifdef QUEUE_DEBUG
	//printf("HasData->front:%d, rear:%d\n", queue->front, queue->rear);
#endif // QUEUE_DEBUG
	
	return ((queue->front) != (queue->rear));
}


/*
 * 函数名:Push
 * 功能:队列入栈,尾进
 * 输入:
 *		queue:要入栈的队列
 *		p:要入栈的数据指针
 * 输出:
 *		无
 */
void Push(queueP_t queue, uint8_t *p)
{
	uint8_t *tempP,i;
	
	(queue->rear) %= QUEUE_CNT;		//到队列底返回到队头*/

#ifdef QUEUE_DEBUG
	printf("push->front:%d, rear:%d\n", queue->front, queue->rear);
#endif // QUEUE_DEBUG

	tempP = queue->buf[(queue->rear)++];
	//for(i=0;i<13;i++)
	//	*tempP++ = *p++;
	memcpy(tempP,p,12);
}

/*
 * 函数名:Push
 * 功能:队列出栈,头出
 * 输入:
 *		queue:要出栈的队列
 *		p:保存出栈数组的指针
 * 输出:
 *		出栈成功返回1,栈空返回0
 */
char Pop(queueP_t queue, uint8_t *p)
{
	char rtn = 0;
	uint8_t *tempP,i;
	
	if (HasData(queue))
	{
		(queue->front) %= QUEUE_CNT;		//到队列底返回到队头
		
#ifdef QUEUE_DEBUG
		printf("pop->front:%d, rear:%d\n", queue->front, queue->rear);
#endif // QUEUE_DEBUG
	
		tempP = queue->buf[(queue->front)++];
		memcpy(p, tempP,12);

			//for(i=0;i<13;i++)
		//	  *p++ = *tempP++;
			memset(tempP, '\0', QUEUE_LEN);	//缓存清空
			rtn = 1;

	}

	return rtn;
}

