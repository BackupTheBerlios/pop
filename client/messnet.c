// messnet.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>

#include "sysconf.h"
#include "message.h"

/* *****************
   * Einstellungen *
   *****************/

#include "cconfig.h"

/* ***************
   * Server Teil *
   ***************/

#ifdef PAS_RECV
void __INLINE__ s_sendmsg (serverchan *sc,s_client *c,int g,int n,int a1,int a2)
{
 if (sc->rcvmsg)
  sc->rcvmsg (c,g,n,a1,a2);
}

void __INLINE__ s_sendmsgs (serverchan *sc,s_client *c,int g,int n,int a1,const char *a2)
{
 if (sc->rcvmsgs)
  sc->rcvmsgs (c,g,n,a1,a2);
}

void __INLINE__ s_sendmsgd (serverchan *sc,s_client *c,int g,int n,void *a1,int a2)
{
 if (sc->rcvmsgd)
  sc->rcvmsgd (c,g,n,a1,a2);
}
#endif

int start_server(serverchan **sc,int port)
{
  int a;
  size_t addrlength;

  *sc = (serverchan *) malloc (sizeof (serverchan));
  if (!(*sc)) return -1;
  
  (*sc)->fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if ((*sc)->fd<0) {
   free((*sc));
   *sc=NULL;
   return -2;
  }

  a=1;
  if (ioctl ((*sc)->fd, FIONBIO, &a)) {
    close ((*sc)->fd);
    free((*sc));
    *sc=NULL;
    return -3;
  }
  
  (*sc)->s_addr.sin_family      = AF_INET;
  (*sc)->s_addr.sin_port        = htons(port);
  (*sc)->s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  addrlength = sizeof (struct sockaddr_in);

  if (bind ((*sc)->fd, (struct sockaddr *)&(*sc)->s_addr, addrlength)) {
    close ((*sc)->fd);
    free((*sc));
    *sc=NULL;
    return -4;
  }

 (*sc)->acpt_client=1;

 (*sc)->first=NULL;
 (*sc)->qpos=0;

#ifdef PAS_RECV
 (*sc)->rcvmsg=NULL;
 (*sc)->rcvmsgs=NULL;
 (*sc)->rcvmsgd=NULL;
#endif

 return 0;
}

void stop_server(serverchan *sc)
{
 mestyp b;
 s_client *al;

 b=MESEXITSERVER;

 for (al=sc->first;al!=NULL;al=al->next) {
  if (sendto ( sc->fd,&b ,sizeof(mestyp), 0, (struct sockaddr *) &al->a,
 			sizeof(struct sockaddr_in) ) < 0) {
   debug_print ("Send Error!\n");
  }
  free (al->buf);
  free (al);
 }

 if (sc) {
  close (sc->fd);
  free (sc);
  }
 return;
}

void serv_setacpt (serverchan *sc,int a)
{
 sc->acpt_client=a;
}

#ifdef PAS_RECV
void serv_set_rcvmsg(serverchan *sc,s_rcvmsg_func f)
{
 sc->rcvmsg=f;
}

void serv_set_rcvmsgs(serverchan *sc,s_rcvmsgs_func f)
{
 sc->rcvmsgs=f;
}

void serv_set_rcvmsgd(serverchan *sc,s_rcvmsgd_func f)
{
 sc->rcvmsgd=f;
}
#endif

void server_sync (s_client *c)
{
 debug_print ("Sync kommt noch...\n");
}

void __INLINE__ s_decompr_int (serverchan *c,s_client *a,char *buf)
{
 int n,g,arg1,arg2;
 messheader *h=(messheader *) buf;
 int *data=(int *) (h+1);

 n=h->num;g=h->group;
 arg1=data[0];arg2=data[1];
 s_sendmsg (c,a,g,n,arg1,arg2);
}

void __INLINE__ s_decompr_str (serverchan *c,s_client *a,char *buf)
{
 int n,g,arg1;
 messheader *h=(messheader *) buf;
 int *datai=(int *) (h+1);
 char *arg2=(char *) (datai+1);

 n=h->num;g=h->group;
 arg1=*datai;
 s_sendmsgs (c,a,g,n,arg1,arg2);
}

void __INLINE__ s_decompr_dat (serverchan *c,s_client *a,char *buf)
{
 int n,g;
 messheader *h=(messheader *)buf;
 void *arg1=(char *) (h+1);

 n=h->num;
 g=h->group;
 s_sendmsgd (c,a,g,n,arg1,h->len);
}

