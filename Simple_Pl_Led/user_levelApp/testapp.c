/* Abushaqra - October - 16 - 2021 - created, This is a userlvlapp to test the functionality of the ip core */
/* This User-Level-App */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h> 	
#include <unistd.h>    		
#include <time.h>
#include <sys/ioctl.h>	
#include <sys/sendfile.h>
#include <sys/mman.h>

#include "pl-led-driver.h"

int main ( void ) {
	
	unsigned char simple_ipcore_dev_node[56] = "/dev/PL_LED";

	unsigned int i = 0; 
	unsigned int timer = 0;
    
	int simple_ipcore_handle = open (simple_ipcore_dev_node , O_RDWR);
	
	if ( simple_ipcore_handle < 0 )
    {
		printf ("user_level_app : could not open file handles to dev nodes fine. Are all nodes present?\n");
		exit (-1);
	}
	
	//*******************************************************************************************************//
	
	ioctl ( simple_ipcore_handle, IOCTL_LEDS_OFF );
    
	printf ("\n... Starting Test App for the PL-Leds, press enter to switch on the Led ...\n");
	getchar();
	ioctl ( simple_ipcore_handle, IOCTL_LEDS_ON );

    printf("================================================\n");
	
	printf ("Press enter to switch off the PL-Leds..\n");
	getchar();
    ioctl ( simple_ipcore_handle, IOCTL_LEDS_OFF );
    
     
}



