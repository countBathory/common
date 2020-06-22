#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/signal.h> 
#include <malloc.h>

#define size 256
#define number 6
int p1[2], p2[2];
int *pid_ptr = NULL;

void sighandler(int signum) 
{  
	switch(signum) 
	{ 
		case SIGUSR1:
		{
			char str[size];
			read(p1[0], str, size);
			size_t length = strlen(str);
			write(p2[1], &length, sizeof(size_t));
			close(p1[0]);
			close(p1[1]);
			close(p2[0]);
			close(p2[1]);
			break;
		}
		case SIGUSR2:
		{
			size_t length;
			int pid;
			read(p1[0], &length, sizeof(size_t));
			close(p1[0]);
			close(p1[1]);
			int childCount = length - number;
			printf("Number of processes: %d\n", childCount);
			for (int i = 0; i < childCount; i++)
			{
				pid = fork();
				if (pid == 0)
				{
					printf("Child %d of process %d\n", getpid(), getppid());
					break;
				}
				else 
				{
						write(p2[1], &pid, sizeof(int));
				}
			}
			close(p2[0]);
			close(p2[1]);
			break;
		}
	}
	
} 


 
int main(int argc, char * argv[]) { 
	printf("Main process: %d\n", getpid());

	//set signal handler
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = sighandler;
	sigaction(SIGUSR1, &act, 0);
	sigaction(SIGUSR2, &act, 0);
	
	//create pipes
	if ((pipe(p1)==-1)||(pipe(p2)==-1))
	{
		printf("Creating pipe error!\n");
		return 1;
	}

	
	pid_ptr = (int *)malloc(3 * sizeof(int));
	int processNum = 0;

	for(int i = 0; i < 3; i++)
	{
		int pid = fork();
		if (pid == 0) 
		{
			processNum = i+1;
			break;
		}
		else 
		{
			pid_ptr[i] = pid;
		}
	}

	switch(processNum)
	{
	case 0:
		ParentProcess();
		break;
	case 1:
		printf("Process %d(%d) start working\n", processNum, getpid());
		pause();
		printf("Process %d(%d) end working\n", processNum, getpid());
		break;
	case 2:
		printf("Process %d(%d) start working\n", processNum, getpid());
		pause();
		pause();
		break;
	case 3:
	{
		printf("Process %d(%d) start working\n", processNum, getpid());
		sigset_t newset;
		sigemptyset(&newset);
		sigaddset(&newset, SIGINT);
		sigprocmask(SIG_BLOCK, &newset, 0);
		sleep(15);
		break;
	}
	default: break;
	}
	return 0; 
} 

	



void ParentProcess()
{
	

	//input string
	char str[size];
	printf("Input string: ");
	scanf("%s", str);

	//work with 1st process
	size_t length;
	write(p1[1], str, size);
	kill(pid_ptr[0], SIGUSR1);
	read(p2[0], &length, sizeof(size_t));
	printf("Length of string is: %d\n", length);

	//work with 2nd process
	int *secondChildPids = NULL;
	int childCount = (length - number);
	if (childCount > 0)
	{
		secondChildPids = (int *)malloc(childCount * sizeof(int));
		write(p1[1], &length, sizeof(size_t));
		kill(pid_ptr[1], SIGUSR2);
		for (int i = 0; i < childCount; i++)
			read(p2[0], &(secondChildPids[i]), sizeof(int));
	}

	close(p1[0]);
	close(p1[1]);
	close(p2[0]);
	close(p2[1]);

	sleep(5);
	printf("Sending SIGKILL to the second process(%d) and its childs:\n", pid_ptr[1]);
	kill(pid_ptr[1], SIGKILL);
	for(int i = 0; i < childCount; i++)
	{
		printf("%d. Sending SIGKILL to the process(%d)\n", i+1, secondChildPids[i]);	
		kill(secondChildPids[i], SIGKILL);
	}
	printf("Sending SIGINT to the third process(%d)\n", pid_ptr[2]);
	kill(pid_ptr[2], SIGINT);
	printf("Sending SIGKILL to the third process(%d)\n", pid_ptr[2]);
	while(wait(NULL) != -1);
}

