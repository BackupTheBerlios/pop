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

#include "message.h"
#include "level.h"
#include "auto.h"

#define CSTATUS_ANMELDUNG1 0
#define CSTATUS_ANMELDUNG2 1
#define CSTATUS_LOAD1	   2
#define CSTATUS_LOAD2	   3
#define CSTATUS_RUN	   4
#define CSTATUS_PAUSE	   5
#define CSTATUS_ERGEBNIS   6

typedef struct {
  unsigned short xp,yp;
  fix richtung;
} objdyn;

class game_objekt
{
 public:
  objdyn dyn;

      game_objekt();
  int xp();
  int yp();
  virtual void calc();
  virtual void contr(int num,int a);
  virtual void setpos(unsigned short x,unsigned short y);
  virtual void setrot(fix r);
};

class zuschauer : public game_objekt
{
 public:
  float fx,fy;

       zuschauer();
  void calc();
  void contr(int num,int a);
};

class spieler : public game_objekt
{
public:
  autodaten *car;
  float f,v;
  float px,py,richtung;
  float drehung;
  int msg;

       spieler();
  void setcar(int num);

  void calc();
  void contr(int num,int a);
  virtual void setpos(unsigned short x,unsigned short y);
  virtual void setrot(fix r);
};

class client
{
 public:
  client();
  ~client();

  void init() {}

  void initobj (int typ);
  void setstatus (int st);
  int getstatus();

  void selectcar(int num);
  void contr(int num,int a) { obj->contr(num,a); }
  void calc() { obj->calc(); }
  void lcalc();
  void send(objdyn *datao);
  void setname(const char *n);
  const char *getname();
  void setnum(int n);
  int getnum();
  void setres(int xr,int yr);
  int getobjtyp();
  void setmaster(bool m);
  bool ismaster();
  void setok(bool o);
  bool isok();

  game_objekt *obj;
  s_client *a;

 private:
  int num;
  int resx,resy;
  char name[255];
  int cstatus;
  int stat2;
  int objtyp;
  bool master;
  bool ok;
};

// globale Vars
extern level al;

#endif
