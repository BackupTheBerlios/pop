#ifndef __AUTOS_H__
#define __AUTOS_H__

#include "cconfig.h"

#define MAX_AUTOS	20

#define DESCR_NAME	"autos.dat"

typedef struct
{
 char *name;
 int b,h;
 int flags;
 double reifen;
 double ps;
 #ifdef WITH_ALLEGRO
  BITMAP *gfx;
 #endif
} autodaten;

typedef struct
{
 int anz;
 autodaten d[MAX_AUTOS];
} autos;

int autos_load (autos *s, const char *verz);

#endif