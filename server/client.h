/***************************************************************************
                          client.h  -  description
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

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <vector>

#include <stdio.h>
#include "fixed.h"
#include "message.h"
#include "level.h"
#include "auto.h"
#include "gameobj.h"
#include "net.h"

#define CSTATUS_ANMELDUNG1 0
#define CSTATUS_ANMELDUNG2 1
#define CSTATUS_LOAD1	   2
#define CSTATUS_LOAD2	   3
#define CSTATUS_RUN	   4
#define CSTATUS_PAUSE	   5
#define CSTATUS_ERGEBNIS   6

class client;

class clients
{
public:
	// Add/Del Client
	client *makenew(s_client *adr);
	void delclient (s_client *adr);

	// Iterator
	typedef vector<client>::iterator iterator;
	typedef vector<game_objekt *>::iterator iteratoro;

	// Find Client funcs
	client *findclient(s_client *adr);
	client *findclient(int i);
	iterator begin()	{ return clist.begin(); }
	iterator end()	{ return clist.end(); }
	iteratoro begino() { return obj.begin(); }
	iteratoro endo()	{ return obj.end(); }

	// Algemein
	void calc();
	int size();
	void setobj(client *c,game_objekt *o);
	
protected:
	vector<client> clist;
	vector<game_objekt *> obj;
};

class game_objekt;

class client
{
 public:
  client();
  ~client();

  void init() {}

  void setstatus (int st);
  int getstatus();
  void setobj (game_objekt *o) { obj=o; }

  void lcalc();
  void setname(const char *n);
  const char *getname();
  void setnum(int n);
  int getnum();
  void setmaster(bool m);
  bool ismaster();
  void setok(bool o);
  bool isok();
  void sendmsg(int num,const char *msg);
  s_client *geta()	{ return a; }
  void seta(s_client *ad) { a=ad; }

  //s_client *a;
  // Oh shit, why must everything be depent on something other?
  game_objekt *obj;
 protected:
  s_client *a;

 private:
  int num;
  char name[255];
  int cstatus;
  int stat2;
  bool master;
  bool ok;
};

#endif
