/***************************************************************************
                          server.cc  -  description
                             -------------------
    begin                : Sun Apr 2 2000
    copyright            : (C) 2000 by Matze Braun
    email                : MatzeBraun@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream.h>
#include <list>		// Now uses Ansi C++ Lib

#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <getopt.h>

#include "server.h"
#include "schedule.h"
#include "status.h"
#include "config.h"
#include "message.h"
#include "auto.h"
#include "level.h"
#include "client.h"
#include "gameobj.h"

using namespace std;

/* globale Variablen */
serverchan *sc;
levels *ls;
int lnum;
level al;
levelteile at;
clients clist;
int clientnum;
int status;
int starttime;
volatile unsigned int timepos;

objdyn *outbuf;

/* forward deklaration */
void pops_init();
void pops_start();
void pops_stop();
void pops_statuswechsel (int st);
void pops_setlevel (int num);
void pops_addclient (s_client *s);
void pops_delclient (s_client *s);
void pops_setoknull();
void pops_checkaende();
void pops_checklende();
void pops_assignnumbers();
void pops_acceptclient (client &c);
void pops_sendlevel (client &c);
void pops_calcstart();

void pops_calculate();
void pops_calcl();
void pops_send();

/* Init Exit Funktionen */
void pops_init()
{
 sc=NULL;
 status=STATUS_WAIT;
 clientnum=0;
}

void pops_exit()
{
 if (sc)
  stop_server(sc);
}

/* Message Handling */
void pops_rcvmsg (s_client *s,int g,int n,int a1,int a2)
{
 client *ac;

 if (g==G_NET) {
  // Neue Clients?
  switch (n) {
   case 1:
    cout << "Neuer Client\n";
    pops_addclient (s);
    break;
   case 2:
    cout << "Client Exit\n";
    pops_delclient (s);
    break;
  }

  return;
 }

 ac=clist.findclient(s);
 if (ac==NULL)	{
	cout << "Client not found?!? This shouldn't happen\n";
	return;
 }

 if (g==G_SETUP) {
  // Warten Status
  if (status==STATUS_WAIT) {
   switch (n) {
    // Anmeldung 1
    case 51: // Refreshrate... Kommt noch
     break;
    case 52: // Spielertyp
     game_objekt *to;
     switch (a1) {
	case TYPE_ZUSCHAUER:
		to=new zuschauer;
		clist.setobj(ac,to);
		break;
	case TYPE_SPIELER:
		to=new spieler;
		clist.setobj(ac,to);
		break;
	default:
		cout << "Unknown Type! Chose Zuschauer\n";
		to=new zuschauer;
		clist.setobj(ac,to);
     }
     cout << "Spielertyp:" << a1 << "\n";
     break;
    case 53: // naechste Client Stufe
     if (clientnum==0)
      ac->setmaster(true);
     else
      ac->setmaster(false);

     ac->setnum(clientnum);
     clientnum++;

     ac->setstatus(CSTATUS_ANMELDUNG2);

     pops_acceptclient (*ac);
     ((spieler*)(ac->obj))->setcar (ac->getnum() % autotypen::d.anz);
     serv_sendmsg_all (sc,G_SETUP,12,ac->getnum(),ac->getnum() % autotypen::d.anz);
     cout << "Client fully accepted! Nummer:" << ac->getnum() << "\n";
     break;

    // Anmeldung 2
    case 61: // Auto Auswahl
     ((spieler*)(ac->obj))->setcar (a1);
     serv_sendmsg_all (sc,G_SETUP,12,ac->getnum(),a1);
     cout << "Nummer " << ac->getnum() << " changed car to:" << a1 << "\n";
     break;
    case 62: // Level Auswahl
     if (ac->ismaster()) {
      pops_setlevel (a1);
      serv_sendmsgs_all (sc,G_SETUP,13,0,al.name);
      cout << "Level changed to:" << a1 << " - " << al.name << "\n";
     }
     break;
    case 63: // Alles Klar
     ac->setok(true);
     cout << "Nummer " << ac->getnum() << " is ready!\n";
     ac->setstatus(CSTATUS_LOAD1);
     pops_checkaende();
     break;
    default:
     cout << "Unbekannte Message: Setup," << n << "\n";
   }
   return;
  }

  // Lade Status
  if (status==STATUS_LOAD) {
   switch (n) {
    case 70:
     ac->setstatus(CSTATUS_LOAD2);
     break;
    case 75:
     if (ac->getstatus()==CSTATUS_LOAD2) {
      ac->setok(true);
      pops_checklende();
     }
     break;

    default:
     cout << "Unbekannte Message: Setup," << n << "\n";
   }

   return;
  }
 }

 if (status==STATUS_RUN) {
  switch (n) {
   case 80:	//Infos angefordert
	//kommt noch
	break;
  }
 }

 // Tastatur eingaben
 if (g==G_CONTROL) {
  ac->obj->contr (a2,a1);
  return;
 }
}

