/***************************************************************************
                          level.cc  -  description
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
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fstream.h>
#include <math.h>

#include "level.h"

//#define STOSS_DEBUG

// ---- Class Level ------------------------------------

int level::load(const char *file)
{
 ifstream *is=new ifstream (file);

 if (!is->good())
  return -1;
 is->read(this, sizeof(level));

 if (!is->good()) {
  is->close();
  return -2;
 }

 is->close();

 if (b<0 || b>=LEVELMAXX || h<0 || h>=LEVELMAXY) {
  cout << "B,h\n" << b << "," << h << "\n";
  return -3;
 }

 // Namen berechnen...
 int p1,p2;
 p1=strlen(file)-1;
 p2=0;
 for (int p=strlen(file)-1;p>=0;p--) {
	if (file[p]=='.') p1=p;
	if (file[p]=='/' || file[p]=='\\') {
		p2=p;
		break;
	}
 }
 name=new char[p1-p2];
 strncpy (name,file+p2+1,p1-p2-1);
 name[p1-p2-1]='\0';

 return 0;
}

int level::save(const char *file)
{
 ofstream *os=new ofstream (file);

 if (!os->good())
  return -1;

 os->write(this, sizeof(level));

 if (!os->good()) {
  os->close();
  return -2;
 }

 os->close();

 return 0;
}

int level::anzdata()
{
 if ( ((b*h) % LEVELTRANS) != 0)
  return ((b*h)/LEVELTRANS)+1;
 else
  return ((b*h)/LEVELTRANS);
}

void level::getdata(int anz, strtyp *buf)
{
 int x;
 int y;
 int bpos=0;

 if (anz>0) {
  for (y=(b*h) / (LEVELTRANS*anz);y<h;y++) {
   for (x=(b*h) % (LEVELTRANS*anz);x<b;x++) {
    buf[bpos]=l[x][y];
	  bpos++;
    if (bpos>=LEVELTRANS) return;
   }
  }
 } else {
  for (y=0;y<h;y++) {
   for (x=0;x<b;x++) {
    buf[bpos++]=l[x][y];
    if (bpos>=LEVELTRANS) return;
   }
  }
 }
}

inline void swap(int &n1,int &n2)
{
	int t=n1;
	n1=n2;
	n2=t;
}

inline int linekreuz (int x1,int y1,int x2,int y2,float xp,float yp,
	float ox,float oy)
{
 float dx,dy,odx,ody;

 if (x1>x2) dx=x1-x2; else dx=x2-x1;
 if (y1>y2) dy=y1-y2; else dy=y2-y1;
 if (xp>ox) odx=xp-ox; else odx=ox-xp;
 if (yp>oy) odx=yp-oy; else ody=oy-yp;

 return 0;
}

#define RELATIV	10	// auf 10 Pixel ungenau rechnen (sonst kann man ¸berall
				// durch die W‰nde fahren :)

// Seite einer Linie bestimmen (rechts/ links)
inline int getsitel( int x1,int y1,int x2,int y2,float xp,float yp)
{
 int dx,dy;

 // Deltas berechnen
 if (x1>x2)
  dx=x1-x2; else dx=x2-x1;
 if (y1>y2)
  dy=y1-y2; else dy=y2-y1;

 if (dx>dy) {// Taktik 1 -> horizontal
  float m; // Geradengleichung: y=m*x+b
  float b;

  // ‹berhaupt testen?
  if (x1<x2) {
 	if (xp+RELATIV<x1 || xp-RELATIV>x2) return 0;
  } else {
	if (xp+RELATIV<x2 || xp-RELATIV>x1) return 0;
  }

  if (x2>x1) {
   b=y1;
   m=dy?dx/(y2-y1):0;
  } else {
   b=y2;
   m=dy?dx/(y1-y2):0;
  }
  if ((m*xp+b)>yp)
   return 1;
  else
   return -1;
 } else { // Taktik 2 -> vertikal
  float m; // Geradengleichung: x=m*y+b
  float b;

  // ‹berhaupt testen?
  if (y1<y2) {
	if (yp+RELATIV<y1 || yp-RELATIV>y2) return 0;
  } else {
	if (yp+RELATIV<y2 || yp-RELATIV>y1) return 0;
  }

  if (y2>y1) {
   b=x1;
   m=dx?dy/(x2-x1):0;
  } else {
   b=x2;
   m=dx?dy/(x1-x2):0;
  }
  if ((m*yp+b)>xp)
   return 1;
  else
   return -1;
 }
}

// Seite eines Kreises bestimmen (innerhalb/auﬂerhalb)
inline int getsitek(int xp, int yp, int rad, int t, float x, float y)
{
	float dx,dy;
	float alpha;

	dx=x- xp;
	dy=yp- y;	// Achtung, ykoordinate jetzt mathematisch von unten nach oben!
			// Macht aber das Denken in Winkeln leichter :)
	// Spezialf‰lle
	if (dx==0) {
		if (dx>rad) return 1; else return -1;
	}

	// Testen obs in den Bereich des Kreis f‰llt....
	alpha=atan(dy/dx);
	// Falls sich das Auto links vom Mittelpunkt befindet
	// muﬂ der Winkel angepaﬂt werden, da tan nur im Bereich -90 - 90 grad
	// liefert
	if (dx<0) alpha-=M_PI;

	if (alpha<0) 
		alpha+=M_PI*2;
	if (alpha<0 || alpha>M_PI*2) {
		cout << "AError: %f" << alpha;
	}
	// Etwas mehr Spielraum lassen, so daﬂ grenzen zeischen Objekten funktionieren
	#define ADDSPACE	((float)RELATIV/50)
	switch (t) {
		case geom_kreisro: if (alpha>(M_PI/2)+ADDSPACE) return 0; break;
		case geom_kreislo: if (alpha>M_PI+ADDSPACE) return 0; if (alpha+ADDSPACE<(M_PI/2)) return 0; break;
		case geom_kreislu: if (alpha>(3*M_PI/2)+ADDSPACE) return 0; if (alpha+ADDSPACE<M_PI) return 0; break;
		case geom_kreisru: if (alpha+ADDSPACE<(3*M_PI/2)) return 0; break;
		default: break;
	}

	//printf ("X:%f Y:%f TP%d W:%3.0f S:%3.3f",dx,dy,t,alpha*180/M_PI,(float)dy/dx);
	
	// Innen/Auﬂen?
	float len=sqrt(dx*dx + dy*dy);
	if (len<0 || len>1000) {
		cout << "Len?!? " << len << "\n";
	}
	if ((int)len>rad) {
		return 1;
	} else
		return -1;
}

int level::stoss(float ox,float oy,float nx,float ny)
{
 // Koordinaten des Levelteils berechnen
 int xk=((int)nx)/LEVEL_NORMTB;
 int yk=((int)ny)/LEVEL_NORMTH;
 ox-=xk*LEVEL_NORMTB;
 nx-=xk*LEVEL_NORMTB;
 oy-=yk*LEVEL_NORMTH;
 ny-=yk*LEVEL_NORMTH;

 // Kein Stoﬂ ¸ber Teilgrenzen hinweg
 //if (ox<0 || oy<0) return false;

 strteil *t=&(at.s.t[l[xk][yk]]);
 int s1,s2;
 for (int i=0;i<t->anzg;i++) {
  strgeom *g=&(t->gobj[i]);
  if (g->typ==geom_linie) {
   s1=getsitel(g->x1,g->y1,g->x2,g->y2,ox,oy);
   s2=getsitel(g->x1,g->y1,g->x2,g->y2,nx,ny);
   if (s1!=s2 && s1!=0 && s2!=0) {
    if (i==0 && (t->flags & 3)) {
	//Checkpoint?
	for (int a=0;a<6;a++) {
		if (cpoints[a].x==xk && cpoints[a].y==yk)
			return 2+a;
	}
    }
    return 1;
   }
  } else {
   s1=getsitek(g->x1,g->y1,g->x2,g->typ,ox,oy);
   s2=getsitek(g->x1,g->y1,g->x2,g->typ,nx,ny); 
   #ifdef STOSS_DEBUG
   if (s1!=0 || s2!=0)
	cout << g->typ << s1+1 << s2+1 << ":";
   #endif
   if (s1!=s2 && s1!=0 && s2!=0) {
      #ifdef STOSS_DEBUG
	cout << "Kreisstoﬂ: x1:" << g->x1 << " y1:" << g->y1 << " radius:" << g->x2 << " Typ:" << g->typ << "\n";
      #endif
	return 1;
   }
  }
 }

 return 0;
}

void level::startp(int &xs,int &ys)
{
 for (int y=0;y<h;y++)
	for (int x=0;x<b;x++) {
		strteil *t=&(at.s.t[l[x][y]]);
		// Startpunkt gefunden...
		if (t->flags & 1) {
			//Noch etwas rumrechnen und festsetzen
			xs=x*LEVEL_NORMTB+220;
			ys=y*LEVEL_NORMTH+100;
		}
	}
}

// ---- Class Levels -----------------------------------

int levels::load(const char *dir)
{
 DIR *d;
 struct dirent *de;
 level tlevel;
 char name[500];

 if ( (d=opendir(dir)) == NULL)
  return -1;

 while ((de=readdir(d))!=NULL) {
  name[0]='\0';
  strcat ((char *) &name,dir);
  strcat ((char *) &name,(char *) &de->d_name);
  cout << "Lade Level: " << (char *) name << " - ";
  if ( (tlevel.load((char *) name)) >= 0) {
   cout << " OK";
   l.push_back (tlevel);
  } else cout << " Fehler";
  cout << "\n";
 }

 if (closedir(d) < 0)
  return -2;

 return 0;
}

level *levels::getnum (int num)
{
 int a=0;
 list<level>::iterator i;

 for (i=l.begin(); i!=l.end() && a<num;i++,a++);

 if (i==l.end())
  return NULL;

 return &(*i);
}

int levels::getanz()
{
 return l.size();
}

// ---- Class Levelteile --------------------------------------

bool levelteile::load (const char *dir)
{
  return level_strload (&s, dir);
}
