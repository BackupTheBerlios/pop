// gameobj.cc

#include <stdio.h>
#include <math.h>

#include "client.h"
#include "gameobj.h"
// Shit should remove this, but need some few global vars
#include "server.h"

/* Client Klasse */
client::client()
{
 num=0;
 strcpy (name,"");
 cstatus=CSTATUS_ANMELDUNG1;
 stat2=0;
 objtyp=0;
 master=false;
 ok=false;

 obj=NULL;
 a=NULL;
}

client::~client()
{
 if (obj)
  delete obj;
}

void client::initobj (int typ)
{
 if (typ<0 || typ>1) {
  cout << "Achtung! Fehlerhafter Objekttyp!\n";
  return;
 }
 if (obj) {
  cout << "Achtung! Objekt bereits vorhanden. Lösche...\n";
  delete obj;
 }
 if (typ==0)
  obj=(game_objekt *) new zuschauer(this);
 if (typ==1)
  obj=(game_objekt *) new spieler(this);
 objtyp=typ+1;
}

int client::getobjtyp()
{
 return objtyp-1;
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

void client::selectcar (int num)
{
 if (objtyp==2)
  ((spieler *) obj)->setcar (num);
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

void client::send (objdyn *datao)
{
 if (obj)
  memcpy ((void*) datao,(void*) &obj->dyn,sizeof(objdyn));
 else
  cout << "Achtung! Kein Objekt gesetzt!!! Das sollte NIE passieren!\n";
}

void client::sendmsg(int num,const char *msg)
{
 serv_sendmsgs (sc,a,G_SETUP,20,num,msg);
}

game_objekt::game_objekt(client *c)
{
 dyn.xp=0;
 dyn.yp=0;
 dyn.richtung=0;
 cl=c;
}

void game_objekt::calcpos()
{
}

void game_objekt::calccoll(const vector<game_objekt>)
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

spieler::spieler(client *c)
 : game_objekt (c)
{
 v=f=0;
 px=py=richtung=0;
 acp=0;
 round=0;
}

void spieler::setcar(int num)
{
 car = autotypen::gettyp(num);
}

void spieler::setpos(unsigned short x,unsigned short y)
{
 px=x;py=y;
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

extern int starttime; // Hack

#define vmax	7
#define kkonst	0.8
#define DKONST	0.2

void inline spieler::checkstoss()
{
 int s= al.stoss(px,py,nx,ny);

 // Rundenzählen
 if (s>=2) {
	if (s-2==acp) {
		acp++;
		if (acp==6) {
			round++;
			cout << "Runde " << round << " beendet!\n";
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
			cout << "Runde weniger!\n";
			round--;
		}
		acp=(acp+5)%6;
		sendmsg(22,"");	// Wrong way
	} else {
		cout << "Fehlerhaftes Leveldesign!!!\n";
	}
 }

 // Stoss beachten?
 if (s==1) {
  // angestoßen
  v=-(v/2);
  px += sin(richtung) *v;
  py -= cos(richtung) *v;
 } else {
  px=nx;
  py=ny;
 }
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

 richtung += drehung * (M_PI/180); // 5 Grad Schritte...
 nx=px+sin(richtung)*v;
 ny=py-cos(richtung)*v;

 if (nx<0) nx=0;
 if (nx>al.b * 640) nx=al.b*640;
 if (ny<0) ny=0;
 if (ny>al.h * 480) ny=al.h*480;
 } else {
  nx=px; ny=py;
 }
}

void spieler::setnpos()
{
 dyn.xp = (unsigned short int) px;
 dyn.yp = (unsigned short int) py;
 dyn.richtung = richtung * (128/M_PI);
}

void spieler::calccoll(const vector <game_objekt> objs)
{
	checkstoss();
	setnpos();
}

void spieler::getstatus(char *msg)
{
	sprintf (msg,"%s - Runde %d",cl->getname(),round);
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
 }
}

zuschauer::zuschauer(client *c)
 : game_objekt (c)
{
 fx=0;
 fy=0;
}

void zuschauer::calc()
{
 if (-(fx)<=dyn.xp && dyn.xp+fx< al.b*640)
  dyn.xp+=(int)fx;
 if (-(fy)<=dyn.yp && dyn.yp+fx< al.h*480)
  dyn.yp+=(int)fy;
}

#define Z_SPEED	6

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
 }
}
