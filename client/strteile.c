// Strteile.c
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "strteile.h"

int level_strload (strteile *s, const char *verz)
{
 parsf a;
 char name[160];
 char buf[160];
 int status=0;
 int ateil=0;
 int maxteil=0;
 int aline=0;
 #ifdef WITH_ALLEGRO
  BITMAP *tmpgfx;
 #endif

 strcpy (name,verz);
 strcat (name,DESCR_NAME_STRTEILE);

 a=startparsef(name);

 while ( !(parsefile (&a,buf)) ) {
  // Status Gesamt
  if (status==0) {
   if ( !strcmp (buf,"#") ) {
    status=5; // Kommentar
   }else if ( !strcmp (buf,"SetName") ) {
    parsefile (&a,buf);
   } else if ( !strcmp (buf,"Start") ) {
    parsefile (&a,buf);
    ateil = atoi(buf);
    aline = 0;
    if (ateil>=MAXSTRTEILE) {
     debug_print ("Warnung: Falsche STRTeilNummer!");
     return -1;
    }
    status=1; // TeilParsing...
    #ifdef WITH_ALLEGRO
     s->t[ateil].gfx=NULL;
    #endif
   } else if (iszeichen(buf[0])) {
    debug_print ("Fehlerhaftes Argument:");
    debug_print (buf);
   }
  } else

  // Status Kommentar
  if (status==5) {
   if (buf[0]=='\n')
    status=0;
  } else

  // Status Parsteil
  if (status==1) {
   if (!strcmp (buf,"End")) {
    #ifdef WITH_ALLEGRO
    if (! (s->t[ateil].gfx) ) {
     debug_print ("Kein GfxName in DefFile!");
     return -4;
    }
    #endif
    s->t[ateil].anzg=aline;
    status=0;
    if (ateil+1>maxteil)
     maxteil=ateil+1;
    ateil=0;
   } else if (!strcmp (buf,"Name")) {
    parsefile (&a,buf);
   } else if (!strcmp (buf,"GfxName")) {
    parsefile (&a,buf);
    #ifdef WITH_ALLEGRO
     // Gfx laden...
     strcpy (name,verz);
     strcat (name,buf);
     s->t[ateil].gfx = create_bitmap (Strteilgrx,Strteilgry);
     if (!s->t[ateil].gfx) {
      debug_print ("Speicherfehler!");
      return -6;
     }
     if ( !(tmpgfx = load_bitmap(name,NULL)) ) {
      s->t[ateil].gfx=NULL;
      debug_print ("Lesefehler bei:");
      debug_print (name);
      return -3;
     }
     stretch_sprite(s->t[ateil].gfx,tmpgfx,0,0,Strteilgrx,Strteilgry);
     destroy_bitmap (tmpgfx);
    #endif
   } else if (!strcmp (buf,"Flags")) {
    parsefile (&a,buf);
    s->t[ateil].flags=atoi(buf);
   } else if (!strcmp (buf,"Line")) {
    if (aline>=STRMAXLINES) {
     debug_print ("Warnung! zu viele Linien/Kreise!");
     aline--;
    }
    s->t[ateil].gobj[aline].typ=geom_linie;
    parsefile (&a,buf);
    s->t[ateil].gobj[aline].x1=atoi(buf);
    parsefile (&a,buf); /* Für , */
    parsefile (&a,buf);
    s->t[ateil].gobj[aline].y1=atoi(buf);
    parsefile (&a,buf); /* Für , */
    parsefile (&a,buf);
    s->t[ateil].gobj[aline].x2=atoi(buf);
    parsefile (&a,buf); /* Für , */
    parsefile (&a,buf);
    s->t[ateil].gobj[aline].y2=atoi(buf);
    aline++;
   } else if (!strcmp (buf,"Kreis")) {
    if (aline>=STRMAXLINES) {
     debug_print ("Warnung! zu viele Linien/Kreise!");
     aline--;
    }
    s->t[ateil].gobj[aline].typ=geom_kreisro;
    parsefile (&a,buf);
    s->t[ateil].gobj[aline].x1=atoi(buf);
    parsefile (&a,buf); /* Für , */
    parsefile (&a,buf);
    s->t[ateil].gobj[aline].y1=atoi(buf);
    parsefile (&a,buf); /* Für , */
    parsefile (&a,buf);
    s->t[ateil].gobj[aline].x2=atoi(buf);
    parsefile (&a,buf); /* Für , */
    parsefile (&a,buf);
    s->t[ateil].gobj[aline].typ=atoi(buf);
    aline++;

   } else if (iszeichen(buf[0])) {
    debug_print ("Warnung! Unknown Syntax!");
    debug_print (buf);
   }
  }
 }

 if (ateil>0) {
  debug_print ("Warnug! Teilbeschreibung nicht abgeschlossen!");
 }

 s->anz=maxteil;

 if (!a.f)
  return -1;

 return 0;
}
