// gameobj.h

#ifndef __GAMEOBJ_H__
#define __GAMEOBJ_H__

#include "net.h"
#include "control.h"
#include "auto.h"

// This makes game Objekts independent from Server and Network stuff
// at least, most...
class client;

class game_objekt
{
 public:
  objdyn dyn;
  client *cl;

      game_objekt(client *c);
  int xp();
  int yp();
  virtual void calcpos();
  virtual void calccoll(const vector<game_objekt> objs);
  virtual void contr(int num,int a);
  virtual void setpos(unsigned short x,unsigned short y);
  virtual void setrot(fix r);
  void sendmsg(int num,const char *msg);
  // FixME: Buffer overflows
  virtual void getstatus(char *msg);
};

class zuschauer : public game_objekt
{
 public:
  float fx,fy;

       zuschauer(client *c);
  void calc();
  void contr(int num,int a);
};

class spieler : public game_objekt
{
public:
  autodaten *car;
  float f,v;
  float px,py,richtung;
  float nx,ny;
  float drehung;
  int acp;
  int round;
  int fertig;

       spieler(client *c);
  void setcar(int num);

  void calcpos();
  void calccoll(const vector<game_objekt> objs);
  void setnpos();
  void contr(int num,int a);
  virtual void setpos(unsigned short x,unsigned short y);
  virtual void setrot(fix r);
  void getstatus(char *msg);
  void inline checkstoss();
};

#endif
