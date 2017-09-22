#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main()
{
        int fd=0, val=0;
        int buf_size=256;
        char *buffer;
        buffer = (char *)malloc(sizeof(char)*buf_size);
        int bytes =0;

        fd = open("/dev/process_list", O_RDONLY);
        if(fd<0){
        perror("Failed to write the message to the device.");
return errno;}

        printf("ProcessName\t PID\t PPID\t CPU\t STATE\n");
    do
     {
        bytes = read(fd,buffer,buf_size);
		if(bytes ==0 ){
			printf("Error in reading message");
		}
		printf("%s\n", buffer);
     }while(bytes != -1);
//      printf("The received message is: [%s]\n", buffer);
        close(fd);
        free(buffer);
}
