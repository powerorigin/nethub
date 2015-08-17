/*
 * queue.h
 *
 * ����
 * 2011.10.8
 */ 


#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

//#define QUEUE_DEBUG
//#define QUEUE_BUS

#define QUEUE_LEN	20	//����ÿ����¼��С
#define QUEUE_CNT	15	//���м�¼����	

struct queue
{
	uint8_t front, rear;		//��ͷ��β�α�
	uint8_t buf[QUEUE_CNT][QUEUE_LEN];
};

typedef struct queue  queue_t;
typedef struct queue *queueP_t;


//������
void Push(queueP_t queue, uint8_t *p);

//�����
char Pop(queueP_t queue, uint8_t *p);

#endif /* QUEUE_H_ */
