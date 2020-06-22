#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <time.h>
int main(int argc, char **argv)
{
	int status;
	struct timeb sys_time;
	struct tm *loc_time;
	pid_t child1, child2;
	if (child1 = fork() == 0)
	{
		printf("Child: %d\n", getpid());
		printf("Parent: %d\n", getppid());
		ftime(&sys_time);
		loc_time = localtime(&sys_time.time);
		printf("%d:%d:%d:%d\n", loc_time->tm_hour, loc_time->tm_min, loc_time->tm_sec, sys_time.millitm);
		exit(0);	
	}
	waitpid(child1, &status, 0);
	printf("exit normally? %s\n", (WIFEXITED(status) ? "true" : "false"));
       	printf("child exitcode = %i\n", WEXITSTATUS(status));
	if ((child2 = fork() == 0))
	{
		printf("Child: %d\n", getpid());
		printf("Parent: %d\n", getppid());
		ftime(&sys_time);
		loc_time = localtime(&sys_time.time);
		printf("%d:%d:%d:%d\n", loc_time->tm_hour, loc_time->tm_min, loc_time->tm_sec, sys_time.millitm);
		exit(0);
	}
	waitpid(child2, &status, 0);
	printf("exit normally? %s\n", (WIFEXITED(status) ? "true" : "false"));
       	printf("child exitcode = %i\n", WEXITSTATUS(status));
	printf("Main process: %d\n", getpid());
	ftime(&sys_time);
	loc_time = localtime(&sys_time.time);
	printf("%d:%d:%d:%d\n", loc_time->tm_hour, loc_time->tm_min, loc_time->tm_sec, sys_time.millitm);
	system("ps -x");
	return 0;
}
