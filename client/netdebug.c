// netdebug.c

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "parser.h"

#define DEBUG_PORT      1111

typedef struct al
{
 char von[256];
 char nach[256];
 struct al *next;
} aliaslist;

// globale Vars...
int started;
int chan;
int connection;
char inbuf[255];
int bpos;
aliaslist *list_alias;

int init_netdebug();
void run_netdebug();
void exit_netdebug();

void add_alias(const char *von,const char *nach)
{
 aliaslist *t=list_alias;

 list_alias = malloc (sizeof(aliaslist));
 list_alias->next=t;
 strcpy (list_alias->von,von);
 strcpy (list_alias->nach,nach);
}

void init_alias()
{
 add_alias ("quit","exit");
 add_alias ("hilfe","help");
 add_alias ("CONTROL","1");
 add_alias ("SETUP","2");
 add_alias ("SOUND","4");
 add_alias ("GRAFIK","8");
 add_alias ("PHYSIK","16");
 add_alias ("NET","32");
 add_alias ("DEBUG","1024");
 add_alias ("GROUP_ALL","63");
 add_alias ("STATUS_UNBEKANNT","101");
 add_alias ("STATUS_WAIT","0");
 add_alias ("STATUS_RUN","2");
 add_alias ("STATUS_NO_CONNECT","100");
 add_alias ("STATUS_PAUSE","3");
 add_alias ("STATUS_ERGEBNIS","4");
 add_alias ("STATUS_ENDE","1234");
}

void change_alias (char *buf)
{
 aliaslist *t=list_alias;

 while (t!=NULL) {
  if (strcmp (buf,t->von)==0) {
   strcpy (buf,t->nach);
   /* Rekursives Alias? */
   change_alias (buf);
  }
  t=t->next;
 }
}

int init_netdebug()
{
 struct sockaddr_in in;
 int a;

 started=0;

 /*
  * At first we have to create socket.
  * After that we bind it on desired port and after that we begin
  * to listen it.
  */

 memset ( &in, 0, sizeof ( struct sockaddr_in ) );

 if ( (chan = socket ( AF_INET, SOCK_STREAM, IPPROTO_TCP )) < 0 )
  return -1;

 a=1;
 if (ioctl (chan, FIONBIO, &a) < 0)
  return -2;

 in.sin_family = AF_INET;
 in.sin_addr.s_addr = INADDR_ANY;
 in.sin_port = htons ( DEBUG_PORT );

 if (bind(chan, (struct sockaddr *) &in, sizeof (struct sockaddr_in)) < 0)
  return -3;

 /* Matze: What stands 5 for? */
 if (listen (chan, 5) < 0)
  return -4;

 started=1;
 connection=-1;
 bpos=0;
 list_alias=NULL;
 init_alias();

 return 0;
}

void sstring (const char *s)
{
 char cbuf[256];

 if (connection > 0) {
  strcpy (cbuf,s);
  cbuf[strlen(cbuf)+1]=13;
  cbuf[strlen(cbuf)+2]='\0';
  cbuf[strlen(cbuf)]=10;

  send (connection,(void *) cbuf, strlen(cbuf)+1, 0);
 }
}

/*Aus message.pas*/
void Sendmsg(int g,int n,int a1,int a2);
void Sendmsgs(int g,int n,int a1,const char *a2);

void showhelp()
{
 sstring ("Befehle:");
 sstring ("");
 sstring ("sendmsg gruppe,nummer,arg1,arg2");
 sstring ("sendmsgs gruppe,nummer,arg1,string");
 sstring ("alias von,nach");
 sstring ("exit");
 sstring ("help");
}

void showaliases()
{
 aliaslist *t=list_alias;
 char buf[256];

 sstring ("Aliases:");
 while (t!=NULL) {
  sprintf (buf,"%s - %s",t->von,t->nach);
  sstring (buf);
  t=t->next;
 }
}

void doalias (parss *p)
{
 aliaslist *a=malloc (sizeof (aliaslist));
 aliaslist *t;

 while (parseline(p,a->von) && !iszeichen(a->von[0]));

 if (!iszeichen(a->von[0])) {
  showaliases();
  free (a);
  return;
 }

 while (parseline(p,a->nach)>=0 && !iszeichen(a->nach[0]));

 if (!iszeichen(a->nach[0])) {
  change_alias (a->von);
  sstring (a->von);
  free(a);
  return;
 } else {
  t=list_alias;
  list_alias=a;
  a->next=t;
  sstring ("Alias defined!");
 }
}

