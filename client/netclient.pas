unit netclient;

interface

uses auto,config,leveldat,allunit,gvars;

type
 {dynamische (sich verändernde) Daten eines Spielobjekts}
 objdynamic = record
 	xp, yp : ShortWord;   {aktuelle Position als x- und y-Koordinate}
        richtung : fixed;   {Die Richtung in die sich das Auto gedreht hat
        			- als Allegro Fixkomma Zahl}
        end;

 objdynamic_ptr = ^objdynamic;

 playerinfo = record
	name : String(255);
	typ : integer;
	gfx : BITMAP_ptr;
	end;	

var
 netclient_fehler : String(255);
 players : array [0..MAXPLAYERS-1] of playerinfo;
 dyndata : array [0..MAXPLAYERS-1] of objdynamic;
 myself : ^objdynamic;
 myselfpl : ^playerinfo;
 mynumber : integer;
 mytyp : integer;
 master : integer;
 alevelname : String(255);
 levelok : boolean;
 netchanged : boolean;

procedure netclient_run;
procedure netclient_exit;

function netclient_connect (s: String) : integer;
procedure netclient_disconnect;

procedure netclient_check;

implementation

uses gpc,message,netmess,fehler,grafik;

var
 cc : clientchan_ptr;
 lanz : integer;

procedure setnull;
var
 i : integer;
begin
 cc := nil;
 netclient_fehler:='';
 mynumber:=-1;
 myself:=nil;
 myselfpl:=nil;
 mytyp:=1; {Typ Auto}
 for i:=0 to MAXPLAYERS-1 do begin
  players[i].typ:=$ffff;
  players[i].gfx:=nil;
 end;
end;

procedure netclient_run;
begin
 if cc<>nil then begin
  netchanged:=false;
  calcclient (cc);
 end;
end;

procedure netclient_msgrcv(g,n,a1,a2 : integer);
begin
 netchanged:=true;
 if (g and NET)<>0 then
  cli_sendmsg (cc,g and (not NET),n,a1,a2)
 else if (g and SETUP)<>0 then begin
  case n of
   6: begin		{ANzahl der verfügbaren Level}
    anz_level := a1;
    alevelnum := a2;
   end;
   8: begin 	{Übermittlung des Objekttyps eines neuen Speilers}
    if a1<MAXPLAYERS then begin
     players[a1].typ:=a2;
    end else
     sendmsgs (DEBUG,0,0,'Falsches Argument bei Spielertyp! (Setup,6)');
   end;
   9: begin		{neuer Meister}
    master:=a1;
   end;
   10: begin
    master:=a1;
    mynumber:=a2;
    if mynumber>=MAXPLAYERS then begin
     sendmsg (SETUP,1,STATUS_NO_CONNECT,0);
     netclient_fehler:='Server lieferte eine zu hohe Nummer zurück!';
    end else begin
     players[mynumber].typ:=mytyp;
     myself := @dyndata[mynumber];
     myselfpl := @players[mynumber];
    end;
   end;
   12: begin {Autowechsel}
    if (a1>=MAXPLAYERS) or (a2>=typen.anz) then
     sendmsgs (DEBUG,0,0,'Protokollfehler bei Autowahl');

    players[a1].gfx:=typen.d[a2].gfx;
    end;
   21: begin
    alevel.b := a1;
    alevel.h := a2;
    lanz:=0;
   end;
  end; {case}
 end;
end;

procedure netclient_msgrcvs(g,n,a1 : integer;a2 : CString);
var
 temps : String(255);
begin
 netchanged:=true;
 if (g and NET)<>0 then
  cli_sendmsgs (cc,g and (not NET),n,a1,a2)
 else if (g and SETUP)<>0 then begin
  case n of
   7: begin       	{Übermittlung eines neuen Spielers}
    if a1<MAXPLAYERS then begin
     CopyCString (a2,players[a1].name);
    end else
     sendmsgs (DEBUG,0,0,'Falsches Argument bei Spielernamen (Setup,7)');
   end;
   13: begin {Neuer Level}
    CopyCString (a2,alevelname);
   end;
   20: begin {Status Message während des Spiels}
    CopyCString (a2,temps);
    grafik_setmsg(a1,temps);
   end;
  end;
 end;
end;

type
 dyns = array [0..MAXPLAYERS-1] of objdynamic;

