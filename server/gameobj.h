// gameobj.h

#ifndef __GAMEOBJ_H__
#define __GAMEOBJ_H__

#include "net.h"
#include "control.h"
#include "auto.h"

#define TYPE_NONE		-1
#define TYPE_ZUSCHAUER	0
#define TYPE_SPIELER	1

// This makes game Objekts independent from Server and Network stuff
// at least, most...
class client;

class game_objekt
{
 public:
  objdyn dyn;

      game_objekt();
  void setclient(client *c);
  int xp();
  int yp();
  virtual void calcpos();
  virtual void calccoll(vector<game_objekt *> *objs);
  virtual void contr(int num,int a);
  virtual void setpos(unsigned short x,unsigned short y);
  virtual void setrot(fix r);
  virtual int gettyp();
  void sendmsg(int num,const char *msg);
  // FixME: Buffer overflows
  virtual void getstatus(char *msg);
 protected:
  client *cl;
};

class zuschauer : public game_objekt
{
 public:
  float fx,fy;

       zuschauer();
  void calcpos();
  void contr(int num,int a);
  int gettyp();
};

class spieler : public game_objekt
{
public:
  autodaten *car;
  float f,v;
  float px,py,richtung;
  float nx,ny,nrichtung;
  float drehung;
  int acp;
  int round;
  int fertig;

       spieler();
  void setcar(int num);

  void calcpos();
  void calccoll(vector<game_objekt *> *objs);
  void contr(int num,int a);
  virtual void setpos(unsigned short x,unsigned short y);
  virtual void setrot(fix r);
  void getstatus(char *msg);
  int gettyp();
protected:
  void checkcp();
  void setnpos();
  int inline checkstoss(float,float,float,float);
  void getcorners(float,float,float,float[4],float[4]);
};

#endif
