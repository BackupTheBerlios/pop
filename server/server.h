#ifndef __SERVER_H__
#define __SERVER_H__

#include "message.h"

// Some global vars... Should get rid of these
class level;
extern level al;
extern serverchan *sc;
extern volatile unsigned int timepos;
class autotypen;
extern autotypen *typen;
extern int starttime;

#endif
