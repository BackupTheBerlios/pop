// Message.h

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <sys/socket.h>
#include <netinet/in.h>

/****** Einstellungen */
#define PAS_RECV

/****** Konstanten */

#define MAXCONN		8 // Max 8 Clients
#define MAXBUF		2048 // 2kB max Buffer
#define C_QUEUELEN	60 // Max Messages in Client Queue
#define S_QUEUELEN	1000 // Max Messages in Server queue.


#define G_CONTROL	1
#define G_SETUP		2
#define G_SOUND		4
#define G_GRAFIK	8
#define G_PHYSIK	16
#define G_NET		32
#define G_DEBUG		1024
#define G_ALL		(G_CONTROL+ G_SETUP+ G_SOUND+ G_GRAFIK+ G_PHYSIK+ G_NET+ G_DEBUG)

/****** Strukturen zum Message senden: */

typedef unsigned short mestyp;

// Zufällige Werte für Messagetyp
#define MESNEWCLIENT	0x0012
#define MESACPTCLIENT	0x64a1
#define MESDENYCLIENT	0x8e6d
#define MESSYNC		0x0154
#define MESMESS		0x1245
#define MESEXITCLIENT	0x6575
#define MESEXITSERVER	0x9754

//Für Message Header
#define MT_INTEGER	1
#define MT_STRING	2
#define MT_DATA		3

typedef struct
{
 unsigned char typ;
 int num,group;
 int len;
} messheader;

/****** Allgemeine Connection Funktionen */

struct clientliste
{
 struct sockaddr_in a;
 char *buf;
 int bufpos; /* Position im Buffer, an dem die Daten angehängt werden */
 int status; /* =1 auf weitere Daten wird gewartet
  		=0 auf weiter Messages wird gewartet
                =2 auf Header wird gewartet*/
 int rest; /* Bytes die noch fehlen... */
 int aktion; /* Aktion, die gerade durchgeführt wurde */

 struct clientliste *next;
};

typedef struct clientliste s_client;

typedef struct
{
 struct sockaddr_in *a;
 messheader h;
} s_queue_item;

typedef struct
{
 messheader h;
} c_queue_item;

#ifdef PAS_RECV
typedef void (*c_rcvmsg_func) (int,int,int,int);
typedef void (*c_rcvmsgs_func) (int,int,int,const char *);
typedef void (*c_rcvmsgd_func) (int,int,void*,int);

typedef void (*s_rcvmsg_func) (s_client *,int,int,int,int);
typedef void (*s_rcvmsgs_func) (s_client *,int,int,int,const char *);
typedef void (*s_rcvmsgd_func) (s_client *,int,int,void*,int);
#endif

typedef struct
{
 int fd;
 struct sockaddr_in s_addr;

 /* Verkettete Liste der Clients */
 s_client *first;

 /* Message Queue */
 s_queue_item *queue[S_QUEUELEN];
 int qpos;

 int acpt_client;

#ifdef PAS_RECV
 s_rcvmsg_func rcvmsg;
 s_rcvmsgs_func rcvmsgs;
 s_rcvmsgd_func rcvmsgd;
#endif
} serverchan;

typedef struct
{
 int fd;
 struct sockaddr_in my_addr;
 struct sockaddr_in serv_addr;

 /* Data zum fortsetzen von Übertragungen (s. clientliste) */
 char *buf;
 int bufpos;
 int status;
 int rest;
 int aktion;

 /* Message Buffer... */
 c_queue_item *queue[C_QUEUELEN];
 int qpos;

#ifdef PAS_RECV
 c_rcvmsg_func rcvmsg;
 c_rcvmsgs_func rcvmsgs;
 c_rcvmsgd_func rcvmsgd;
#endif
} clientchan;

int start_server(serverchan **,int port);
void stop_server(serverchan *);

int connect_server( clientchan **,const char *addr, int port);
void disconnect_server(clientchan *);

void calcserv (serverchan *);
void calcclient (clientchan *);

void serv_setacpt (serverchan *,int a);
void serv_kill_client (serverchan *,s_client *);

/****** Interface zu den Pascal Programmen */

#ifdef PAS_RECV
void serv_set_rcvmsg (serverchan *sc,s_rcvmsg_func);
void serv_set_rcvmsgs (serverchan *sc,s_rcvmsgs_func);
void serv_set_rcvmsgd (serverchan *sc,s_rcvmsgd_func);

void cli_set_recvmsg (clientchan *cc,c_rcvmsg_func);
void cli_set_recvmsgs (clientchan *cc,c_rcvmsgs_func);
void cli_set_recvmsgd (clientchan *cc,c_rcvmsgd_func);
#endif

// Können von Pascal aufgerufen werden...
void cli_sendmsg (clientchan *c,int,int,int,int);
void cli_sendmsgs (clientchan *c,int,int,int,const char*);
void cli_sendmsgd (clientchan *c,int,int,void*,int);

void serv_sendmsg (serverchan *sc,s_client *c,int,int,int,int);
void serv_sendmsgs (serverchan *sc,s_client *c,int,int,int,const char*);
void serv_sendmsgd (serverchan *sc,s_client *c,int,int,void*,int);

void serv_sendmsg_all (serverchan *sc,int,int,int,int);
void serv_sendmsgs_all (serverchan *sc,int,int,int,const char*);
void serv_sendmsgd_all (serverchan *sc,int,int,void*,int);

#endif