// Server Main Loop...
void calcserv(serverchan *sc)
{
 struct sockaddr_in taddr;
 sock_size_t addrlength; // I hate these different types in different envs
 char gbuf[MAXBUF];
 int gbuflen,gbufpos,datalen;
 int i;
 mestyp b;
 s_client *al,*bl;

 // Messages empfangen
 addrlength=sizeof(struct sockaddr_in);
 while ((gbuflen=recvfrom (sc->fd, gbuf, MAXBUF, 0, (struct sockaddr *) &taddr,
        					&addrlength)) > 0) {
  gbufpos=0;
  // Client suchen
  for (al=sc->first;al!=NULL;al=al->next)
   if (al->a.sin_port==taddr.sin_port && al->a.sin_addr.s_addr==taddr.sin_addr.s_addr)
    break;

  if (al!=NULL || (*((mestyp *) (gbuf+gbufpos))==MESNEWCLIENT)) {

  while (gbufpos<gbuflen) {
  if (al!=NULL && al->status==1) {
   // Noch Daten erwartet...
   memcpy (al->buf+al->bufpos,gbuf, (gbuflen-gbufpos)<al->rest ? (gbuflen-gbufpos) : al->rest);

   if ( (gbuflen-gbufpos) <al->rest) {
     al->bufpos+=gbuflen-gbufpos;
     gbufpos=0;
   } else {
    switch (al->aktion) {
     case MT_INTEGER:
      s_decompr_int (sc,al,al->buf);
      break;
     case MT_STRING:
      s_decompr_str (sc,al,al->buf);
      break;
     case MT_DATA:
      s_decompr_dat (sc,al,al->buf);
      break;
     default:
      debug_print ("seltsamer Fehler!\n");
     }
    al->bufpos=0;
    al->aktion=0;
    al->status=0;

    gbufpos+=al->rest;
   }
  } else {

  // Aktion herausfinden
  if (al!=NULL && al->status!=0) {
   b=al->status;
  } else {
   b=*((mestyp *) (gbuf+gbufpos));
   gbufpos+=sizeof(mestyp);
  }

  switch ( b ) {
   case MESNEWCLIENT:
    // An Liste anh„ngen...
    if (al==NULL) {
     if (sc->acpt_client) {
      al=sc->first;
      sc->first= (s_client *) malloc (sizeof(s_client));
      sc->first->buf=(char *) malloc(MAXBUF);
      sc->first->bufpos=0;
      sc->first->status=0;
      sc->first->rest=0;
      sc->first->aktion=0;

      sc->first->a=taddr;
      sc->first->next=al;
      al=sc->first;

      // Accept Message
      b=MESACPTCLIENT;
      if (sendto (sc->fd, &b, sizeof(mestyp), 0, (struct sockaddr *) &taddr,
         						addrlength) < 0) {
       debug_print ("Send Error!\n");
       return;
      }
      debug_print ("Client accepted!\n");
#ifdef PAS_RECV
      s_sendmsg (sc,sc->first,32,1,0,0);
#endif
     } else {
      //Client rausschmei˜en...
      b=MESDENYCLIENT;
      if (sendto (sc->fd, &b, sizeof(mestyp), 0, (struct sockaddr *) &taddr,
          						addrlength) < 0) {
       debug_print ("Send Error!\n");
       return;
      }
      debug_print ("Client denied!\n");
     }
    } else debug_print ("Double Accepting!\n");
    break;

   case MESSYNC:
    debug_print ("Synchronizing... \n");
    server_sync(al);
    break;

   case MESEXITCLIENT:
    // Aus Liste l÷schen...
    debug_print ("Client disconnected!\n");
#ifdef PAS_RECV
    s_sendmsg (sc,al,32,2,0,0);
#endif
    if (sc->first==al)
     sc->first=al->next;
    else {
     for (bl=sc->first;bl->next!=al;bl=bl->next);
     bl->next=al->next;
    }
    /* Alle vorkommen in der Message-Queue suchen und löschen... */
    for (i=0;i<sc->qpos;i++) {
     if (sc->queue[i]->a==&al->a)
      sc->queue[i]->a=NULL;
    }
    free (al->buf);
    free (al);
    al=NULL;
    break;

   case MESMESS:
    //Header lesen...
    if (gbuflen-gbufpos==0) {
     al->status=MESMESS; /* Auf Header warten */
    } else

    if (gbuflen-gbufpos < sizeof(messheader)) { /*Schit! Header ist nicht mehr
                                                  mit im Paket!*/
     debug_print ("Header nicht mit im Paket!\n");
     server_sync(al);
    } else {
     messheader *h=(messheader *) gbuf+gbufpos;
     if (gbuflen-gbufpos>=sizeof(messheader)+h->len) /* Message passt in einen
       						Buffer, GUT! */
      {
       switch (h->typ) {
        case MT_INTEGER:
         s_decompr_int (sc,al,(char *) h);
         break;
        case MT_STRING:
         s_decompr_str (sc,al,(char *) h);
         break;
        case MT_DATA:
         s_decompr_dat (sc,al,(char *) h);
         break;
        default:
         debug_print ("fehlerhafte Message!\n");
         server_sync(al);
         break;
       }
      gbufpos+=sizeof(messheader)+h->len;
      al->status=0;
      }
     else { /* Message passt nicht, SCHLECHT! */
      al->status=1;
      al->rest=h->len- (sizeof(messheader)+gbuflen-gbufpos);
      al->bufpos=gbuflen-gbufpos;
      al->aktion=h->typ;
      memcpy (al->buf,h,gbuflen-gbufpos);

      gbufpos=gbuflen;
      debug_print ("Big Message!\n");
     }
    }
    break;

   default:
    debug_print ("Unbekannte Message!\n");
    server_sync(al);
  } // case mestyp...
  } // if status=0
  } // While gbufpos>0
  } // if al!=NULL || b=MESNEWCLIENT
 } // while recvfrom...

 // Queue senden...
 for (i=0;i<sc->qpos;i++) {
  if (sc->queue[i]->a!=NULL) {
   b=MESMESS;
   if (sendto (sc->fd, &b, sizeof(mestyp),
  	0, (struct sockaddr *) sc->queue[i]->a, sizeof(struct sockaddr_in)) !=
         	sizeof(mestyp)) {
     debug_print ("Send Fehler!\n");
   }

   datalen=sizeof(messheader)+sc->queue[i]->h.len;
   if (sendto (sc->fd, &(sc->queue[i]->h), datalen,
  	0, (struct sockaddr *) sc->queue[i]->a, sizeof(struct sockaddr_in)) !=
         	datalen) {
     debug_print ("Send Fehler!\n");
   }
   free (sc->queue[i]);
  }
 }
 // Queue leeren
 sc->qpos=0;
}

