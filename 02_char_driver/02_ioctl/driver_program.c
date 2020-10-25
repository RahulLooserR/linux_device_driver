/* ************************************************************************
 * Created by    : Rahul Kumar Nonia
 * File name     : driver_program.c
 * Created on    : Sunday 25 October 2020 11:35:41 AM IST
 * Last modified : Sunday 25 October 2020 05:53:55 PM IST
 * Description   : 
 * ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE 256


int main()
{
	int fd;
	int option;
	char read_buf[SIZE];
	char write_buf[SIZE];

	printf("This driver program to communicate with driver..\n");

	fd = open("/dev/my_char_device", O_RDWR);
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
				break;

			default:
				printf("wrong choice.. try again \n");
				break;
		}
	}

	return 0;
}
