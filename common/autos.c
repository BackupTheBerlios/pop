#include <string.h>
#include <stdlib.h>

#include "autos.h"
#include "parser.h"

int autos_load (autos *s, const char *verz)
{
 parsf a;
 char name[1024];
 char buf[1024];
 int status=0;
 int aauto=0;
 #ifdef WITH_ALLEGRO
  BITMAP *tmpgfx;
 #endif

 strcpy (name,verz);
 strcat (name,DESCR_NAME);

 a=startparsef(name);

 while ( !(parsefile (&a,buf)) ) {
  // Status Gesamt
  if (status==0) {
   if ( !strcmp (buf,"#") ) {
    status=5; // Kommentar
   }else if ( !strcmp (buf,"SetName") ) {
    parsefile (&a,buf);
   } else if ( !strcmp (buf,"Auto") ) {
    status=1; // AutoParsing...
    s->d[aauto].name=NULL;
    #ifdef WITH_ALLEGRO
    s->d[aauto].gfx=NULL;
    #endif
    s->d[aauto].b=0;
    s->d[aauto].h=0;
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

  // Status ParsAuto
  if (status==1) {
   if (!strcmp (buf,"End")) {
    #ifdef WITH_ALLEGRO
    if (! (s->d[aauto].gfx) ) {
     debug_print ("Kein GfxName in DefFile!");
     return -4;
    }
    #endif
    if (! (s->d[aauto].name) ) {
     debug_print ("Auto hat keinen Namen!");
     return -4;
    }
    status=0;
    aauto++;
    if (aauto>=MAX_AUTOS) {
     debug_print ("Achtung! Zu viele Autos in Datei! überschreibe letztes!\n");
     aauto=MAX_AUTOS-1;
    }
   } else if (!strcmp (buf,"Breite")) {
    parsefile (&a,buf);
    s->d[aauto].b=atoi(buf);
    if (s->d[aauto].b<=0) {
     debug_print ("Falscher Wert fr Breite!\n");
     return -5;
    }
    parsefile (&a,buf); /* für , */
    parsefile (&a,buf);
    s->d[aauto].h=atoi(buf);
    if (s->d[aauto].h<=0) {
     debug_print ("Falscher Wert fr H÷he!\n");
     return -5;
    }
   } else if (!strcmp (buf,"Name")) {
    parsefile (&a,buf);
    s->d[aauto].name=(char *) malloc (strlen(buf)+1);
    strcpy (s->d[aauto].name,buf);
   } else if (!strcmp (buf,"GfxName")) {
    if (s->d[aauto].b<=0 || s->d[aauto].h<=0) {
     debug_print ("Breite nicht vor GFXName gesetzt!\n");
     return -5;
    }
    parsefile (&a,buf);
    #ifdef WITH_ALLEGRO
     // Gfx laden...
     strcpy (name,verz);
     strcat (name,buf);
     s->d[aauto].gfx = create_bitmap (s->d[aauto].b,s->d[aauto].h);
     clear_to_color (s->d[aauto].gfx,makecol16(255,0,255));
     if (!s->d[aauto].gfx) {
      debug_print ("Speicherfehler!");
      return -6;
     }
     if ( !(tmpgfx = load_bitmap(name,NULL)) ) {
      s->d[aauto].gfx=NULL;
      debug_print ("Lesefehler bei:");
      debug_print (name);
      return -3;
     }
     stretch_sprite(s->d[aauto].gfx,tmpgfx,0,0,s->d[aauto].b,s->d[aauto].h);
     destroy_bitmap (tmpgfx);
    #endif
   } else if (!strcmp (buf,"Flags")) {
    parsefile (&a,buf);
    s->d[aauto].flags=atoi(buf);
   } else if (!strcmp (buf,"PS")) {
    parsefile (&a,buf);
    s->d[aauto].ps=atoi(buf);
   } else if (!strcmp (buf,"Reifen")) {
    parsefile (&a,buf);
    s->d[aauto].reifen=atoi(buf);
   } else if (iszeichen(buf[0])) {
    debug_print ("Warnung! Unknown Syntax:");
    debug_print (buf);
   }
  }
 }

 if (status!=0) {
  debug_print ("Warnug! Autobeschreibung nicht abgeschlossen!");
 }

 s->anz=aauto;

 if (!a.f)
  return -1;

 return 0;
}
