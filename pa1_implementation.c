#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "pa1_implementation.h"

#define round(X) (((X)>= 0)?(int)((X)+0.5):(int)((X)-0.5))
#define max(a,b) (((a)>(b))?(a):(b))

double packet_arrival_rate = (double)0;
double service_rate = (double)0;
int number_of_packets = 0;
long seed = 0;
int number_of_current_packets = 0;
FILE *tsfile = NULL;
double packet_arrival_time = (double)0;
double service_time = (double)0;
double previous_time = (double)0;
int server_flag = 0;
double arrival_time = (double)0;
double servicing_time = (double)0;
int number_of_serviced_packets = 0;
double previous_depart_time = (double)0;
double previous_arrival_time = (double)0;
double total_time_in_Q = (double)0;
double total_simulation_time = (double)0;
double total_inter_arrival_time = (double)0;
double total_service_time = (double)0;
double square_time = (double)0;
double system_time = (double)0;
double prev_packet_arrival_time = (double)0;
char l [20]={};
double p1 = (double)0;
double time_in_server = (double)0;
int a=0;
int b=0;
int c=0;
//double arr [501] = {};
//double srr [501] = {};
//char line[100]={};
char packet_arrival_t [10]={};
//char tkns [10]={};
char service_t [10]={}; 
double time_in_system = (double)0;
double prev_service_time = (double)0;
double leave_Q = (double)0;
double prev = (double)0;

/*------------------Exponential Functions-----------------------*/

void InitRandom(long l_seed)
        /*
         * initialize the random number generator
         * if l_seed is 0, will seed the random number generator using the current clock
         * if l_seed is NOT 0, your simulation should be repeatable if you use the same l_seed
         * call this function at the beginning of main() and only once
         */
    {
        if (l_seed == 0L) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);

            srand48(ts.tv_nsec);
        } else {
            srand48(l_seed);
        }
    }

int ExponentialInterval(double rate)
        /*
         * returns an exponentially distributed random interval (in milliseconds) with mean 1/rate
         * rate is either lambda or mu (in packets/second)
         * according to the spec: w = ln(1-r) / (-rate)
         */
    {
        double dval;
        do {
            dval = drand48();
            /* if dval is too small or too large, try again */
        } while (dval < 0.000001 || dval > 0.999999);

        dval = ((double)1.0) - dval;
        dval = -log(dval);
        dval = ((dval / rate) * ((double)1000));

        return round(dval);
    }

/*---------------Queue--------------*/
struct Node 
{
	void *obj;
	struct Node* next; 
};

struct Node* front = NULL;
struct Node* rear = NULL;



void Enqueue(void *object)
{
	struct Node* newnode = (struct Node*)malloc(sizeof(struct Node));
	newnode->obj = object;
 
	if(front == NULL && rear == NULL)
	{
  		newnode->next = NULL;
  		front=newnode;
  		rear=newnode;
	    return;
	}
	else
	{
		  struct Node* temp= front;
		  struct Node* temp_prev= NULL;
		  struct PacketElement* temp_packet=NULL;
		  struct PacketElement* new_packet= (PacketElement *)object;
	  
	  	while(temp!=NULL)
	  	{
	   		temp_packet= (PacketElement *)temp->obj;
	   		if(temp_packet->store_time > new_packet->store_time)
	   		{
	    		if(temp_prev==NULL)
	    		{
	     			newnode->next= front;
	     			front=  newnode;
	     			break;
	    		}
	    		temp_prev->next= newnode;
	    		newnode->next= temp;
	    		break;
	   		}
	   
	   		temp_prev= temp;
	   		temp= temp->next;
	   
	 		 }
	  
	  		if(temp==NULL)
	  		{
	   			temp_prev->next= newnode;
	   			newnode->next=NULL;
	   			rear= newnode;
	  		}
	   
	 	}
}

struct PacketElement * First()
{
	if(front==NULL)
		return NULL;
	else return front->obj;
}

/*---------------------------------Dynamic Printing---------------------------------*/

