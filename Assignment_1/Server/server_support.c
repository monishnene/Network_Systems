#include "server_support.h"
/* 
 * error - wrapper for perror
 */
void error(uint8_t *msg) 
{
	perror(msg);
    	exit(0);
}

uint8_t command_catch(uint8_t* input)
{
	
}
