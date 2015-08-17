/*
 * queue.h
 *
 * 队列
 * 2011.10.8
 */ 


#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

//#define QUEUE_DEBUG
//#define QUEUE_BUS

#define QUEUE_LEN	20	//队列每个记录大小
#define QUEUE_CNT	15	//队列记录个数	

struct queue
{
	uint8_t front, rear;		//队头队尾游标
	uint8_t buf[QUEUE_CNT][QUEUE_LEN];
};

typedef struct queue  queue_t;
typedef struct queue *queueP_t;


//出队列
void Push(queueP_t queue, uint8_t *p);

//入队列
char Pop(queueP_t queue, uint8_t *p);

#endif /* QUEUE_H_ */
