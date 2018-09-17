#include "client_support.h"
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
	if(!strncmp(input,get_str,strlen(get_str)))
	{
		input += strlen(get_str)+1;
		syslog(SYSLOG_PRIORITY,"%s",input);
		strcpy(filename,input);
		syslog(SYSLOG_PRIORITY,"%s",filename);
		return get;
	}
	return 0;
}
