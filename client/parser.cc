// netdebug.c

#include <malloc.h>
#include <ctype.h>
#include <fcntl.h>

#include "parser.h"

#define MAXIBUF		240

parss startparse(const char *str)
{
 parss temp;

 temp.apos=-1;
 temp.str=str;

 return temp;
}

parsf startparsef(const char *fname)
{
 parsf temp;

 temp.f=fopen(fname,"r");

 temp.temps = (char *) malloc(MAXIBUF);
 if ( !(temp.temps) )
  exit (1);

 strcpy (temp.temps,"");

 temp.apars = startparse(temp.temps);

 return temp;
}

int iszeichen(char c)
{
 if (c=='_')
  return 1;
 if (isalnum(c))
  return 1;

 return 0;
}

int issz(char c)
{
 if (c==',' || c==';' || c=='\n')
  return 1;
 if (c=='#' || c=='(' || c==')')
  return 1;
 if (c=='.')
  return 1;

 return 0;
}

int parseline(parss *s,char *buf)
{
 int atyp=0;
 int start=-1;
 int brack=0;
 int outpos=0;
 char aktc;

 while (1) {
  s->apos++;
  aktc=s->str[s->apos];

  /* String zu Ende? */
  if (atyp==0 && aktc=='\0') {
   buf[0]='\0';
   s->apos--;
   return -1;
  }

  /* Zeichen zum Beenden? dann raus*/
  if (atyp==0 && issz(aktc)) {
   buf[0]=aktc;
   buf[1]='\0';
   return 0;
  }

  /* Typ1 : Alphanumerische Zeichen */
  if (iszeichen(aktc) && atyp==0) {
   start=s->apos;
   atyp=1;
  }

  if (atyp==1) {
   if (iszeichen(aktc))
    buf[s->apos-start]=aktc;
   else {
    buf[s->apos-start]='\0';
    s->apos--;
    return 0;
   }
  }

  /* Typ2 : Zeichenketten */
  if (atyp==0 && aktc=='"') {
   start=s->apos;
   atyp=2;
  }

  else if (atyp==2) {
   if (aktc=='\\') {
    if (brack==0)
     brack=1;
    else {
     buf[outpos++]=aktc;
     brack=0;
    }
   } else if (brack) {
    buf[outpos++]=aktc;
    brack=0;
   } else if (aktc=='"' && !brack && start!=s->apos) {
    buf[outpos]='\0';
    return 0;
   } else {
    buf[outpos++]=aktc;
   }
  }
 }
}

int parsefile(parsf *f,char *buf)
{
 if (!f->f) {
  return -2;
 }

 while (parseline(&f->apars,buf)<0) {
  if (feof(f->f))
   return -1;

  fgets (f->temps,MAXIBUF-1,f->f);
  f->apars = startparse(f->temps);
 }

 return 0;
}
