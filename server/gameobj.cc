// gameobj.cc

#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include "client.h"
#include "gameobj.h"
// Shit should remove this, but need some few global vars
#include "server.h"
#define   RELATIV 5
#include "collcode.cc"

game_objekt::game_objekt()
{
	cl=NULL;
	dyn.xp=0;
	dyn.yp=0;
	dyn.richtung=0;
}

void game_objekt::setclient(client *c)
{
	cl=c;
}

int game_objekt::gettyp()
{
	return TYPE_NONE;
}

void game_objekt::calcpos()
{
}

void game_objekt::calccoll(vector<game_objekt *> *)
{
}

void game_objekt::contr(int num,int a)
{
}

void game_objekt::setpos(unsigned short x,unsigned short y)
{
 dyn.xp=x;
 dyn.yp=y;
}

void game_objekt::setrot(fix r)
{
 dyn.richtung=r;
}

void game_objekt::sendmsg(int num,const char *msg)
{
 cl->sendmsg(num,msg);
}

void game_objekt::getstatus(char *msg)
{
	strcpy (msg,"");
}

spieler::spieler()
{
 car=NULL;
 v=f=0;
 px=py=richtung=0;
 nx=ny=nrichtung=0;
 drehung=0;
 acp=0;
 round=0;
 fertig=false;
 car = autotypen::gettyp(0);
}

void spieler::setcar(int num)
{
 car = autotypen::gettyp(num);
}

void spieler::setpos(unsigned short x,unsigned short y)
{
 nx=x;ny=y;
}

void spieler::setrot(fix r)
{
 drehung= r * (M_PI/128);
}

#define km	10

float inline abs(float a)
{
	return a<0 ? -a : a;
}

#define vmax	(840/FPS)
#define kkonst	2
#define DKONST	0.2

void spieler::checkcp()
{
 int s= al.stoss(px,py,nx,ny,true);

  // Rundenzählen
 if (s>=2) {
	if (s-2==acp) {
		acp++;
		if (acp==6) {
			round++;
			if (round==2) {
				sendmsg(21,"letzte");
			} else if (round==3) {
				sendmsg(21,"fertig");
				fertig=timepos;
			} else {
				sendmsg(21,"");
			}
			acp=0;
		} else {
			sendmsg(20,"");	// Checkpoint
		}
	} else if (s-2==(acp+5)%6) {
		if (acp==0) {
			round--;
		}
		acp=(acp+5)%6;
		sendmsg(22,"");	// Wrong way
	} else {
		cout << "Fehlerhaftes Leveldesign!!!\n";
	}
 }
}

int inline spieler::checkstoss(float xp,float yp,float xn,float yn)
{
 int s= al.stoss(xp,yp,xn,yn,false);

 // Stoss beachten?
 if (s==1) {
  // angestoßen
  v=-(v/2);
  nx = px + (3/2) * sin(richtung) *v;
  ny = py - (3/2) * cos(richtung) *v;
  return 1;
 }

 return 0;
}

void inline spieler::calcpos()
{
 if (starttime<=0) { // Rennen schon gestartet

 if (fertig) {
      // Alle Runden beendet => abbremsen
	v-=0.125;
	if (v<0) v=0;
 } else {
  if (f>0) { // beschleunigen
	  if (v>=vmax)
		v=vmax-0.0001;
	  float t= log(vmax/(vmax-v))/kkonst;
	  t+=(float)1/FPS;
	  v=vmax*(1-exp(-kkonst*t));
  } else if (f<0) { //bremsen
	  v += f;
	  v += ((-abs(v))/km)*(-f);
  } else {
	  v -= ((v)/km)*0.1;
  }
 }

 richtung = nrichtung;
 px=nx;
 py=ny;
 nrichtung +=  drehung * (M_PI/(1.5*FPS)); // 10 Grad Schritte...
 nx += sin(nrichtung)*v;
 ny -= cos(nrichtung)*v;

 if (nx<0) nx=0;
 if (nx>al.b * 640) nx=al.b*640;
 if (ny<0) ny=0;
 if (ny>al.h * 480) ny=al.h*480;
 }
}

void spieler::setnpos()
{
 dyn.xp = (unsigned short int) nx;
 dyn.yp = (unsigned short int) ny;
 dyn.richtung = nrichtung * (128/M_PI);
}

void spieler::getcorners(float xp,float yp,float winkel,float x[4],float y[4])
{
	//normale Ecken
	x[0]=xp-(car->b/2);
	y[0]=yp-(car->h/2);
	x[1]=xp+(car->b/2);
	y[1]=yp-(car->h/2);
	x[2]=xp-(car->b/2);
	y[2]=yp+(car->h/2);
	x[3]=xp+(car->b/2);
	y[3]=yp+(car->h/2);

	//rotieren (hmm, could use this gnu addition, with sin+cos with 1 function
	// but this doesn't exist in cygwin...
	float cosw=cos(winkel);
	float sinw=sin(winkel);
	for (int i=0;i<4;i++) {
		x[i]-=xp; y[i]-=yp;
		float xt=x[i];
		x[i]=x[i]*cosw-y[i]*sinw;
		y[i]=xt*sinw+y[i]*cosw;
		x[i]+=xp; y[i]+=yp;
	}
}

