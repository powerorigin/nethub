/*
 * queue.c
 *
 * Created: 2011/10/8 9:35:59
 *  Author: AMS-FZC
 */ 

#include "queue.h"
#include <string.h>
#include <stdint.h>
#include "ucos_ii.h"

/*
 * ������:HasData
 * ����:���������Ƿ������
 * ����:
 *		queue:Ҫ���Ķ���
 * ���:
 *		�����������ݷ���1,���򷵻�0
 */
static uint8_t HasData(queueP_t queue)
{
#ifdef QUEUE_DEBUG
	//printf("HasData->front:%d, rear:%d\n", queue->front, queue->rear);
#endif // QUEUE_DEBUG
	
	return ((queue->front) != (queue->rear));
}


/*
 * ������:Push
 * ����:������ջ,β��
 * ����:
 *		queue:Ҫ��ջ�Ķ���
 *		p:Ҫ��ջ������ָ��
 * ���:
 *		��
 */
void Push(queueP_t queue, uint8_t *p)
{
	uint8_t *tempP,i;
	OS_CPU_SR  cpu_sr;


  OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */  
	(queue->rear) %= QUEUE_CNT;		//�����е׷��ص���ͷ*/

#ifdef QUEUE_DEBUG
	printf("push->front:%d, rear:%d\n", queue->front, queue->rear);
#endif // QUEUE_DEBUG

	tempP = queue->buf[(queue->rear)++];
	//for(i=0;i<13;i++)
	//	*tempP++ = *p++;
	memcpy(tempP,p,12);
	OS_EXIT_CRITICAL();
}

/*
 * ������:Push
 * ����:���г�ջ,ͷ��
 * ����:
 *		queue:Ҫ��ջ�Ķ���
 *		p:�����ջ�����ָ��
 * ���:
 *		��ջ�ɹ�����1,ջ�շ���0
 */
char Pop(queueP_t queue, uint8_t *p)
{
	char rtn = 0;
	uint8_t *tempP,i;
	OS_CPU_SR  cpu_sr;

  OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
	if (HasData(queue))
	{
		(queue->front) %= QUEUE_CNT;		//�����е׷��ص���ͷ
		
#ifdef QUEUE_DEBUG
		printf("pop->front:%d, rear:%d\n", queue->front, queue->rear);
#endif // QUEUE_DEBUG
	
		tempP = queue->buf[(queue->front)++];
		memcpy(p, tempP,12);

			//for(i=0;i<13;i++)
		//	  *p++ = *tempP++;
			memset(tempP, '\0', QUEUE_LEN);	//�������
		  OS_EXIT_CRITICAL();
			rtn = 1;

	}
   OS_EXIT_CRITICAL();
	return rtn;
}

