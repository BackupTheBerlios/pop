#ifndef __SYSCONF_H__
#define __SYSCONF_H__

#ifdef CYGWIN
#define sock_size_t int
#else
#define sock_size_t size_t
#endif

#endif
