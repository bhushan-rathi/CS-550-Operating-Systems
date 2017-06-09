#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>

int main(void)
{

	char buffer[350];
	int buffer_reader = 1 ;
	int fd = open("/dev/process_list", O_RDONLY);
	if(fd<0)
	{
		printf("open error");
		exit(1);
	}

	while(buffer_reader >0)
	{
		buffer_reader = read(fd, buffer, sizeof(buffer));
		if(buffer_reader<0)
		{
			printf("read error");
			exit(1);
		}
		printf("\n %s", buffer);
	}
	close(fd);
	return 0;
}