void spieler::calccoll(vector <game_objekt *> *objs)
{
	// 4 Ecken berechnen
	float xb[4],yb[4];
	float xn[4],yn[4];

	// andere Autos (erster dirty Hack...)
	getcorners(px,py,richtung,xb,yb);
	getcorners(nx,ny,nrichtung,xn,yn);
	vector<game_objekt *>::iterator i;
	for (i=objs->begin();i!=objs->end();i++) {
		if ((*i)->gettyp()==TYPE_SPIELER) {
			spieler *s=(spieler *)*i;
			if (s==this) continue;
			float sxn[4],syn[4];
			getcorners(s->nx,s->ny,s->nrichtung,sxn,syn);
			for (int i1=0;i1<4;i1++) {
				for (int i2=0;i2<4;i2++) {
					int s1=getsitel(sxn[i2],syn[i2],sxn[(i2+1)%4],syn[(i2+1)%4],xb[i1],yb[i1]);
					int s2=getsitel(sxn[i2],syn[i2],sxn[(i2+1)%4],syn[(i2+1)%4],xn[i1],yn[i1]);
					if (s1!=s2 && s1!=0 && s2!=0) {
						float t=v;
						v=s->v;
						s->v=t;
						calcpos();
						s->calcpos();
						//Schleifen abbrechen
						goto schlend;
					}
				}
			}
		}	
	}
	schlend:

	// Streckenränder
	for (int i=0;i<4;i++) {
		if (checkstoss(xb[i],yb[i],xn[i],yn[i]))
			break;
	}

	// Checkpoints?
	checkcp();

	setnpos();
}

void spieler::getstatus(char *msg)
{
	sprintf (msg,"%s - Runde %d",cl->getname(),round);
}

int spieler::gettyp()
{
	return TYPE_SPIELER;
}

void spieler::contr(int num,int a1)
{
 switch (num) {
  case C_GAS_ON:
   f=0.125;
   break;
  case C_GAS_OFF:
   if (f==0.125)
    f=0;
   break;
  case C_BREMS_ON:
   f=-0.125;
   break;
  case C_BREMS_OFF:
   if (f==-0.125)
    f=0;
   break;
  case C_LINKS_ON:
   drehung=-1;
   break;
  case C_LINKS_OFF:
   if (drehung==-1)
    drehung=0;
   break;
  case C_RECHTS_ON:
   drehung=1;
   break;
  case C_RECHTS_OFF:
   if (drehung==1)
    drehung=0;
   break;
  default:
   cout << "Unknown Control:" << num << "\n";
 }
}

zuschauer::zuschauer()
{
 fx=fy=0;
}

int zuschauer::gettyp()
{
	return TYPE_ZUSCHAUER;
}

void zuschauer::calcpos()
{
 if (-(fx)<=dyn.xp && dyn.xp+fx< al.b*640)
  dyn.xp+=(int)fx;
 if (-(fy)<=dyn.yp && dyn.yp+fx< al.h*480)
  dyn.yp+=(int)fy;
}

#define Z_SPEED	((float)720/FPS)

void zuschauer::contr(int num,int a)
{
 switch (num) {
  case C_GAS_ON:
   fy=-Z_SPEED;
   if (fx==Z_SPEED)
    dyn.richtung=32;
   else if (fx==-Z_SPEED)
    dyn.richtung=224;
   else
    dyn.richtung=0;
   break;
  case C_GAS_OFF:
   if (fy==-Z_SPEED)
    fy=0;
   if (fx==Z_SPEED)
    dyn.richtung=64;
   else if (fx==-Z_SPEED)
    dyn.richtung=192;
   break;
  case C_BREMS_ON:
   fy=Z_SPEED;
   if (fx==Z_SPEED)
    dyn.richtung=96;
   else if (fx==-Z_SPEED)
    dyn.richtung=160;
   else
    dyn.richtung=128;
   break;
  case C_BREMS_OFF:
   if (fy==Z_SPEED)
    fy=0;
   if (fx==Z_SPEED)
    dyn.richtung=64;
   else if (fx==-Z_SPEED)
    dyn.richtung=192;
   break;
  case C_LINKS_ON:
   fx=-Z_SPEED;
   if (fy==-Z_SPEED)
    dyn.richtung=224;
   else if (fy==Z_SPEED)
    dyn.richtung=160;
   else
    dyn.richtung=192;
   break;
  case C_LINKS_OFF:
   if (fx==-Z_SPEED)
    fx=0;
   if (fy==-Z_SPEED)
    dyn.richtung=0;
   else if (fy==Z_SPEED)
    dyn.richtung=128;
   break;
  case C_RECHTS_ON:
   fx=Z_SPEED;
   if (fy==-Z_SPEED)
    dyn.richtung=32;
   else if (fy==Z_SPEED)
    dyn.richtung=96;
   else
    dyn.richtung=64;
   break;
  case C_RECHTS_OFF:
   if (fx==Z_SPEED)
    fx=0;
   if (fy==-Z_SPEED)
    dyn.richtung=0;
   else if (fy==Z_SPEED)
    dyn.richtung=128;
   break;
  default:
   cout << "Unknown Control " << num << "\n";
 }
}
