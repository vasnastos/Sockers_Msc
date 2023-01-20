#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef linux
#include <netinet/in.h>
#endif
#include <sys/types.h>
#ifdef linux
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif
#include <time.h>
#include <math.h>
#include <assert.h>
#include <signal.h>
#define MAXLINE 1000
#define CLOSEFLAG "exit\n"
#define MEASURE "MEASUREMENTS\n"

typedef struct
{
	char server_ip[255];
	char dedicated[255];
	int port;
	unsigned int experiment_interval;
	unsigned int experiment_duration;
	int sizeof_packages;
}Conf;


typedef struct 
{
	unsigned int number_of_data;
	char **container;
}Data;

void read_data(Data *d)
{
	d->number_of_data=0;
	char line[255];
	FILE *fp=fopen("data.in","r");
	if(fp==NULL)
	{
		perror("File data.in did not open properly");
		return;
	}
	while(fgets(line,255,fp)!=NULL)
	{
		d->number_of_data++;
	}
	fclose(fp);
	if(d->number_of_data==0 || d->number_of_data==1)
	{
		return;
	}

	d->container=(char **)malloc(sizeof(char *)*(d->number_of_data+1));
	for(int i=0;i<d->number_of_data;i++)
	{
		d->container[i]=(char *)malloc(sizeof(char)*255);
	}

	fp=fopen("data.in","r");
	if(fp==NULL)
	{
		perror("File data.in did not open properly");
		return;
	}
	int c=0;
	while(fgets(line,255,fp)!=NULL)
	{
		strcpy(d->container[c++],line);
	}
	fclose(fp);

}

char* ip_address()
{
	int n;
    struct ifreq ifr;
    char array[] = "eth0";
    n = socket(AF_INET, SOCK_DGRAM, 0);
    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , array , IFNAMSIZ - 1);
    ioctl(n, SIOCGIFADDR, &ifr);
    close(n);
	return inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);
}

Conf configure(int argc,char **argv)
{
	int argcounter=1;
	Conf conf; 
	conf.port=0;
	conf.experiment_interval=0;
	conf.experiment_duration=0;
	while(argcounter<argc)
	{
		if(strcmp(argv[argcounter],"--s")==0)
		{
			strcpy(conf.dedicated,"server");
			argcounter++;	
		}
		else if(strcmp(argv[argcounter],"--p")==0)
		{
			conf.port=atoi(argv[++argcounter]);
			argcounter++;
		}	
		else if(strcmp(argv[argcounter],"--c")==0)
		{
			strcpy(conf.dedicated,"client");
			argcounter++;
		}
		else if(strcmp(argv[argcounter],"--a")==0)
		{
			strcpy(conf.server_ip,argv[++argcounter]);
			argcounter++;
		}
		else if(strcmp(argv[argcounter],"--t")==0)
		{
			conf.experiment_duration=atoll(argv[++argcounter]);
			argcounter++;
		}
		else if(strcmp(argv[argcounter],"--l")==0)
		{
			conf.sizeof_packages=atoi(argv[++argcounter]);
			argcounter++;
		}
		else if(strcmp(argv[argcounter],"--i")==0)
		{
			conf.experiment_interval=atoll(argv[++argcounter]);
			argcounter++;
		}
	}
	return conf;
}

void free_data(Data *d)
{
	for(int i=0;i<d->number_of_data;i++)
	{
		free(d->container[i]);
	}
	free(d->container);
}

void print_conf(Conf *c)
{
	// char server_ip[255];
	// char dedicated[255];
	// int port;
	// unsigned int experiment_interval;
	// unsigned int experiment_duration;
	// int sizeof_packages;
	printf("**** Confingurations *****\n");
	printf("Ip adrress:%s\n",c->server_ip);
	printf("Dedicated:%s\n",c->dedicated);
	printf("Port:%d\n",c->port);
	printf("Experiment duration:%d\n",c->experiment_duration);
	printf("Experiment interval:%d\n",c->experiment_interval);
}