void Dequeue(struct PacketElement * packet_comp)
{
	while(front!=NULL)
		{
			struct PacketElement * packet;
			packet = First();
			if(packet_comp->store_time > packet->store_time || packet_comp->id == number_of_packets)
			{
			if(packet->flag==0)
			{
				printf("%012.3lfs: p%d arrives and enters Q, inter-arrival time = %.3lfs\n", packet->store_time, packet->id, (packet->store_time-previous_time));
				number_of_current_packets++;
				a++;
				previous_time = packet->store_time;
				//arr[packet->id]=packet->store_time;
				previous_arrival_time = packet->store_time;
				if(packet->id==1)
					system_time = packet->store_time;
				if(server_flag==0&&packet->id==1)
				{
					printf("               p%d leaves Q and begins service at S, time in Q = 0.000s, requesting %.3lfs of service\n", number_of_serviced_packets+1, p1);
					server_flag=1;
				}
			}
			else if(packet->flag==1)
			{
				printf("%012.3lfs: p%d departs from S, service time = %.3lfs, time in system = %.3lfs\n", packet->store_time, packet->id, packet->s_time , (packet->store_time-packet->enter_Q));
				server_flag=0;
				b++;
				previous_depart_time = packet->store_time;
				time_in_server = time_in_server + packet->s_time;
				time_in_system=time_in_system+(packet->store_time-packet->enter_Q);
				square_time = square_time + ((packet->store_time-packet->enter_Q)*(packet->store_time-packet->enter_Q));
				number_of_serviced_packets++;
				if(packet->id==number_of_packets){
					total_simulation_time = total_simulation_time + packet->store_time;
					system_time = packet->store_time - system_time;}
			}
			else
			{
				if(a==b)
				printf("               p%d leaves Q and begins service at S, time in Q = %.3lfs, requesting %.3lfs of service\n", packet->id, (packet->exit_Q-packet->enter_Q), packet->s_time);
				else
					printf("               p%d leaves Q and begins service at S, time in Q = %.3lfs, requesting %.3lfs of service\n", packet->id, (packet->exit_Q-packet->enter_Q), packet->s_time);
				server_flag=1;
				if(a==b)
				total_time_in_Q = total_time_in_Q + (packet->exit_Q-packet->enter_Q);
				else total_time_in_Q = total_time_in_Q + (packet->exit_Q-packet->enter_Q);
			}
			
			//free(packet);
			front=front->next;
		}
		else break;
	
}
	}



/*---------------------Start Implementation--------------------------------*/