void dosendmsg (parss *p)
{
 int n,g,a1,a2;
 char buf[256];

 while (parseline (p,buf)>=0) {
  change_alias (buf);

  if (iszeichen(buf[0]))
   break;
 }

 if (!iszeichen(buf[0]))
  sstring ("too less Parameters!");

 g=atoi(buf);

 while (parseline (p,buf)>=0) {
  change_alias (buf);

  if (iszeichen(buf[0]))
   break;
 }

 if (!iszeichen(buf[0]))
  sstring ("too less Parameters!");

 n=atoi(buf);

 while (parseline (p,buf)>=0) {
  change_alias (buf);

  if (iszeichen(buf[0]))
   break;
 }

 if (!iszeichen(buf[0]))
  sstring ("too less Parameters!");

 a1=atoi(buf);

 while (parseline (p,buf)>=0) {
  change_alias (buf);

  if (iszeichen(buf[0]))
   break;
 }

 if (!iszeichen(buf[0]))
  sstring ("too less Parameters!");

 a2=atoi(buf);

 Sendmsg (g,n,a1,a2);
}

void dosendmsgs (parss *p)
{
 int n,g,a1;
 char buf[256];

 while (parseline (p,buf)>=0) {
  change_alias (buf);

  if (iszeichen(buf[0]))
   break;
 }

 if (!iszeichen(buf[0]))
  sstring ("too less Parameters!");

 g=atoi(buf);

 while (parseline (p,buf)>=0) {
  change_alias (buf);

  if (iszeichen(buf[0]))
   break;
 }

 if (!iszeichen(buf[0]))
  sstring ("too less Parameters!");

 n=atoi(buf);

 while (parseline (p,buf)>=0) {
  change_alias (buf);

  if (iszeichen(buf[0]))
   break;
 }

 if (!iszeichen(buf[0]))
  sstring ("too less Parameters!");

 a1=atoi(buf);

 while (parseline (p,buf)>=0) {
  change_alias (buf);

  if (iszeichen(buf[0]))
   break;
 }

 if (!iszeichen(buf[0]))
  sstring ("too less Parameters!");

 Sendmsgs (g,n,a1,buf);
}

void action (const char *s)
{
 parss p;
 char buf[256];

 p=startparse(s);

 while (parseline(&p,buf)>=0) {
  change_alias (buf);

  if (iszeichen(buf[0])) {
   /* Befehl gefunden */
   if (strcmp(buf,"sendmsg")==0) {
    dosendmsg (&p);
    return;

   } else if (strcmp(buf,"sendmsgs")==0) {
    dosendmsgs (&p);
    return;

   } else if (strcmp(buf,"alias")==0) {
    doalias (&p);
    return;

   } else if (strcmp(buf,"exit")==0) {
    sstring ("Bye!");
    close(connection);
    connection=-1;
    return;

   } else if (strcmp(buf,"help")==0) {
    showhelp();
    return;

   } else {
    sstring ("Unknown Command");
   }
  }
 }
}

void run_netdebug()
{
 struct sockaddr_in peer_in;
 size_t len;
 char c;
 int a;

 if (started) {
  if (connection < 0) {
   len = sizeof ( struct sockaddr_in );
   connection = accept ( chan, (struct sockaddr *)&peer_in, &len );

   if (connection > 0) {
    a=1;
    ioctl (connection, FIONBIO, &a);

    sstring ("--- Debug Connection zu PoP! ---");
    sstring ("");
    sstring ("netdebug modules by M.B.");
    sstring ("OK!");
   }
  }

  if (connection > 0) {
   if (recv (connection,&c,1,0) >= 1) {
    inbuf[bpos]=c;
    bpos++;
    send (connection,&c,1,0);
    if (c=='\n') {
     inbuf[bpos]='\0';
     bpos=0;
     action (inbuf);
    }
   }
  }
 }
}

void exit_netdebug()
{
 started=0;
 if (connection>0)
  close(connection);
 close(chan);
}
