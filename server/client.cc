/***************************************************************************
                          client.cc  -  description
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

#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <math.h>

#include "server.h"
#include "client.h"

// Class Client
client::client()
{
 num=0;
 strcpy (name,"");
 cstatus=CSTATUS_ANMELDUNG1;
 stat2=0;
 master=false;
 ok=false;
 accepted=false;

 obj=NULL;
 a=NULL;
}

client::~client()
{
}

void client::setstatus(int st)
{
 if (cstatus==st)
  return;

 cstatus=st;
 if (st==CSTATUS_LOAD2)
  stat2=0;
}

int client::getstatus()
{
 return cstatus;
}

void client::lcalc()
{
 strtyp buf[LEVELTRANS];

 if (cstatus==CSTATUS_LOAD2) {
  if (stat2<al.anzdata()) {
   if (stat2==0)
    serv_sendmsg (sc,a,G_SETUP,21,al.b,al.h);
   al.getdata (stat2,buf);
   stat2++;
   serv_sendmsgd (sc,a,G_SETUP,22,buf,LEVELTRANS*sizeof(strtyp));
  }
 }
}

void client::setnum(int n)
{
 num=n;
}

int client::getnum()
{
 return num;
}

void client::setmaster(bool m)
{
 master=m;
}

bool client::ismaster()
{
 return master;
}

void client::setok(bool o)
{
 ok=o;
}

bool client::isok()
{
 return ok;
}

void client::setname (const char *n)
{
 if (strlen(n)<255)
  strcpy (name,n);
 else
  cout << "Achtung! Name zu lang!\n";
}

const char *client::getname()
{
 return name;
}

void client::sendmsg(int mnum,const char *msg)
{
 serv_sendmsgs (sc,a,G_SETUP,20,mnum,msg);
}

// Class Clients

client *clients::findclient(s_client *adr)
{
	vector<client>::iterator i;
	for (i=clist.begin();i!=clist.end();i++) {
		if (i->geta()==adr) break;
	}
	if (i==clist.end())
		return NULL;

	return i;
}

client *clients::findclient(int i)
{
	return &(clist[i]);
}

client *clients::makenew(s_client *adr)
{
	client *tc=new client;

	clist.push_back(*tc);
	obj.resize(clist.size());
	delete tc;
	tc=(clist.end()-1);
	tc->seta(adr);
	return (clist.end()-1);
}

void clients::delclient(s_client *adr)
{
	unsigned int i;

	for (i=0;i<clist.size();i++) {
		if (clist[i].geta()==adr) {
			clist.erase( &(clist[i]) );
			obj.erase( &(obj[i]) );
			break;	// Hmm, should search for others...
		}
	}
}

int clients::size()
{
	return clist.size();
}

void clients::calc()
{
	iteratoro i;

	// Schritt 1
	for (i=begino();i!=endo();i++) {
		(*i)->calcpos();
	}

	// Schritt 2:
	for (i=begino();i!=endo();i++) {
		(*i)->calccoll(&obj);
	}
}

void clients::setobj(client *c,game_objekt *o)
{
	unsigned int i;

	for (i=0;i<clist.size();i++) {
		if ( &(clist[i])==c ) {
			obj[i]=o;
			c->obj=o;
			o->setclient(c);
		}
	}
}