void client(Conf conf)
{
	char recvline[MAXLINE - 1];
	int sockfd, n,clientfd;
	char message[255];
	struct sockaddr_in servaddr;
	
	// Random data
	srand(time(NULL));
	Data d;
	read_data(&d);

	
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
		printf("Socket error");
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(conf.port);
	
	if ( inet_pton(AF_INET, conf.server_ip, &servaddr.sin_addr) <= 0)
		printf("inet_pton error for %s", conf.server_ip);

	if ( clientfd=connect(sockfd, (struct sockaddr *) &servaddr,sizeof(servaddr)) < 0 )
		printf("Connection error:%s, TCP PORT:%u",conf.server_ip,conf.port);


	unsigned int start_experiment_timer=time(NULL);
	unsigned int time_since_start;

	while(1) {
		strcpy(message,d.container[rand()%d.number_of_data]);
		send(sockfd, message, strlen(message), 0);
		if((n = read(sockfd, recvline, 1024))<=0) 
			break;
		recvline[n] = '\0';

		// Info in client
		printf("->%s",message);
		printf("$%s\n\n",recvline);

		time_since_start=time(NULL)-start_experiment_timer;
		if(time_since_start>conf.experiment_duration)
		{
			send(sockfd, CLOSEFLAG, strlen(CLOSEFLAG), 0);
			break;
		}

		else if(time_since_start%conf.experiment_interval==0)
		{
			send(sockfd,MEASURE,strlen(MEASURE),0);
		}

		// if(strcmp(message,CLOSEFLAG)==0)
		// {
		// 	break;
		// }

	}
	close(clientfd);
	free_data(&d);
}

void server(Conf conf)
{
	int new_socket,value_read,server_fd,number_of_packages;
	struct sockaddr_in address;
	unsigned int address_len=sizeof(address);
	char buffer[1024]={0};
	int opt=1;
	char server_message[255]="Message response from server";
	unsigned int count_packages=0;
	double data_len=0.0,jitter=0.0,owd=0.0;

	if ((server_fd= socket(AF_INET, SOCK_STREAM, 0))<0)
	printf("Socket error");
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
	{
		printf("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr=inet_addr(conf.server_ip);
	address.sin_port = htons(conf.port);

	// Bind Connections
	if(bind(server_fd,(struct sockaddr*)&address,sizeof(address))<0)
	{
		printf("bind failure");
		exit(EXIT_FAILURE);
	}
	if(listen(server_fd,3)<0)
	{
		printf("Listen error");
		exit(EXIT_FAILURE);
	}
	if((new_socket=accept(server_fd,(struct sockaddr*)&address,(socklen_t*)&address_len))<0)
	{
		printf("Error in accepting connections");
		exit(EXIT_FAILURE);
	}

	printf("Server side: Performance test started\n");
	printf("----------------------------------------------------\n");
	printf("Server listening on TCP PORT %u\n",conf.port);
	printf("----------------------------------------------------\n");

	printf("[ INFO] local %s port %u connected with %s port %u\n",ip_address(),conf.port,conf.server_ip,conf.port);
	printf("[   ID] Interval\tThroughput\tJitter\tOne Way delay\n");
	unsigned int start_package_read_timer,end_package_read_timer,elapsed,elapsed_previous;
	unsigned int start_timer=time(NULL);
	unsigned int end_timer=start_timer;
	unsigned int previous_elapsed_time=0;
	unsigned int previous_time=0;
	while(1)
	{	
		start_package_read_timer=time(NULL);
		value_read=read(new_socket,buffer,1024);
		buffer[value_read]='\0';
		number_of_packages=send(new_socket,server_message,strlen(server_message),0);
		unsigned int end_package_read_timer=time(NULL);
		
		data_len+=strlen(server_message)+value_read;

		
		elapsed=time(NULL)-start_timer;
		if(count_packages>0)
		{
			elapsed_previous=end_package_read_timer-previous_elapsed_time;
			jitter+=elapsed_previous-elapsed;
		}
		previous_elapsed_time=end_package_read_timer;
		owd+=elapsed;
		count_packages++;
		
		if(strcmp(buffer,CLOSEFLAG)==0)
		{
			break;
		}

		if(strcmp(buffer,MEASURE)==0)
		{
			previous_time=end_timer-start_timer;
			end_timer=time(NULL);
			printf("[ INFO] %u - %u\t%.3lfKbps \t%.3lfms \t%.3lfms \n",previous_time,end_timer-start_timer,(data_len*1e-3)/(end_package_read_timer-start_package_read_timer),jitter*1e-3/count_packages,owd*1e-3/count_packages);
		}
	}
	close(new_socket);
	shutdown(server_fd,SHUT_RDWR);
}


int main(int argc, char **argv) {
	Conf conf=configure(argc,argv);
	print_conf(&conf);

	if(strcmp(conf.dedicated,"client")==0)
	{
		client(conf);
	}
	else if(strcmp(conf.dedicated,"server")==0)
	{
		server(conf);
	}	
	exit(0);
}