void serv_kill_client (serverchan *sc,s_client *al)
{
 s_client *bl;
 mestyp b;

 if (al!=NULL) {
  //Client rausschmei˜en...
  b=MESEXITSERVER;
  if (sendto (sc->fd, &b, sizeof(mestyp), 0, (struct sockaddr *) &al->a,
       						sizeof (struct sockaddr)) < 0) {
   debug_print ("Send Error!\n");
   return;
  }

  // Aus Liste löschen...
#ifdef PAS_RECV
  s_sendmsg (sc,al,32,2,0,0);
#endif
  if (sc->first==al)
   sc->first=al->next;
  else {
   for (bl=sc->first;bl->next!=al;bl=bl->next);
   bl->next=al->next;
  }
  free (al->buf);
  free (al);
 }
}

/***************
 * Client Teil *
 ***************/

#ifdef PAS_RECV
void __INLINE__ c_sendmsg (clientchan *cc,int g,int n,int a1,int a2)
{
 if (cc->rcvmsg)
  cc->rcvmsg (g,n,a1,a2);
}

void __INLINE__ c_sendmsgs (clientchan *cc,int g,int n,int a1,const char *a2)
{
 if (cc->rcvmsgs)
  cc->rcvmsgs (g,n,a1,a2);
}

void __INLINE__ c_sendmsgd (clientchan *cc,int g,int n,void *a1,int a2)
{
 if (cc->rcvmsgd)
  cc->rcvmsgd (g,n,a1,a2);
}
#endif

