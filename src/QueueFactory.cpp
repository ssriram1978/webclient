#include "QueueFactory.h"

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


void webclient::Queue_Factory::set_total_number_of_queues(uint8_t queue_total)
{
   if(!queue_total)
   {
      VLOG_ERROR("%s:%d Invalid input parameters.\n",__FILE__,__LINE__);
      return;
   }

   p_g_msgQ=(msgQ *)calloc(queue_total,sizeof(msgQ));
   total_number_of_queues=queue_total;
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
    
    temp->pData=calloc(message_size, sizeof(uint8_t));
    
    memcpy(temp->pData,message,message_size);
    
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
   
   memcpy(*ppMessage,(p_g_msgQ+queue_type)->pFront->pData,(p_g_msgQ+queue_type)->pFront->message_length);
   *pMessageLength = (p_g_msgQ+queue_type)->pFront->message_length;

   free((p_g_msgQ+queue_type)->pFront->pData);
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

webclient::uint64_t webclient::Queue_Factory::count(uint8_t queue_type)
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

