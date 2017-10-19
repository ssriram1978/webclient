#ifndef Queue_Factory_H
#define Queue_Factory_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;


class Queue_Factory {
  private:
 
  typedef struct _msgq_node
  {
     void *pData;
     uint32_t message_length;
     struct _msgq_node *pLink;
  } msgq_node;
  
  typedef struct _msgQ
  {
     msgq_node *pFront;
     msgq_node *pRear;
     uint64_t count;
  } msgQ;
 
  msgQ *p_g_msgQ;
  uint8_t total_number_of_queues;

  static Queue_Factory* m_pInstance;

  Queue_Factory() {
     p_g_msgQ = NULL;     
  }
  
public:

   ~Queue_Factory();
   uint8_t enqueue(uint8_t queue_type,void * message,uint32_t message_size);
   void  dequeue(uint8_t queue_type,void **ppMessage, uint32_t *pMessageLength);
   uint8_t is_empty(uint8_t queue_type);
   uint64_t count(uint8_t queue_type);
   void set_total_number_of_queues(uint8_t queue_total);
   
   static Queue_Factory* Instance()
   {       
      if(!m_pInstance)
      {
         m_pInstance = new Queue_Factory();
      }
      
      return m_pInstance;
   }
};

#endif

