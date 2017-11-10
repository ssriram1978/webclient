/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   QueueFactory.h
 * Author: ssridhar
 *
 * Created on October 11, 2017, 1:06 PM
 */
#ifndef QUEUE_FACTORY_H
#define QUEUE_FACTORY_H

#include "pipeline_framework.h"

namespace pipeline_framework {

    class Queue_Factory {
    private:

        typedef struct _msgq_node {
            void *pData;
            uint32_t message_length;
            struct _msgq_node *pLink;
        } msgq_node;

        typedef struct _msgQ {
            msgq_node *pFront;
            msgq_node *pRear;
            long count;
            uint8_t is_thread_processing_in_progress;
        } msgQ;

        msgQ *p_g_msgQ;
        uint8_t total_number_of_queues;

        static Queue_Factory* m_pInstance;

        Queue_Factory() {
            p_g_msgQ = NULL;
        }

    public:
        long count(uint8_t queue_type);
        ~Queue_Factory();
        uint8_t enqueue(uint8_t queue_type, void * message, uint32_t message_size);
        void dequeue(uint8_t queue_type, void **ppMessage, uint32_t *pMessageLength);
        uint8_t is_empty(uint8_t queue_type);
        uint8_t is_this_queue_currently_processed_by_a_thread(uint8_t queue_type);
        void set_total_number_of_queues(uint8_t queue_total);
        void peek_at_the_front_of_queue(uint8_t queue_type,
                void **ppMessage,
                uint32_t *pMessageLength,
                long location);
        void set_is_thread_processing_in_progress(
                uint8_t queue_type,
                uint8_t is_thread_processing_in_progress);
        static Queue_Factory* Instance();
        static long return_current_queue_count(void *arg);
    };
}

#endif