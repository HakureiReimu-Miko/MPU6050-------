
/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : queue.h
  版 本 号   : V1.0
  作    者   : chenmeishu
  生成日期   : 2019.9.17
  功能描述   : 队列头文件    
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/
#ifndef __QUEUE__H__
#define __QUEUE__H__
#include "sys.h"


#define QUEUE_SIZE   128     //队列大小


typedef struct Queue
 {
     void *pbuf;
     u8 front;
     u8 rear;
 }QUEUE;

 void InitQueue(QUEUE *queue_q,void *p);
 u8 isfullQueue(QUEUE *queue_q);
 u8 isemptyQueue(QUEUE *queue_q);
 u8 In_Queue(QUEUE *queue_q ,  void *pbufp,u16 msize); 
 u8 Out_Queue(QUEUE *queue_q , void *pbufp,u16 msize); 
void os_memcpy(u8 *dst,u8 *src,u16 msize);
 
#endif



