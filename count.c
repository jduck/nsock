/*
 * count the #defines and spit them out with the $ replaced
 * with their number.
 * 
 * by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>

#define LINE_MATCH 		"define"


int
main(void)
{
   u_char buf[LINE_MAX + 1];
   u_char *nl;
   u_int line = 1;
   u_int def_num = 0;
   
   printf("/* do not modified, automatically generated. modify the .in */\n\n");
   while (fgets(buf, LINE_MAX + 1, stdin))
     {
	nl = strrchr(buf, '\n');
	if (!nl)
	  {
	     fprintf(stderr, "Ack! line %d too long?\n", line);
	     return 1;
	  }
	*nl = '\0';
	
	if (buf[0] == '#'
	    && strstr(buf, LINE_MATCH)
	    && *(nl - 1) == '$')
	  {
	     nl--;
	     *nl = '\0';
	     printf("%s%u\n", buf, def_num++);
	  }
	else
	  printf("%s\n", buf);
	line++;
     }
   return 0;
}
