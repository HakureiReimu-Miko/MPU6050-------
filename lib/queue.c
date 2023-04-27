
/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : queue.c
  版 本 号   : V1.0
  作    者   : chenmeishu
  生成日期   : 2019.9.17
  功能描述   : 该文件实现了队列    
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/

#include "sys.h"
#include "queue.h"

void os_memcpy(u8 *dst,u8 *src,u16 msize)
{
		u16 i;
    
	  for(i=0;i<msize;i++)
	  {
	
			dst[i]=src[i];

		}
	
}







/*****************************************************************************
 函 数 名  : InitQueue(QUEUE   *queue_q)
 功能描述  : 初始化队列
 输入参数  :	QUEUE   *queue_q  队列指针
 输出参数  :  无
 修改历史  :
  1.日    期   : 2019年9月23日
    作    者   : chenmeishu
    修改内容   : 创建
*****************************************************************************/ 

void InitQueue(QUEUE *queue_q,void *p)
{
   queue_q->pbuf=p;
   queue_q->front =0; //初始化头尾指针 
   queue_q->rear =0;
    
 }

 /*****************************************************************************
 函 数 名  : u8 isfullQueue(QUEUE *queue_q)
 功能描述  : 判断队列是否满
 输入参数  :	QUEUE   *queue_q  队列指针
 输出参数  :  0  不满     1满
 修改历史  :
  1.日    期   : 2019年9月23日
    作    者   : chenmeishu
    修改内容   : 创建
*****************************************************************************/ 

u8 isfullQueue(QUEUE *queue_q)
 {
     if((queue_q->rear +1)%QUEUE_SIZE == queue_q->front)
     {
         return 1;
     }else
         return 0;
 }

  /*****************************************************************************
 函 数 名  :u8 isemptyQueue(QUEUE *queue_q)
 功能描述  : 判断队列是否空
 输入参数  :	QUEUE   *queue_q  队列指针
 输出参数  :  0  不空     1空
 修改历史  :
  1.日    期   : 2019年9月23日
    作    者   : chenmeishu
    修改内容   : 创建
*****************************************************************************/ 
u8 isemptyQueue(QUEUE *queue_q)
 {
     if(queue_q->front == queue_q->rear)
     {
         return 1;
     }
     else
         return 0;
 }

 
   /*****************************************************************************
 函 数 名  :u8 In_Queue(QUEUE *queue_q , UNIT *pvalue)
 功能描述  : 将消息入队列
 输入参数  :	QUEUE   *queue_q  队列指针，UNIT *pvalue 存放消息的指针
 输出参数  :  0  队列满     1成功
 修改历史  :
  1.日    期   : 2019年9月23日
    作    者   : chenmeishu
    修改内容   : 创建
*****************************************************************************/ 
 
u8 In_Queue(QUEUE *queue_q , void *pvalue,u16 msize)
 {

     if(isfullQueue(queue_q) != 1)        //队列未满
     {			 			    
			   os_memcpy((u8*)(queue_q->pbuf+(queue_q->rear)*msize),(u8*)pvalue,msize);			 								  
         queue_q->rear = (queue_q->rear + 1)%QUEUE_SIZE ;    //尾指针偏移 
			   return 1;
     }
		 return 0;
 }
 
 
/*****************************************************************************
 函 数 名  :u8 In_Queue(QUEUE *queue_q , UNIT *pvalue)
 功能描述  : 从队列取出消息
 输入参数  :	QUEUE   *queue_q  队列指针，HIDMSG *pvalue 存放取出消息的指针
 输出参数  :  0  没有消息     1有消息
 修改历史  :
  1.日    期   : 2019年9月23日
    作    者   : chenmeishu
    修改内容   : 创建
*****************************************************************************/ 
 
u8 Out_Queue(QUEUE *queue_q , void *pvalue,u16 msize)
{  
	    EA=0;
      if(isemptyQueue(queue_q) != 1)        //队列未空
      {				
         os_memcpy((u8*)pvalue,(u8*)(queue_q->pbuf+(queue_q->front)*msize),msize);							
         queue_q->front = (queue_q->front + 1)%QUEUE_SIZE ;					
				 EA=1;
				return 1;
      }			
			EA=1;
			return 0;
 }
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 