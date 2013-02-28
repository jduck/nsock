/*
 * nsock_init()
 * 
 * initialize anything that needs it..
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


u_char nsock_initialized = 0;


void
nsock_init(void)
{
   u_char seeded = 0;
   u_int rand_seed;
#if defined(HAVE_DEV_RANDOM) || defined(HAVE_DEV_URANDOM)
   FILE *fp;
   u_char *rand_file;
#endif
#ifdef HAVE_SSL
   u_char entropy_pool[128];
   u_char ssl_seeded = 0;
   u_int i;
#endif
   
   /* first we must initialize our pseudo-random generators */
#if defined(HAVE_DEV_RANDOM) || defined(HAVE_DEV_URANDOM)
   /* figure out which file to use if we have one */
# if defined(HAVE_DEV_RANDOM)
   rand_file = "/dev/random";
# elif defined(HAVE_DEV_URANDOM)
   rand_file = "/dev/urandom";
# endif
   /* get some random bytes from our pool */
   fp = fopen(rand_file, "r");
   if (fp)
     {
	/* seed the random number generator from the dev */
	if (fread(&rand_seed, sizeof(rand_seed), 1, fp) == 1)
	  {
	     srandom(rand_seed);
	     memset(&rand_seed, 0, sizeof(rand_seed));
	     seeded = 1;
	  }
# ifdef HAVE_SSL
	if (fread(entropy_pool, sizeof(entropy_pool), 1, fp) == 1)
	  {
	     RAND_seed(entropy_pool, sizeof(entropy_pool));
	     ssl_seeded = 1;
	  }
# endif
	fclose(fp);
     }
#endif
   
   /* if we failed to seed any of the required values, lets do crappy seeding here */
   if (!seeded)
     {
	rand_seed = getpid() + time(NULL);
	srandom(rand_seed);
	memset(&rand_seed, 0, sizeof(rand_seed));
     }

#ifdef HAVE_SSL
   /* initialize ssl stuff */
   SSL_load_error_strings();
   SSL_library_init();
   if (!ssl_seeded)
     {
	for (i = 0; i < sizeof(entropy_pool); i++)
	  entropy_pool[i] = (u_char)(random() & 0xff);
	RAND_seed(entropy_pool, sizeof(entropy_pool));
     }
#endif
   
   /* any other initialization stuff shall go here. */
}
