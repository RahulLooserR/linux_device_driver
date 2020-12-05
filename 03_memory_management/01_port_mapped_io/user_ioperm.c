/* ************************************************************************
 * Created by    : Rahul Kumar Nonia
 * File name     : user_ioperm.c
 * Created on    : Saturday 05 December 2020 12:57:05 PM IST
 * Last modified : Saturday 05 December 2020 01:30:33 PM IST
 * Description   : accessing io port using ioperm
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

#define BASEPORT 0x200

int main()
{
	char val = 0;
	/* get access to the ports */
	if(ioperm(BASEPORT, 4, 1)){
		perror("ioperm error1");
		exit(1);
	}
	
	// reading a byte from port (inb)
	// writing bye (outb)
	val  = inb(BASEPORT + 1);
	printf("seconds = %x\n", val);

	/* we dont need the port anymore */
	if(ioperm(BASEPORT, 3, 0)){
		perror("ioperm error1");
		exit(1);
	}

	return 0;
}
