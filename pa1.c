#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <libgen.h>

#include "pa1_implementation.h"

extern int errno;

    double lambda = (double)1;
	double mu = 0.7;
	int num = 20;
    int s = 10;
	FILE *t = NULL;
	char file_name [50] = {};
	int i = 1;
    int deterministic = 1;
    int trace = 0;
    int exponential = 0;
    char line[100]={};

int main(int argc, char *argv[])
{
	
	/*----------Checking command line and assigning values----------*/
	for(i=1; i<argc; i=i+2)
	{
		if(i+1>=argc)
		{
			fprintf(stderr, "Malformed command line - Arguments do not match with required parameters\n");
        		return 0;
		}
		if(strcmp(argv[i],"-lambda")==0)
		{
			if (sscanf(argv[i+1], "%lf", &lambda) != 1 && argv[i+1][0]!='-')
			{
        		fprintf(stderr, "Malformed command line - Incorrect value of lambda\n");
        		return 0;
    		} 
    		else
    		{
        		lambda = strtod(argv[i+1],NULL);
        		if(lambda<0.00)
        			fprintf(stderr, "Malformed command line - Incorrect value of lambda\n");
        	}
		}
		else if(strcmp(argv[i],"-mu")==0 && argv[i+1][0]!='-')
		{
			if (sscanf(argv[i+1], "%lf", &mu) != 1)
			{
        		fprintf(stderr, "Malformed command line - Incorrect value of mu\n");
        		return 0;
    		} 
    		else
    		{
        		mu = strtod(argv[i+1],NULL);
        		if(mu<0.00)
        			fprintf(stderr, "Malformed command line - Incorrect value of mu\n");
        	}
		}
		else if(strcmp(argv[i],"-det")==0 && argv[i+1][0]!='-')
		{
			if (sscanf(argv[i+1], "%d", &num) != 1)
			{
        		fprintf(stderr, "Malformed command line - Incorrect value of det\n");
        		return 0;
    		} 
    		else
    		{
        		num = atoi(argv[i+1]);
        		if(num<0||num>2147483647)
        			fprintf(stderr, "Malformed command line - Incorrect value of det\n");
        	}
		}
		else if(strcmp(argv[i],"-exp")==0 && argv[i+1][0]!='-')
		{
			if (sscanf(argv[i+1], "%d", &num) != 1)
			{
        		fprintf(stderr, "Malformed command line - Incorrect value of exp\n");
        		return 0;
    		} 
    		else
    		{
        		num = atoi(argv[i+1]);
                deterministic = 0;
                exponential = 1;
        		if(num<0||num>2147483647)
        			fprintf(stderr, "Malformed command line - Incorrect value of exp\n");
        	}
		}
		else if(strcmp(argv[i],"-s")==0 && argv[i+1][0]!='-')
		{
			if (sscanf(argv[i+1], "%d", &s) != 1)
			{
        		fprintf(stderr, "Malformed command line - Incorrect value of s\n");
        		return 0;
    		} 
    		else
    		{
    			s = atoi(argv[i+1]);
    			if(s<0)
    				fprintf(stderr, "Malformed command line - Incorrect value of s\n");
    		}
        }	
		else if(strcmp(argv[i],"-t")==0 && argv[i+1][0]!='-')
		{
			strcpy(file_name,argv[i+1]);
			struct stat stat1;
			stat(argv[2],&stat1);
			t = fopen (argv[i+1], "r");
            deterministic = 0;
            trace = 1;
			/*if(errno)
			{
				fprintf(stderr, "%s\n", strerror(errno));
				return 0;
			}*/
			if(access(argv[i+1],F_OK)!=0)
			{
				fprintf(stderr, "%s\n", strerror(errno));
				return 0;
			}
			else if(S_ISDIR(stat1.st_mode))
			{
				printf("Is a directory, not a file\n");
				return 0;
			}
        	else if(t==NULL)
        	{
        		fprintf(stderr, "Permission denied\n");
        		return 0;
        	}
		}
	}

    if((deterministic==1)&&(trace==1||exponential==1))
    {
        printf("Malformed command line - Not clear which mode to run the system in\n");
        return 0;
    }

    /*----------Printing Simulation Parameters----------*/

    if(deterministic==1)
    {
        printf("Simulation Parameters:\n");
        printf("\tnumber to arrive = %d\n", num);
        printf("\tlambda = %.6g\n", lambda);
        printf("\tmu = %.6g\n", mu);
        printf("\tmode = det\n");
    }
    else if(exponential==1)
    {
        printf("Simulation Parameters:\n");
        printf("\tnumber to arrive = %d\n", num);
        printf("\tlambda = %.6g\n", lambda);
        printf("\tmu = %.6g\n", mu);
        printf("\tmode = exp\n");
        printf("\tseed = %d\n", s);
    }
    else 
    {
        if(fgets ( line, sizeof( line), t )!= NULL)
            num = atoi(line);
        if(!isdigit(line[0]))
        {
            printf("Input file is not in the right format\n");
            exit(0);
        }
        fclose(t);
        printf("Simulation Parameters:\n");
        printf("\tnumber to arrive = %d\n", num);
        char *fname = (char *) basename(file_name);
        printf("\ttsfile = %s\n", fname);
    }

    /*----------Call function to start implementation----------*/

	start_implementation(lambda, mu, num, s, file_name, deterministic, exponential, trace);

	return 0;
}