void pops_rcvmsgs (s_client *s,int g,int n,int a1,const char *a2)
{
 client *ac;

 ac=clist.findclient(s);
 if (ac==NULL) return;

 if (g==G_SETUP) {
  switch (n) {
   case 60: // Namen geändert
    ac->setname (a2);
    serv_sendmsgs_all (sc,G_SETUP,11,ac->getnum(),a2);
    cout << "Nummer " << ac->getnum() << " changed Name to " << a2 << "\n";
   break;
  }
 }
}

void pops_rcvmsgd (s_client *s,int g,int n,void *a1,int a2)
{
}

/* Netzwerk */
void pops_start(unsigned int port)
{
 int erg;

 erg=start_server (&sc, port);

 if (erg<0) {
  cerr << "Fehler beim Initialisieren des Servers:\n";
  switch (erg) {
   case -1: cerr << "   Kein Speicher mehr!\n"; break;
   case -2: cerr << "   Scoket Fehler! (Dostreiber installiert?)\n"; break;
   case -3: cerr << "   IOCTL Fehler! (Non Blocking Connection)\n";break;
   case -4: cerr << "   Bind Fehler. Adresse schon benutzt?\n";break;
  }
  pops_exit();
  exit(1);
 }

 serv_setacpt (sc,1);

 serv_set_rcvmsg (sc,pops_rcvmsg);
 serv_set_rcvmsgs (sc,pops_rcvmsgs);
 serv_set_rcvmsgd (sc,pops_rcvmsgd);
}

void pops_stop()
{
 stop_server (sc);
 sc=NULL;
}

/* Clients */
void pops_addclient (s_client *s)
{
 client *c=clist.makenew(s);

 c->init();
}

void pops_delclient (s_client *s)
{
 client *i;

 i=clist.findclient(s);
 if (i==NULL) {
	cout << "Falsche Nummer bei DelClient!\n";
	return;
 }

 if (i->getnum()==clientnum-1)
  clientnum--;

 //Hmm, should continue instead of interrupting game
 if (status!=STATUS_WAIT) {
  cout << "Client exited - change Status\n";
  pops_statuswechsel(STATUS_WAIT);
 }

 bool wasmaster=i->ismaster();
 clist.delclient(s);

 if (wasmaster) {
  cout << "Master Client exited! Looking for new master\n";
  i=clist.findclient(0);

  if (i!=NULL) {
   i->setmaster(true);
   serv_sendmsg (sc,i->geta(),G_SETUP,9,1,0); // Client ist jetzt Master
  } else {
   cout << "No Player found... Abort!\n";
  }
 }
}

void pops_acceptclient (client &s)
{
 // Client Accepted! Daten schicken!
 serv_sendmsg (sc,s.geta(),G_SETUP,10, s.ismaster() ? 1 : 0, s.getnum());
 serv_sendmsg (sc,s.geta(),G_SETUP,5, autotypen::getanz(),0);
 if (s.ismaster())
  serv_sendmsg (sc,s.geta(),G_SETUP,6, ls->getanz(),0);
 serv_sendmsgs (sc,s.geta(),G_SETUP,13, 0, al.name);

 clients::iterator i;
 for (i=clist.begin();i!=clist.end();i++) {
  if ((i->geta() != s.geta()) && i->isacpt()) {
   serv_sendmsgs (sc,s.geta(),G_SETUP,7, i->getnum(), i->getname());
   serv_sendmsg (sc,s.geta(), G_SETUP,8, i->getnum(), i->obj->gettyp());
  }
 }

 // Alle anderen Clients benachrichtigen
 serv_sendmsgs_all (sc,G_SETUP,7,s.getnum(),s.getname());
 serv_sendmsg_all (sc,G_SETUP,8,s.getnum(),s.obj->gettyp());
}

/* Status wechsel */
int pops_getstatus()
{
 return status;
}

void pops_statuswechsel(int st)
{
 if (status==st)
  return;

 status=st;

 pops_setoknull();

 switch (status) {
  case STATUS_WAIT:
   cout << "Status wechsel - Warten\n";
   serv_setacpt (sc,1);
   break;
  case STATUS_LOAD:
   cout << "Status wechsel - Laden\n";
   serv_setacpt (sc,0);
   pops_calcstart();
   break;
  case STATUS_RUN:
   cout << "Status wechsel - Spiel\n";
   break;
  case STATUS_PAUSE:
   cout << "Status wechsel - Pause\n";
   break;
  case STATUS_ERG:
   cout << "Status wechsel - Ergebnis\n";
   break;
 }

 // Clients benachrichtigen
 serv_sendmsg_all (sc,G_SETUP,1,st,0);
}

void pops_setlevel(int num)
{
 level *l;

 if (status==STATUS_WAIT)
  {
   l = ls->getnum(num);
   if (!l)
    return;

   lnum=num;
   al=*l;
  }
}

void pops_calcstart()
{
 cout << "Berechne Startpositionen...";
 cout.flush();

 int ox,oy;
 al.startp(ox,oy);
 int xp=ox,yp=oy;

 clients::iterator i;
 for (i=clist.begin();i!=clist.end();i++) {
  i->obj->setpos (xp,yp);
  i->obj->setrot (0);
  // Nächste Reihe?
  xp+=70;
  if (xp-ox>=70*3) {
	xp=ox;
	yp+=100;
  }
 }

 starttime=4*FPS;
 cout << "OK!\n";
}