int connect_server(clientchan **sc,const char *addr,int port)
{
  unsigned int a;
  sock_size_t addrlength;
  mestyp b;
  int timeout;
  struct hostent *hpke;

  *sc = (clientchan *) malloc (sizeof (clientchan));
  if (!*sc) return -1;
  
  (*sc)->fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if ((*sc)->fd<0) {
   free((*sc));
   return -2;
  }

  a=1;
  if (ioctl ((*sc)->fd, FIONBIO, &a)) {
    close ((*sc)->fd);
    free((*sc));
    return -3;
  }

  hpke = gethostbyname ( addr );

  if ( hpke == NULL ) {
      return -4;
  }

  (*sc)->my_addr.sin_family      = AF_INET;
  (*sc)->my_addr.sin_port        = htons(0);
  (*sc)->my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  (*sc)->serv_addr.sin_family	 = AF_INET;
  (*sc)->serv_addr.sin_port	 = htons(port);
  (*sc)->serv_addr.sin_addr.s_addr = ((struct in_addr *)hpke->h_addr)->s_addr;

  addrlength = sizeof ((*sc)->my_addr);

  if (bind ((*sc)->fd, (struct sockaddr *)&((*sc)->my_addr), addrlength)) {
    close ((*sc)->fd);
    free((*sc));
    return -5;
  }

  // Set Vars
  (*sc)->qpos=0;
  (*sc)->buf=(char *) malloc(MAXBUF);
  (*sc)->bufpos=0;
  (*sc)->status=0;
  (*sc)->rest=0;
  (*sc)->aktion=0;

#ifdef PAS_RECV
  (*sc)->rcvmsg=NULL;
  (*sc)->rcvmsgs=NULL;
  (*sc)->rcvmsgd=NULL;
#endif

  // Validate Server...
  for (timeout=10;timeout>0;timeout--) {
        b=MESNEWCLIENT;
	if (sendto ( (*sc)->fd,&b ,sizeof(mestyp), 0, (struct sockaddr *) &((*sc)->serv_addr),
				sizeof(struct sockaddr_in) ) < 0) {
		close((*sc)->fd);
                free(*sc);
		return -6;
	}
  	/* Matze: Funktioniert nicht auf allen Rechnern... */
	#ifdef WITH_ALLEGRO
	rest(1000);
	#else
  	usleep(1000);
	#endif
      
	  addrlength=sizeof(struct sockaddr_in);
        if (recvfrom ( (*sc)->fd, (char *)&b, sizeof(mestyp), 0, (struct sockaddr *) &((*sc)->serv_addr),
            			&addrlength ) > 0) {
         if (b==MESACPTCLIENT) {
          debug_print ("Server response!\n");
          return 0;
         }
         if (b==MESDENYCLIENT) {
          debug_print ("Server denied acces!\n");
          close ((*sc)->fd);
          free (*sc);
          return -7;
         }
        }
  }

  close ((*sc)->fd);
  free (*sc);
  return -8;
}

void disconnect_server(clientchan *sc)
{
 mestyp b;
 // Send Disconnect Message...
 b=MESEXITCLIENT;
 if (sendto ( sc->fd,&b ,sizeof(mestyp), 0, (struct sockaddr *) &sc->serv_addr,
			sizeof(struct sockaddr_in) ) < 0) {
  debug_print ("Send Error!\n");
 }

 if (sc) {
  close (sc->fd);
  free (sc->buf);
  free (sc);
  }
 return;
}

#ifdef PAS_RECV
void cli_set_rcvmsg(clientchan *cc,c_rcvmsg_func f)
{
 cc->rcvmsg=f;
}

void cli_set_rcvmsgs(clientchan *cc,c_rcvmsgs_func f)
{
 cc->rcvmsgs=f;
}

void cli_set_rcvmsgd(clientchan *cc,c_rcvmsgd_func f)
{
 cc->rcvmsgd=f;
}
#endif

void client_sync (clientchan *c)
{
 debug_print ("Client Sync kommt noch...\n");
}

void __INLINE__ c_decompr_int (clientchan *c,char *buf)
{
 int n,g,arg1,arg2;
 messheader *h=(messheader *) buf;
 int *data=(int *) (h+1);

 n=h->num;g=h->group;
 arg1=data[0];arg2=data[1];
 c_sendmsg (c,g,n,arg1,arg2);
}

void __INLINE__ c_decompr_str (clientchan *c,char *buf)
{
 int n,g,arg1;
 messheader *h=(messheader *) buf;
 int *datai=(int *) (h+1);
 char *arg2=(char *) (datai+1);

 n=h->num;g=h->group;
 arg1=*datai;
 c_sendmsgs (c,g,n,arg1,arg2);
}

void __INLINE__ c_decompr_dat (clientchan *c,char *buf)
{
 int n,g;
 messheader *h=(messheader *)buf;
 void *arg1=(char *) (h+1);

 n=h->num;
 g=h->group;
 c_sendmsgd (c,g,n,arg1,h->len);
}

