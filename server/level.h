/***************************************************************************
                          level.h  -  description
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

#ifndef __LEVELDAT_H__
#define __LEVELDAT_H__

#include <fstream.h>
#include <list>

#include "config.h"
#include "strteile.h"

/*******************
 * Datenstrukturen *
 *******************/

// Normale Breite und H�he eines Teils
#define LEVEL_NORMTB	640
#define LEVEL_NORMTH	480

class levelteile
{
 public:
  strteile s;

  bool load(const char *dir);
};

typedef unsigned char strtyp;

typedef struct
{
	unsigned short x,y;
} pos;

class level
{
public:
 int b,h;
 pos cpoints[6];
 int res[20]; // Reserviert f�r Erweiterungen (nicht l�schen!)
 char *name;  // Achtung! Ist ein Pointer immer 4 Bytes gro�?
 strtyp l[LEVELMAXX][LEVELMAXY];

 int load(const char *file);
 int save(const char *file);

 // Funktionen zum �bertragen des Levels in Paketen
 int anzdata();
 void getdata(int anz,strtyp *buf);

 //�berpr�fen des Levels
 int stoss(float ox,float oy,float nx,float ny,bool cpc=true);

 //Gibt Position des Startpunkts zur�ck
 void startp(int &x,int &y);
};

class levels
{
public:
 list<level> l;

 int load(const char *dir);

 level *getnum(int num);
 int getanz();
};

extern level al;
extern levelteile at;

#endif
