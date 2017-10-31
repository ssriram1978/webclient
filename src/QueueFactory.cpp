/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   QueueFactory.cpp
 * Author: ssridhar
 * 
 * Created on October 31, 2017, 1:06 PM
 */

#include "QueueFactory.h"
#include "one_second_timer.h"
#include "StateFactory.h"

webclient::Queue_Factory* webclient::Queue_Factory::m_pInstance = NULL;

   webclient::Queue_Factory* webclient::Queue_Factory::Instance()
   {       
      if(!webclient::Queue_Factory::m_pInstance)
      {
         webclient::Queue_Factory::m_pInstance = new webclient::Queue_Factory();
      }
      
      return webclient::Queue_Factory::m_pInstance;
   }

webclient::Queue_Factory::~Queue_Factory()
{
    for(int queue_index=0; queue_index < total_number_of_queues; queue_index++)
    {
       msgq_node *pTemp = (p_g_msgQ+queue_index)->pFront;
       while(pTemp)
       {
          msgq_node *pTemp2 =pTemp->pLink;
          free(pTemp);
          pTemp=pTemp2;
       }
       free(p_g_msgQ+queue_index);
    }
}

#define QUEUE_CURRENT_COUNT "current-queue-count-"

void webclient::Queue_Factory::set_total_number_of_queues(uint8_t queue_total)
{
   if(!queue_total)
   {
      VLOG_ERROR("%s:%d Invalid input parameters.\n",__FILE__,__LINE__);
      return;
   }

   p_g_msgQ=(msgQ *)calloc(queue_total,sizeof(msgQ));
   total_number_of_queues=queue_total;
   
   //Register for one second timer event callback.
   for(int count=0; count < total_number_of_queues; count++)
   {
       std::string str_count(QUEUE_CURRENT_COUNT);
       str_count.append(webclient::State_Factory::convert_state_to_name(count));
       
       one_second_timer_factory::Instance()->register_for_one_second_timer(
        str_count,
        webclient::Queue_Factory::Instance()->return_current_queue_count);   
   }
   
}

long webclient::Queue_Factory::return_current_queue_count(void *arg)
{
    std::string *p_queue_name=(std::string *) arg;
    long return_value = -1;
    
    if(!arg)
    {
        VLOG_ERROR("%s:%d Invalid input parameters.\n",__FILE__,__LINE__);
        return return_value;
    }
    
    uint8_t match_found = 0;
    int index=0;
    
    for(index=0; 
            index < webclient::State_Factory::get_total_number_of_states();
            index++)
    {
        std::string str_count(QUEUE_CURRENT_COUNT);
        str_count.append(webclient::State_Factory::convert_state_to_name(index));
        
        if (p_queue_name->compare(str_count) == 0)
        {
            match_found=1;
            break;
        }
    }
    
    if(match_found)
    {
        return_value = webclient::Queue_Factory::Instance()->count(index);
    }
    
    return return_value;
}

/* adds an element to the queue */
uint8_t webclient::Queue_Factory::enqueue(uint8_t queue_type,
                                          void * message,
                                          uint32_t message_size)
{
    msgq_node *temp = NULL ;
    uint8_t msq_send_return_code = -1;

    if(!message ||
       (message_size <=0) ||
       (queue_type <0) ||
       (queue_type >=total_number_of_queues))
    {
       VLOG_ERROR("%s:%d Invalid input parameters.\n",__FILE__,__LINE__);
       return msq_send_return_code;
    }

    if(!(p_g_msgQ+queue_type))
    {
       VLOG_ERROR("%s:%d p_g_msgQ is not initialized for %d.\n",__FILE__,__LINE__,queue_type);
       return msq_send_return_code;
    }
    
    temp = (msgq_node *) calloc(1,sizeof(msgq_node));
    
    if (temp == NULL)
    {
       VLOG_ERROR("%s:%d  Unable to allocate memory for msgq_node\n",__FILE__,__LINE__);
       return msq_send_return_code;
    }
    
    //temp->pData=calloc(message_size, sizeof(uint8_t));
    
    //memcpy(temp->pData,message,message_size);
    
    temp->pData= message;
    
    temp->message_length = message_size;
     
    temp->pLink=NULL;
    
    if (((p_g_msgQ+queue_type)->pFront==NULL) ||
        ((p_g_msgQ+queue_type)->pRear == NULL))
    {
       (p_g_msgQ+queue_type)->pRear=(p_g_msgQ+queue_type)->pFront=temp;
    }
    else
    {
       (p_g_msgQ+queue_type)->pRear->pLink = temp;
       (p_g_msgQ+queue_type)->pRear=(p_g_msgQ+queue_type)->pRear->pLink;
    }
    
    (p_g_msgQ+queue_type)->count++;
    
    msq_send_return_code=0;
    
    return msq_send_return_code;
}

