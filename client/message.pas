{ 			- Unit um Messages zu versenden -

  Funktion	 : Kommunikationshilfe der einzelnen Module untereinander
  Team		 : Physik + Message
  Beschreibung	 : in messagesys.txt
  Änderungen	 :
  	25.5.2000: Änderungsliste eingefügt
        	   dummer Bug in den del-Funktionen entfernt
                   Debug Nummern wieder entfernt, da unn”tig}
unit message;

interface

{Message Gruppen}
const 	CONTROL=	1;
   	SETUP=		2;
   	SOUND=		4;
        GRAFIK=		8;
        PHYSIK=		16;
        NET=		32;
        DEBUG=		1024;
        GROUP_ALL=	CONTROL+ SETUP+ SOUND+ GRAFIK+ PHYSIK+ NET;
        
type
   void_ptr = ^void;

   rcvmsg_ptr = ^procedure (group, num, arg1, arg2 : integer);
   rcvmsgs_ptr = ^procedure (group, num, arg1 : integer; arg2 : CString);
   rcvmsgd_ptr = ^procedure (group, num : integer; arg1 : void_ptr; arg2 : integer);

var
   a : integer;

procedure sendmsg(group, num, arg1, arg2 : integer);
procedure sendmsgs(group, num, arg1: integer; arg2 : CString);
procedure sendmsgd(group, num : integer; arg1: void_ptr; arg2 : integer);

procedure addrcv(group : integer; ptr : rcvmsg_ptr);
procedure addrcvs(group : integer; ptr : rcvmsgs_ptr);
procedure addrcvd(group : integer; ptr : rcvmsgd_ptr);
procedure delrcv(ptr : rcvmsg_ptr);
procedure delrcvs(ptr : rcvmsgs_ptr);
procedure delrcvd(ptr : rcvmsgd_ptr);

{---------------------------------------------------------------------------}

implementation

type rlist_ptr = ^rlist;
     rlists_ptr = ^rlists;
     rlistd_ptr = ^rlistd;

     rlist = record
      group : integer;
      pt : rcvmsg_ptr;
      next : rlist_ptr;
      end;

     rlists = record
      group : integer;
      pt : rcvmsgs_ptr;
      next : rlists_ptr;
      end;

     rlistd = record
      group : integer;
      pt : rcvmsgd_ptr;
      next : rlistd_ptr;
      end;

var
 first : rlist_ptr;
 firsts : rlists_ptr;
 firstd : rlistd_ptr;

procedure sendmsg(group, num, arg1, arg2 : integer);
var
 p : rlist_ptr;
begin
 p:=first;

 while p<>nil do
  begin
   if (group and p^.group) <> 0 then
    p^.pt^ (group, num, arg1, arg2);
   p:=p^.next;
  end;
end;

procedure sendmsgs(group, num, arg1: integer; arg2 : CString);
var
 p : rlists_ptr;
begin
 p:=firsts;

 while p<>nil do
  begin
   if (group and p^.group) <> 0 then
    p^.pt^ (group, num, arg1, arg2);
   p:=p^.next;
  end;
end;

procedure sendmsgd(group, num:integer; arg1: void_ptr; arg2 : integer);
var
 p : rlistd_ptr;
begin
 p:=firstd;

 while p<>nil do
  begin
   if (group and p^.group) <> 0 then
    p^.pt^ (group, num, arg1, arg2);
   p:=p^.next;
 end;
end;

procedure addrcv(group : integer; ptr : rcvmsg_ptr);
var
 p : rlist_ptr;
begin
 { Liste erweitern }
 p:=first;
 new(first);
 first^.next:=p;

 first^.group := group;
 first^.pt := ptr;

 { Debug Message ab }
 sendmsgs (DEBUG, 0, 0, 'Added Receive Proc (Integer)!');
end;

procedure addrcvs(group : integer; ptr : rcvmsgs_ptr);
var
 p : rlists_ptr;
begin
 { Liste erweitern }
 p:=firsts;
 new(firsts);
 firsts^.next:=p;

 firsts^.group := group;
 firsts^.pt := ptr;

 { Debug Message ab }
 sendmsgs (DEBUG, 0, 0, 'Added Receive Proc (String)!');
end;

procedure addrcvd(group : integer; ptr : rcvmsgd_ptr);
var
 p : rlistd_ptr;
begin
 { Liste erweitern }
 p:=firstd;
 new(firstd);
 firstd^.next:=p;

 firstd^.group := group;
 firstd^.pt := ptr;

 { Debug Message ab }
 sendmsgs (DEBUG, 0, 0, 'Added Receive Proc (Data)!');
end;

procedure delrcv(ptr : rcvmsg_ptr);
var
 p,o : rlist_ptr;
begin
 p := first;

 if (p<>nil) and (p^.pt=ptr) then
  begin
   first:=p^.next;
   dispose (p);
   sendmsgs (DEBUG, 0, 0, 'Deleted Receive Proc (Integer)!');
  end
 else
  while p<>nil do
   begin
    if (p^.next<>NIL) and (p^.next^.pt=ptr) then
     begin
      o:=p^.next;
      p^.next:=p^.next^.next;
      dispose(o);
      sendmsgs (DEBUG, 0, 0, 'Deleted Receive Proc (Integer)!');
      break;
     end;
    p:=p^.next;
   end;
 if p=NIL then
  sendmsgs (DEBUG, 0, 0, 'Achtung falsche Pointeradresse bei delrcv!');
end;

procedure delrcvs(ptr : rcvmsgs_ptr);
var
 p,o : rlists_ptr;
begin
 p := firsts;

 if (p<>nil) and (p^.pt=ptr) then
  begin
   firsts:=p^.next;
   dispose (p);
   sendmsgs (DEBUG, 0, 0, 'Deleted Receive Proc (String)!');
  end
 else
  while p<>nil do
   begin
    if (p^.next<>NIL) and (p^.next^.pt=ptr) then
     begin
      o:=p^.next;
      p^.next:=p^.next^.next;
      dispose(o);
      sendmsgs (DEBUG, 0, 0, 'Deleted Receive Proc (String)!');
      break;
     end;
    p:=p^.next;
   end;
 if p=NIL then
  sendmsgs (DEBUG, 0, 0, 'Achtung falsche Pointeradresse bei delrcvs!');
end;

procedure delrcvd(ptr : rcvmsgd_ptr);
var
 p,o : rlistd_ptr;
begin
 p := firstd;

 if (p<>nil) and (p^.pt=ptr) then
  begin
   firstd:=p^.next;
   dispose (p);
   sendmsgs (DEBUG, 0, 0, 'Deleted Receive Proc (Data)!');
  end
 else
  while p<>nil do
   begin
    if (p^.next<>NIL) and (p^.next^.pt=ptr) then
     begin
      o:=p^.next;
      p^.next:=p^.next^.next;
      dispose(o);
      sendmsgs (DEBUG, 0, 0, 'Deleted Receive Proc (Data)!');
      break;
     end;
    p:=p^.next;
   end;
 if p=NIL then
  sendmsgs (DEBUG, 0, 0, 'Achtung falsche Pointeradresse bei delrcvd!');
end;

begin
 { Listen initialisieren }
 first:=nil;
 firsts:=nil;
 firstd:=nil;
end.