void calcclient(clientchan *c)
{
 int i;
 unsigned long datalen;
 mestyp b;
 char gbuf[MAXBUF];
 int gbufpos,gbuflen;
 struct sockaddr_in taddr;
 sock_size_t addrlen;

 // Messages empfangen...
 // Hoffentlich nur Messages vom Server...
 addrlen=sizeof (struct sockaddr_in);
 while ((gbuflen=recvfrom (c->fd, gbuf, MAXBUF, 0,
           (struct sockaddr *) &taddr, &addrlen)) > 0) {
  gbufpos=0;

  while (gbufpos<gbuflen) {
  if (c->status==1) {
   // Noch Daten erwartet...
   memcpy (c->buf+c->bufpos,gbuf, (gbuflen-gbufpos)<c->rest ? (gbuflen-gbufpos) : c->rest);

   if ( (gbuflen-gbufpos) < c->rest) {
     c->bufpos+=gbuflen-gbufpos;
     gbufpos=0;
   } else {
    switch (c->aktion) {
     case MT_INTEGER:
      c_decompr_int (c,c->buf);
      break;
     case MT_STRING:
      c_decompr_str (c,c->buf);
      break;
     case MT_DATA:
      c_decompr_dat (c,c->buf);
      break;
     default:
      debug_print ("seltsamer Fehler!\n");
     }
    c->bufpos=0;
    c->aktion=0;
    c->status=0;

    gbufpos+=c->rest;
   }
  } else {

  // Aktion herausfinden
  if (c->status!=0) {
   b=c->status;
  } else {
   b=*((mestyp *) (gbuf+gbufpos));
   gbufpos+=2;
  }

  switch ( b ) {
   case MESSYNC:
    debug_print ("Synchronizing... \n");
    client_sync(c);
    break;

   case MESEXITSERVER:
    debug_print ("Server killed us! :(!\n");
#ifdef PAS_RECV
    c_sendmsg (c,32,2,0,0);
#endif
    break;

   case MESMESS:
    //Header lesen...
    if (gbuflen-gbufpos==0) {
     c->status=MESMESS; /* Auf Header warten */
    } else

    if (gbuflen-gbufpos < sizeof(messheader)) { /*Schit! Header ist nicht mehr
                                                  mit im Paket!*/
     debug_print ("Header zersplittet!\n");
     client_sync(c);
    } else {
     messheader *h=(messheader *) gbuf+gbufpos;
     if (gbuflen-gbufpos>=sizeof(messheader)+h->len) /* Message passt in einen
       						Buffer, GUT! */
      {
       switch (h->typ) {
        case MT_INTEGER:
         c_decompr_int (c,(char *) h);
         break;
        case MT_STRING:
         c_decompr_str (c,(char *) h);
         break;
        case MT_DATA:
         c_decompr_dat (c,(char *) h);
         break;
        default:
         debug_print ("fehlerhafte Message!\n");
         client_sync(c);
         break;
       }
      gbufpos+=sizeof(messheader)+h->len;
      c->status=0;
      }
     else { /* Message passt nicht, SCHLECHT! */
      c->status=1;
      c->rest=h->len- (sizeof(messheader)+gbuflen-gbufpos);
      c->bufpos=gbuflen-gbufpos;
      c->aktion=h->typ;
      memcpy (c->buf,h,gbuflen-gbufpos);

      gbufpos=gbuflen;
     }
    }
    break;

   default:
    debug_print ("Unbekannte Message!\n");
    client_sync(c);
  }
  gbufpos+=sizeof(mestyp);
  } // if status=0
  } // While gbufpos>0
 }

 // Queue senden...
 for (i=0;i<c->qpos;i++) {
  b=MESMESS;
  if (sendto (c->fd, &b, sizeof(mestyp),
 	0, (struct sockaddr *) &c->serv_addr, sizeof(struct sockaddr_in)) !=
        	sizeof(mestyp)) {
    debug_print ("Send Fehler!\n");
  }

  datalen=sizeof(messheader)+c->queue[i]->h.len;

  if ((unsigned long) (sendto (c->fd, &(c->queue[i]->h), datalen,
 	0, (struct sockaddr *) &c->serv_addr, sizeof(struct sockaddr_in))) !=
        	datalen) {
    debug_print ("Send Fehler!\n");
  }
  free (c->queue[i]);
 }
 // Queue leeren
 c->qpos=0;
}

void __INLINE__ cli_insert_queue(clientchan *c,c_queue_item *q)
{
 if (c->qpos+1>C_QUEUELEN) {
  debug_print ("Client Queue überlauf!\n");
  return;
 }

 c->queue[c->qpos++]=q;
}

