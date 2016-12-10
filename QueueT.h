#ifndef QUEUE
#define QUEUE
#include<stdio.h>
//在此处包含进你需要构造的结构体的头文件
#include<stdlib.h>
#define QUEUE_EMPTY 1
#define QUEUE_NULL -1

//请利用全部替换，将T修改成你需要的数据结构
typedef struct _QUEUE_NODE{
	struct _QUEUE_NODE *next;
	_T_ data;
}QueueNode__T_;

typedef struct _QUEUE{
	QueueNode__T_* head;
	QueueNode__T_* tail;
	int count;
}Queue__T_;

void InitQueue(Queue__T_* queue);
void Enqueue(Queue__T_* queue,_T_* pT);
void Dequeue(Queue__T_* queue,_T_* pT);
int IsQueueEmpty(Queue__T_* queue);
void DestroyQueue(Queue__T_* queue);

//初始化队列
void InitQueue(Queue__T_* queue){
        queue->head = (QueueNode__T_ *)malloc(sizeof(QueueNode__T_));
	queue->tail = queue->head;
	queue->head->next = NULL;
	queue->count = 0;
}

//入队列
void Enqueue(Queue__T_* queue,_T_* pT){
	if(IsQueueEmpty(queue) != QUEUE_NULL){
		QueueNode__T_* pQueueNode__T_ =(QueueNode__T_ *) malloc(sizeof(QueueNode__T_));
		pQueueNode__T_->data = *pT;
		pQueueNode__T_->next = NULL;
		queue->tail->next = pQueueNode__T_;
		queue->tail = pQueueNode__T_;
		queue->count++;
	}
}

//出队列
void Dequeue(Queue__T_* queue,_T_* pT){
	if(!IsQueueEmpty(queue)){	
		QueueNode__T_* pDel = queue->head;
		queue->head->next = pDel->next;
		*pT = pDel->data;
		free(pDel);
		queue->count--;
	}
}
//队列是否为空
int IsQueueEmpty(Queue__T_* queue){
	if(queue == NULL){
		printf("Error, The queue doesn't exist!\n");
		return QUEUE_NULL;
	}
	if(queue->count == 0)
		return QUEUE_EMPTY;
	return 0;
}

//删除队列
void DestroyQueue(Queue__T_* queue){
	if(queue !=NULL){
		QueueNode__T_ *pTraverse,*pDel;
		for(pDel=pTraverse=queue->head;pDel!=NULL;pDel = pTraverse){
			pTraverse = pTraverse->next;
			free(pDel);
		}
		free(pTraverse);
	}
}

#endif