//Dequeue an element from the queue.
void webclient::Queue_Factory::dequeue(uint8_t queue_type, void **ppMessage, uint32_t *pMessageLength)
{
   msgq_node *pTmp = NULL;
   
   if (!ppMessage || !pMessageLength ||
       (queue_type <0) ||
       (queue_type >=total_number_of_queues))
   {
      VLOG_ERROR("%s:%d  Input parameters are invalid.\n",__FILE__,__LINE__);
      return;
   }

   if(!(p_g_msgQ+queue_type))
   {
       VLOG_ERROR("%s:%d p_g_msgQ is not initialized for %d.\n",__FILE__,__LINE__,queue_type);
       return;
   }
    
   if ((p_g_msgQ+queue_type)->pFront==NULL)
   {
      VLOG_ERROR("%s:%d  No messages in messaqe queue\n",__FILE__,__LINE__);
      return;
   }
   
   //memcpy(*ppMessage,(p_g_msgQ+queue_type)->pFront->pData,(p_g_msgQ+queue_type)->pFront->message_length);
   
   *ppMessage= (p_g_msgQ+queue_type)->pFront->pData;
   
   *pMessageLength = (p_g_msgQ+queue_type)->pFront->message_length;

   //free((p_g_msgQ+queue_type)->pFront->pData);
   pTmp = (p_g_msgQ+queue_type)->pFront;
   (p_g_msgQ+queue_type)->pFront = (p_g_msgQ+queue_type)->pFront->pLink;
   free(pTmp);

   (p_g_msgQ+queue_type)->count--;   
}

uint8_t webclient::Queue_Factory::is_empty(uint8_t queue_type)
{
   uint8_t are_there_messages_in_msgQ = 1;
   
   if ((queue_type <0) ||
       (queue_type >=total_number_of_queues))
   {
      VLOG_ERROR("%s:%d  Input parameters are invalid.\n",__FILE__,__LINE__);
      return are_there_messages_in_msgQ;
   }

   if(!(p_g_msgQ+queue_type))
   {
       VLOG_ERROR("%s:%d p_g_msgQ is not initialized for %d.\n",__FILE__,__LINE__,queue_type);
       return are_there_messages_in_msgQ;
   }
   
   if ((p_g_msgQ+queue_type)->pFront !=NULL)
   {
      are_there_messages_in_msgQ = 0;
   }
   
   return are_there_messages_in_msgQ;
}

long webclient::Queue_Factory::count(uint8_t queue_type)
{
    if ((queue_type <0) ||
        (queue_type >=total_number_of_queues))
    {
       VLOG_ERROR("%s:%d  Input parameters are invalid.\n",__FILE__,__LINE__);
       return -1;
    }
    
    if(!(p_g_msgQ+queue_type))
    {
        VLOG_ERROR("%s:%d p_g_msgQ is not initialized for %d.\n",__FILE__,__LINE__,queue_type);
        return -1;
    }
    

    if ((p_g_msgQ+queue_type)->pFront == NULL)
    {
       return -1;
    }
    
    return (p_g_msgQ+queue_type)->count;
}

void webclient::Queue_Factory::peek_at_the_front_of_queue(uint8_t queue_type, 
                                                          void **ppMessage, 
                                                          uint32_t *pMessageLength,
                                                          long location)
{
   if (!ppMessage || !pMessageLength ||
       (queue_type <0) ||
       (location < 0) ||
       (queue_type >=total_number_of_queues))
   {
      VLOG_ERROR("%s:%d  Input parameters are invalid.\n",__FILE__,__LINE__);
      return;
   }

   if(!(p_g_msgQ+queue_type))
   {
       VLOG_ERROR("%s:%d p_g_msgQ is not initialized for %d.\n",__FILE__,__LINE__,queue_type);
       return;
   }
    
   if ((p_g_msgQ+queue_type)->pFront==NULL)
   {
      VLOG_ERROR("%s:%d  No messages in messaqe queue\n",__FILE__,__LINE__);
      return;
   }
   
   int index=location;
   msgq_node *pTmp = (p_g_msgQ+queue_type)->pFront;
   
   while(index)
   {
       pTmp=pTmp->pLink;
       index--;
   }
   
   //memcpy(*ppMessage,pTmp->pData,pTmp->message_length);
   *ppMessage = pTmp->pData;
   
   *pMessageLength = pTmp->message_length;
}
