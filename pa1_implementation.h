#ifndef _IMPLEMENTATION_H_
#define _IMPLEMENTATION_H_


typedef struct PacketElement
{
	double store_time;
	int id;
	int flag;
	double s_time;
	double enter_Q;
	double exit_Q;
	double prev_time;
	double dep_time;
}PacketElement;

void start_implementation(double lambda, double mu, int num, int s, char * file_name, int deterministic, int exponential, int trace);

#endif /*_IMPLEMENTATION_H_*/