void __INLINE__ serv_insert_queue(serverchan *c,s_queue_item *q)
{
 if (c->qpos+1>S_QUEUELEN) {
  debug_print ("Server Queue überlauf!\n");
  return;
 }

 c->queue[c->qpos++]=q;
}

/* *********************
   * Message Interface *
   *********************/

void cli_sendmsg (clientchan *c,int g,int n,int arg1,int arg2)
{
 c_queue_item *q;
 int *data;

 q = (c_queue_item *) malloc(sizeof(c_queue_item)+sizeof(int)*2);
 data = (int *) (q+1);
 q->h.typ=MT_INTEGER;
 q->h.num=n;
 q->h.group=g;
 q->h.len=sizeof(int)*2;
 data[0]=arg1;
 data[1]=arg2;

 cli_insert_queue (c,q);
}

void cli_sendmsgs (clientchan *c,int g,int n,int arg1,const char *arg2)
{
 c_queue_item *q;
 int *datai;
 char *data;

 q = (c_queue_item *) malloc (sizeof(c_queue_item)+sizeof(int)+strlen(arg2)+1);
 datai=(int *) (q+1);
 data =(char *) (datai+1);

 q->h.typ=MT_STRING;
 q->h.num=n;
 q->h.group=g;
 q->h.len=sizeof(int)+strlen(arg2)+1;
 *datai=arg1;
 memcpy (data,arg2,q->h.len-sizeof(int));

 cli_insert_queue (c,q);
}

void cli_sendmsgd (clientchan *c,int g,int n,void *arg1,int arg2)
{
 c_queue_item *q;
 void *data;

 q = (c_queue_item *) malloc (sizeof(c_queue_item)+arg2);
 data = (void *) (q+1);
 q->h.typ=MT_DATA;
 q->h.num=n;
 q->h.group=g;
 q->h.len=arg2;
 memcpy (data,arg1,arg2);

 cli_insert_queue (c,q);
}

void serv_sendmsg (serverchan *sc,s_client *c,int g,int n,int arg1,int arg2)
{
 s_queue_item *q;
 int *data;

 q = (s_queue_item *) malloc (sizeof(s_queue_item)+sizeof(int)*2);
 data = (int *) (q+1);

 q->a=&c->a;
 q->h.typ=MT_INTEGER;
 q->h.num=n;
 q->h.group=g;
 q->h.len=sizeof(int)*2;
 data[0]=arg1;
 data[1]=arg2;

 serv_insert_queue (sc,q);
}

void serv_sendmsgs (serverchan *sc,s_client *c,int g,int n,int arg1,const char *arg2)
{
 s_queue_item *q;
 int *datai;
 char *data;

 q = (s_queue_item *) malloc (sizeof(s_queue_item)+sizeof(int)+strlen(arg2)+1);
 datai = (int *) (q+1);
 data = (char *) (datai+1);

 q->a=&c->a;
 q->h.typ=MT_STRING;
 q->h.num=n;
 q->h.group=g;
 q->h.len=sizeof(int)+strlen(arg2)+1;
 *datai=arg1;
 memcpy (data,arg2,q->h.len-sizeof(int));

 serv_insert_queue (sc,q);
}

void serv_sendmsgd (serverchan *sc,s_client *c,int g,int n,void *arg1,int arg2)
{
 s_queue_item *q;
 void *data;

 q = (s_queue_item *) malloc (sizeof(s_queue_item)+arg2);
 data = (void *) (q+1);
 q->a=&c->a;

 q->h.typ=MT_DATA;
 q->h.num=n;
 q->h.group=g;
 q->h.len=arg2;
 memcpy (data,arg1,arg2);

 serv_insert_queue (sc,q);
}

void serv_sendmsg_all (serverchan *sc, int g, int n, int arg1,int arg2)
{
 s_client *p;

 p=sc->first;
 while (p!=NULL) {
  serv_sendmsg (sc,p,g,n,arg1,arg2);
  p=p->next;
 }
}

void serv_sendmsgs_all (serverchan *sc, int g, int n, int arg1,const char *arg2)
{
 s_client *p;

 p=sc->first;
 while (p!=NULL) {
  serv_sendmsgs (sc,p,g,n,arg1,arg2);
  p=p->next;
 }
}

void serv_sendmsgd_all (serverchan *sc, int g, int n, void *arg1,int arg2)
{
 s_client *p;

 p=sc->first;
 while (p!=NULL) {
  serv_sendmsgd (sc,p,g,n,arg1,arg2);
  p=p->next;
 }
}
