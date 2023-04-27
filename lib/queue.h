
/******************************************************************************

                  ��Ȩ���� (C), 2019, �������ĿƼ����޹�˾

 ******************************************************************************
  �� �� ��   : queue.h
  �� �� ��   : V1.0
  ��    ��   : chenmeishu
  ��������   : 2019.9.17
  ��������   : ����ͷ�ļ�    
  �޸���ʷ   :
  1.��    ��   : 
    ��    ��   : 
    �޸�����   : 
******************************************************************************/
#ifndef __QUEUE__H__
#define __QUEUE__H__
#include "sys.h"


#define QUEUE_SIZE   128     //���д�С


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