void start_implementation(double lambda, double mu, int num, int s, char * file_name, int deterministic, int exponential, int trace)
{
	printf("\n00000000.000s: simulation begins\n");
	number_of_packets = num;
	packet_arrival_rate = lambda;
	service_rate = mu;
	seed = (long)s;
	InitRandom(seed);
	if(trace==1)
	tsfile = fopen(file_name,"r");
	if(tsfile!=NULL)
	{
		
		if(fgets ( l, sizeof( l), tsfile )!= NULL)
			number_of_packets = atoi(l);
		//printf("%d packets\n", number_of_packets);
	}
	
	
/*---------------------------------------------Processing---------------------------------------*/

	if(deterministic==1||exponential==1||trace==1)
	{
		//number_in_queue = 0;
		int i=0;
		packet_arrival_time = (round((1/lambda)*1000.0))/1000.0;
		service_time = (round((1/mu)*1000.0))/1000.0;

		/*------------------Adding Elements to the Queue-------------------*/

		for(i=1;i<=number_of_packets;i++)
		{
			if(exponential==1)
			{
				packet_arrival_time = (ExponentialInterval(packet_arrival_rate))/1000.0;
				service_time = (ExponentialInterval(service_rate))/1000.0;
				//printf("%lf\n", packet_arrival_time);
				//printf("%lf\n", service_time);
				//getchar();
			}

			if(trace==1&&tsfile!=NULL)
			{
				int s = 0;
				if(fgets ( l, sizeof( l), tsfile ) != NULL)
				{	//printf("%s\n", l);
					int j = 0;
					int k = 0;
					memset(packet_arrival_t,0,10);
					memset(service_t,0,10);
					for(s=0;s<strlen(l);s++)
					{	
						if(l[s]!='\t'&&l[s]!=' ')
						{	
							if(j==0)
							{	
								packet_arrival_t[k] = l[s];
								//printf("%s\n", packet_arrival_t);
								k++;
							}	
							else if(j==1) 
							{//printf("hi\n");
								service_t[k] = l[s];
								k++;
							}
						}
						else 
						{
							//printf("hello\n");
							k=0;
							if(strcmp(packet_arrival_t,"")!=0&&strcmp(service_t,"")==0)
								j=1;
						}
					}
					packet_arrival_time = (strtod(packet_arrival_t,NULL))/1000.0;
					service_time = (strtod(service_t,NULL))/1000.0;
				}	
			}
			//if(i<=number_of_packets)
			
			//prev_packet_arrival_time = arrival_time+packet_arrival_time;

			//arr[i]=packet_arrival_time;
			//srr[i]=service_time;

			struct PacketElement * packet;
			packet = (PacketElement*)malloc(sizeof(struct PacketElement));
			packet->id = i;
			total_inter_arrival_time = total_inter_arrival_time + packet_arrival_time;
			arrival_time = arrival_time + packet_arrival_time;
			packet->flag = 0;
			packet->store_time = arrival_time;
			packet->s_time = service_time;
			packet->enter_Q = arrival_time;
			//printf("Enqueue packet %d arrival\n",i);
			Enqueue(packet);

					
			/*if(exponential==1)
				service_time = ExponentialInterval(service_rate);*/

			if(i!=1)
			{
				struct PacketElement * packet2;
				packet2 = (PacketElement*)malloc(sizeof(struct PacketElement));
				packet2->id = i;
				if(packet_arrival_time>prev)
				packet2->store_time = max(arrival_time,prev_service_time);
				else
					packet2->store_time = prev_service_time;
				packet2->s_time = service_time;
				packet2->enter_Q = arrival_time;
				packet2->exit_Q = packet2->store_time;
				leave_Q = packet2->store_time;
				//packet2->enter_Q = servicing_time-prev_packet_arrival_time;
				//packet2->exit_Q = packet2->store_time;
				packet2->flag = 2;
				//printf("Enqueue packet %d begins service\n",i+1);
				Enqueue(packet2);
			}
			else if(i==1)
				p1 = service_time;

			struct PacketElement * packet1;
			packet1 = (PacketElement*)malloc(sizeof(struct PacketElement));
			
			if(i==1)
			servicing_time = servicing_time + service_time + arrival_time;
			else 
			servicing_time = leave_Q+ service_time;
			total_service_time = total_service_time + service_time;
			packet1->id = i;
			packet1->flag = 1;
			packet1->enter_Q = arrival_time;
			packet1->exit_Q = leave_Q;
			packet1->s_time = service_time;
			//packet1->exit_Q = servicing_time;
			packet1->store_time = servicing_time;
			//printf("Enqueue packet %d departs\n",i);
			Enqueue(packet1);

			
			prev_service_time = servicing_time;
			prev = service_time;

			if(i!=1)
				Dequeue(packet);
		}

		
	}

	printf("               simulation ends\n");
	/*------------------------------------Printing Statistics-------------------------------------*/

	printf("\nStatistics:\n\n");
	double avg_Q = total_time_in_Q/total_simulation_time;
	double inter = total_inter_arrival_time/number_of_packets;
	double ser = total_service_time/number_of_packets;
	double spent = time_in_system/number_of_packets;
	double e = square_time/number_of_packets;
	double esq = spent * spent;
	double variance = (double)0;
	variance = e-esq;
	if(e==esq)
		variance = 0;
	double avg_S = time_in_server/total_simulation_time;
	//printf("%lf\n", time_in_system);
	//printf("%lf\n", total_simulation_time);
	//printf("%.6g e\n",e);
	//printf("%.6g esq\n",esq);
	//printf("%.6g variance\n", variance);
	printf("     average packet inter-arrival time = %.6g seconds\n", inter);
	printf("     average packet service time = %.6g seconds\n\n", ser);
	printf("     average number of packets in Q = %.6g packets\n", avg_Q);
	printf("     average number of packets at S = %.6g packets\n\n", avg_S);
	printf("     average time a packet spent in system = %.6g seconds\n", spent);
	printf("     standard deviation for time spent in system = %.6g seconds\n", sqrt(variance));
}

