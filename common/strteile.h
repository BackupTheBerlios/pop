// strteile.h

#ifndef __STRTEILE_H__
#define __STRTEILE_H__

#include "cconfig.h"

// Achtung! In config.pas muss derselbe Wert stehen!!
#define STRMAXLINES	10
#define MAXSTRTEILE	20

#ifdef USE_MSGS
 #define GR_VAR	// Größe durch Variablen angegeben
#endif

#define geom_linie 	1
#define geom_kreisro  	2
#define geom_kreislo	3
#define geom_kreisru	4
#define geom_kreislu	5

#ifdef GR_VAR
 extern int Strteilgrx;
 extern int Strteilgry;
#else
 #define Strteilgrx	100
 #define Strteilgry	100
#endif

#define DESCR_NAME_STRTEILE	"tiles.dat"

typedef struct
{
 int typ;
 int x1,y1,x2,y2;
} strgeom;

typedef struct
{
 #ifdef WITH_ALLEGRO
  BITMAP *gfx;
 #endif
 int b,h;
 int flags;
 int anzg;
 strgeom gobj[STRMAXLINES];
} strteil;

typedef struct
{
 int anz;
 strteil t[MAXSTRTEILE];
} strteile;

int level_strload (strteile *s, const char *verz);

#endif

