/* C-Config */

#ifndef __C_CONF__
#define __C_CONF__

//#define USE_MSGS
//#define WITH_ALLEGRO
//#define PAS_RECV

#ifdef WITH_ALLEGRO
 #include <allegro.h>
#endif

#ifdef USE_MSGS
 void Sendmsgs (int group,int num,int arg1,const char *s);
 #define debug_print(s) 	Sendmsgs (1024,0,0,s)
#else
 #define debug_print(s) 	printf("%s\n",s)
#endif

#ifndef __INLINE__
#define __INLINE__ inline
#endif

#endif