void pops_setoknull()
{
 clients::iterator i;
 for (i=clist.begin();i!=clist.end();i++)
  i->setok(false);
}

void pops_checkaende()
{
 clients::iterator i;

 for (i=clist.begin();i!=clist.end();i++)
  if (!i->isok())
   break;

 if (i!=clist.end())
  return;

 pops_statuswechsel(STATUS_LOAD);
}

void pops_checklende()
{
 clients::iterator i;

 for (i=clist.begin();i!=clist.end();i++)
  if (!i->isok())
   break;

 if (i!=clist.end())
  return;

 pops_statuswechsel(STATUS_RUN);
}

void pops_calculate()
{
 // Noch beim Start?
 if (starttime>=0) {
	if (starttime==4*FPS) {
		cout << "Starttime 2\n";
		serv_sendmsgs_all(sc,G_SETUP,20,11,"");
	} else if (starttime==2*FPS) {
		serv_sendmsgs_all(sc,G_SETUP,20,12,"");
		cout << "Starttime 1\n";
	} else if (starttime==0) {
		serv_sendmsgs_all(sc,G_SETUP,20,13,"");
		cout << "Starttime 0 -> Das Spiel Startet\n";
	}
	starttime--;
 }

 clist.calc();
}

void pops_calcl()
{
 clients::iterator i;

 for (i=clist.begin();i!=clist.end();i++)
  i->lcalc();
}

void pops_send()
{
 objdyn *tbuf=outbuf;

 clients::iterator i;
 for (i=clist.begin();i!=clist.end();i++) {
  *tbuf=i->obj->dyn;
  tbuf++;
 }
 serv_sendmsgd_all (sc,G_PHYSIK,0,(void*)outbuf,sizeof(objdyn) * clist.size());
}

void pops_calcnet()
{
 calcserv (sc);
}

void pops_allocoutbuf()
{
 outbuf=new objdyn[clist.size()];
}

void pops_freeoutbuf()
{
 if (outbuf)
  delete outbuf;
 outbuf=NULL;
}

void pops_exitserver()
{
 cout << "Beende Server...";
 cout.flush();
 pops_stop();
 cout << "OK\n";
 cout << "\n                    PoP ist ein Projekt vom Grundkurs Info 2000 am Johanneum\n";
}

// Globale Config Vars
int FPS;

int main(int argc,char **argv)
{
 unsigned int port=STDPORT;
 int c;
 FPS=60;	// Nur für LAN geeignet

 cout << "				* Pop Server V" << VERSION << " *\n";

 // Kommandozeilencheck
 opterr=0;

 while ((c = getopt (argc, argv, "hp:f:")) != -1)
	switch (c) {
		case 'f':
			FPS = atoi(optarg);
			if (FPS<1 || FPS>100) {
				cerr << "FPS nicht im bereich 5-100\n";
				return 1;
			}
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'h':
			cerr << "Aufruf:\n";
			cerr << "  popserver [-f FPS] [-p PORT]\n";
			return 1;
		case '?':
			if (isprint (optopt))
				cerr << "Unknown Option -" << (char) optopt << "\n";
			else
				cerr << "Unknown Option Character " << optopt << "\n";
			return 1;
		default:
			abort();
	}

 // NonOption Argumente werden jetzt mal nicht behandelt...

 cout << "\nLade Levels...\n";
 ls=new levels;
 if (ls->load(PFAD_LEVEL) < 0 || ls->getanz()==0) {
  cerr << "Fehler beim Laden der Level!\n";
  cerr << "Bitte Überprüfen: " << PFAD_LEVEL << "\n";
  exit (1);
 }
 cout << "OK!\n";
 al=*(ls->getnum(0));
 cout << "Anzahl Levels: " << ls->getanz() << "\n";

 cout << "Lade Levelteile...";
 cout.flush();
 if (at.load(PFAD_LEVELTEILE)) {
  cerr << "Fehler beim Laden der Levelteile!\n";
  cerr << "Bitte Überprüfen: " << PFAD_LEVELTEILE << "\n";
  exit(1);
 }
 cout << "OK!\n";

 cout << "Lade Autotypen...";
 cout.flush();

 if (autotypen::load(PFAD_AUTOGFX)) {
  cerr << "Fehler beim Laden der Autotypen!\n";
  cerr << "Bitte Überprüfen: " << PFAD_AUTOGFX << "\n";
  exit(1);
 }
 cout << "OK!\n";
 cout << "Anzahl Autotypen: " << autotypen::getanz() << "\n";

 cout << "Initialisiere Socket...";
 cout.flush();
 pops_init();
 pops_start(port); // Socket binden
 outbuf=NULL;
 cout << "OK!\n";
 cout << "Server gestartet - Port: " << port << "   FPS: " << FPS << "\n";

 pops_run();

 // Hier sollte das Programm nie ankommen!
 return 13;
}
