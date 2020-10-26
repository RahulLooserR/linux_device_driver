/* ************************************************************************
 * Created by    : Rahul Kumar Nonia
 * File name     : driver_program.c
 * Created on    : Sunday 25 October 2020 11:35:41 AM IST
 * Last modified : Monday 26 October 2020 07:18:10 AM IST
 * Description   : 
 * ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SIZE 256

/* ioctl code */
#define RD_DATA _IOW('a','a',int32_t*)
#define WR_DATA _IOR('a','b',int32_t*)


int main()
{
	int fd;
	int option;
	char read_buf[SIZE];
	char write_buf[SIZE];

	printf("This driver program to communicate with driver through proc fs..\n");

	fd = open("/proc/chr_proc", O_RDWR);
	if(fd < 0){
		printf("Cannot open the device file.\n");
		return -1;
	}

	while(1){
		printf("Enter your choice: \n");
		printf("1. write\n2. read\n3. exit\n");
		scanf("%d", &option);

		switch(option){
			case 1:
				printf("Enter the  string: ");
				//fgets(write_buf, SIZE, stdin);
				scanf("%s", write_buf);
				write(fd, write_buf, strlen(write_buf) + 1);
				printf("Written: %s\n", write_buf);
				break;

			case 2:
				read(fd, read_buf, SIZE);
				printf("Data read: %s\n", read_buf);
				break;

			case 3:
				exit(0);
				close(fd);
				break;

			default:
				printf("wrong choice.. try again \n");
				break;
		}
	}

	return 0;
}