procedure netclient_msgrcvd(g,n : integer; a1 : void_ptr; a2:integer);
var
 dynptr : ^dyns;
begin
 netchanged:=true;
 if (g and NET)<>0 then
  cli_sendmsgd (cc,g and (not NET),n,a1,a2)
 else begin
  if (g and PHYSIK)<>0 then begin
   dynptr:=a1;
   dyndata := dynptr^;
  end;

  if (g and SETUP)<>0 then begin
   if n=22 then begin
    if lanz<level_anzdata(alevel) then begin
     level_trans (alevel,lanz,a1);
     lanz:=lanz+1;
     if (lanz=level_anzdata(alevel)) then begin
      levelok:=true;
     end;
    end;
   end;
  end;
 end;
end;

procedure netclient_netrcv(g,n,a1,a2 : integer);
begin
 if (g and NET)<>0 then begin
  if (n=2) then begin {Server hat uns rausgeworfen :(}
   sendmsg (SETUP,1,STATUS_NO_CONNECT,0);
  end;
 end else begin
  sendmsg (g,n,a1,a2);
 end;
end;

procedure netclient_netrcvs(g,n,a1 : integer;a2 : CString);
begin
 if (g and NET)=0 then
  sendmsgs (g,n,a1,a2);
end;

procedure netclient_netrcvd(g,n : integer; a1 : void_ptr; a2:integer);
begin
 if (g and NET)=0 then
  sendmsgd (g,n,a1,a2);
end;

{Checkt ob alle Daten richtig gesetzt sind}
procedure netclient_check;
var
 i : integer;
begin
 for i:=0 to MAXPLAYERS-1 do begin
  if players[i].typ=1 then begin        {Spieler}
   if players[i].name='' then players[i].name:='NoName';
   if players[i].gfx=nil then players[i].gfx:=typen.d[0].gfx;
  end else if players[i].typ=0 then begin    {Zuschauer}

  end;
 end;
end;


function netclient_connect (s: String) : integer;
var
 erg : integer;
begin
 cc := nil;
 netclient_fehler:='';
 mynumber:=-1;
 myself:=nil;
 levelok:=false;

 {Schritt1: Netzwerk Connection aufbauen!}
 erg:= connect_server (@cc,s,STDPORT);
 if (erg < 0) then begin
  case erg of
   -1: netclient_fehler:='Kein Speicher mehr!';
   -2: netclient_fehler:='Socket Error! (keine Dostreiber installiert?)';
   -3: netclient_fehler:='IOCtl Error!';
   -4: netclient_fehler:='Name resolve error!';
   -5: netclient_fehler:='Bind Error!';
   -6: netclient_fehler:='Send Error!';
   -7: netclient_fehler:='Spiel auf diesem Server laeuft bereits!';
   -8: netclient_fehler:='Timeout! (kein Server gestartet?)';
  end;
 end else begin
  {Schritt 2: Receiveprozeduren anmelden}
  addrcv (GROUP_ALL,@netclient_msgrcv);
  addrcvs (GROUP_ALL,@netclient_msgrcvs);
  addrcvd (GROUP_ALL,@netclient_msgrcvd);
  cli_set_rcvmsg (cc,@netclient_netrcv);
  cli_set_rcvmsgs (cc,@netclient_netrcvs);
  cli_set_rcvmsgd (cc,@netclient_netrcvd);

  {Schritt 3: Allgemeine Daten senden...}
  sendmsg (NET+SETUP,52,mytyp,0); {Typ - Mitspieler}
  sendmsg (NET+SETUP,53,0,0); {Alles gesendet}
  for erg:=1 to 10000 do begin
   calcclient (cc);
   if myself<>nil then break;
   rest (1000);
  end;
  if erg<10000 then begin
   erg:=0;
   players[mynumber].typ:=mytyp;
  end else begin
   netclient_disconnect;
   netclient_fehler:='Server Timeout';
   erg:=-9;
  end;
 end;
 netclient_connect:=erg;
end;

procedure netclient_disconnect;
begin
 if cc<>nil then begin
  {Receiveprozeduren abmelden}
  delrcv (@netclient_msgrcv);
  delrcvs (@netclient_msgrcvs);
  delrcvd (@netclient_msgrcvd);

  disconnect_server (cc);
  cc:=nil;
 end;
end;

procedure netclient_exit;
begin
 if cc<>NIL then
  netclient_disconnect;
 setnull;
end;

begin
 setnull;
end.
