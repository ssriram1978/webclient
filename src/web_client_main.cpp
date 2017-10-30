#include "namespace.h"
#include "Job.h"
#include "JobFactory.h"
#include "QueueFactory.h"
#include "SchedulerFactory.h"
#include "StateFactory.h"
#include "ThreadFactory.h"
#include "stdin_cb.h"
#include  <signal.h>

unsigned char time_to_exit = 0;

int is_webclient_alive()
{
    return !time_to_exit;
}

void INThandler(int dummy) 
{
    VLOG_ERROR("INThandler: Exiting...\n");
    time_to_exit = 1;
}

void print_job_details(void *p_job_details)
{
    webclient::Job *p_job = (webclient::Job *) p_job_details;
    
    if(!p_job)
    {
        VLOG_ERROR("%s:%s:%d Invalid job details.\n",__FILE__,__FUNCTION__,__LINE__);
        return;
    }
    
    p_job->print_Job();
}

void socket_creator(void *p_job_details)
{
   VLOG_NOTICE("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
   print_job_details(p_job_details);
   
}

void socket_connect(void *p_job_details)
{
   VLOG_NOTICE("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
   print_job_details(p_job_details);
}
    
void socket_writer(void *p_job_details)
{
   VLOG_NOTICE("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
   print_job_details(p_job_details);
}
    
void socket_reader(void *p_job_details)
{
   VLOG_NOTICE("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
   print_job_details(p_job_details);
}
    
void socket_destroyer(void *p_job_details)
{
   VLOG_NOTICE("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
   print_job_details(p_job_details);
}

int hostname_to_ip(const char *hostname, char *service, char *ip, uint32_t ip_len)    
{                                                                               
    //int sockfd;                                                                 
    struct addrinfo hints, *servinfo, *p;                                       
    struct sockaddr_in *h;                                                      
    struct sockaddr_in6 *h6;
    int rv;                                                                     

    memset(&hints, 0, sizeof hints);                                            
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6                  
    hints.ai_socktype = SOCK_STREAM;
    if ( (rv = getaddrinfo(hostname, service, &hints, &servinfo)) != 0)         
    {                                                                           
        VLOG_ERROR("getaddrinfo: %s\n", gai_strerror(rv));                          
        return 1;                                                               
    }                                                                           

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)                                
    {                                                                           
        if(p->ai_family == AF_INET) 
        {                                           
            h = (struct sockaddr_in *) p->ai_addr;
            inet_ntop(p->ai_family, (const void *)&(h->sin_addr.s_addr), ip, (socklen_t)ip_len);
            VLOG_ERROR("Hostname %s Resolved to %s\n", hostname,ip);                                                 
            break;                                                               
        }                                                                       
        else if(p->ai_family == AF_INET6) 
        {
            h6 = (struct sockaddr_in6 *) p->ai_addr;
            inet_ntop(p->ai_family, (const void *)&(h6->sin6_addr.s6_addr[0]), ip, (socklen_t)ip_len);
            VLOG_ERROR("Hostname %s Resolved to %s\n", hostname,ip);                                                 
            break;                                                               
        }                                                                       
        else 
        {
            VLOG_ERROR("Invalid address family\n");
        }                                                                       
    }
    freeaddrinfo(servinfo); // all done with this structure, free 'er up        
    return 0;                                                                   
}

#define DOMAIN_NAME_ADDRESS_MAX 256

uint32_t convert_domain_name_to_ip_address(char *pCedIPAddr)
{
   char nCedIPAddr[256];
   int hname_lkup_rc = 0;
   uint32_t ipaddress = -1;

   if(strlen(pCedIPAddr) < DOMAIN_NAME_ADDRESS_MAX)
   {
        hname_lkup_rc = hostname_to_ip(pCedIPAddr, NULL, nCedIPAddr, sizeof(nCedIPAddr));
        if(hname_lkup_rc != 0) 
        {
            VLOG_ERROR("IP name resolution failure... exiting\n");
            exit(2);
        }
      
        int s = inet_pton(AF_INET, nCedIPAddr, &ipaddress);
        if (s <= 0) 
        {
            if (s == 0)
                VLOG_ERROR("%s:%d ipv4 addr not in presentation format\n",__FILE__,__LINE__);
            else
                perror("inet_pton v4\n");
        }
   }
   return ipaddress;
}

#define STARTING_PORT 1024
#define ENDING_PORT 64000
#define LOCAL_IP 0xfefefefe
#define REMOTE_IP 0xfefe1234

int main(int argc, char **argv)
{
//    signal(SIGINT, INThandler);
    initializeLogParameters("WEBCLIENT");
    char threadName[10] = "MAIN";
    pthread_setname_np(pthread_self(), threadName);
    uint16_t local_starting_port=STARTING_PORT;
    uint16_t local_ending_port=ENDING_PORT;
    uint32_t local_ip_address=LOCAL_IP;
    uint32_t remote_ip_address=REMOTE_IP;
    char c = '\0';
    
    while ((c = getopt(argc, argv, "d:s:e:l:r:t:")) != -1) 
    {
        switch (c) 
        {
            case 'd':
                switch(atoi(optarg))
                {
                    case 0:
                        setLogLevel("DEBUG");
                    break;
                    
                    case 1:
                        setLogLevel("INFO");
                    break;

                    case 2:
                        setLogLevel("NOTICE");
                    break;

                    case 3:
                        setLogLevel("ERROR");
                    break;

                    default:
                        setLogLevel("NOTICE");
                    break;
                }
                break;

            case 's':
                local_starting_port = (unsigned short) atoi(optarg);
                break;

            case 'e':
                local_ending_port = (unsigned short) atoi(optarg);
                break;

            case 'l':
                local_ip_address = convert_domain_name_to_ip_address(optarg);
                break;
            
            case 'r':
                remote_ip_address = convert_domain_name_to_ip_address(optarg);
                break;

            case 'h':
            default:
                VLOG_ERROR("options: [-d <dbglvl>] [-s <starting port>] [-e <ending port>] [-l <local ipaddr>] [-r <remote ip or domain name>]\n");
                exit(EXIT_FAILURE);
                break;
        }
    }
    
    pthread_t pthread_var;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    pthread_create(
               &pthread_var, 
               &attr, 
               &stdin_cb_fn, 
               NULL);

   webclient::Scheduler_Factory::Instance()->initialize(
            local_starting_port,
            local_ending_port,
            local_ip_address,
            remote_ip_address);
   
   webclient::Thread_Factory::Instance()->Initialize_Thread_Factory();   
     
#if 0
   webclient::Job *p_job = (webclient::Job *) calloc(1,sizeof(webclient::Job));
   webclient::uint32_t length = 0;
   webclient::Queue_Factory::Instance()->dequeue(webclient::State_Factory::get_init_state(),
           (void **)&p_job,&length);

   printf("%s:%s:%d pmsg=%p,length=%d\n",__FILE__,__FUNCTION__,__LINE__,
           p_job,length);
   
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
#endif
   sleep(1);
   webclient::Scheduler_Factory::Instance()->run();
   
   while(!time_to_exit)
   {
      sleep(1);
   }
   
   return 1;
}

