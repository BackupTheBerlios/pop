{ -- Globale Variablen -- }
unit gvars;

interface

uses leveldat,auto;

const
 STATUS_UNBEKANNT=	101;
 STATUS_NO_CONNECT=	100;
 STATUS_WAIT=		0;
 STATUS_LOAD=		1;
 STATUS_RUN=		2;
 STATUS_PAUSE=		3;
 STATUS_ERGEBNIS=	4;
 STATUS_ENDE=		1234;

var
 status : integer;
 alevel : level;
 alevelnum : integer;
 anz_level : integer;
 typen : autos;
 zuschauer : autodaten;
 lstatus : integer;
 sound_enabled : boolean;
 nopageflip : boolean;

procedure gvars_init;
procedure gvars_exit;

procedure draw_load;
procedure load_message(s : String);
procedure load_messagea(s : String);
function  load_autos : integer;

implementation

uses message,allunit,config;

var
 textposx : integer;
 textposy : integer;

procedure change_status(st : integer);
begin
 if (status<>st) then begin
  lstatus:=status;
  status:=st;
 end;
end;

procedure gvars_rcvmsg(group,num,arg1,arg2 : integer);
begin
 if num=1 then
  change_status(arg1);
end;

procedure gvars_init;
begin
 addrcv (SETUP,@gvars_rcvmsg);
end;

procedure gvars_exit;
begin
 delrcv (@gvars_rcvmsg);
end;

procedure draw_load;
begin
 clear (screen);
 textposy:=-8;
 textposx:=0;
end;

procedure load_message(s : String);
begin
 textposy:=textposy+text_height(font)+1;
 textposx:=0;
 textout (screen,font,s,textposx,textposy,makecol(255,0,0));
 textposx:=textposx+text_length(font,s);
end;

procedure load_messagea(s : String);
begin
 textout (screen,font,s,textposx,textposy,makecol(255,0,0));
 textposx:=textposx+text_length(font,s);
end;

function load_autos : integer;
var
 i,erg : integer;
begin
 erg:=autos_load (@typen,PFAD_AUTOGFX);
 if erg<0 then begin
  load_autos:=erg;
 end else begin
  zuschauer:=typen.d[0];
  for i:=0 to typen.anz-1 do begin
   if (typen.d[i].flags and 1)=1 then
    zuschauer:=typen.d[i];
  end;
  load_autos:=0;
 end;
end;

begin
 status := STATUS_UNBEKANNT;
 lstatus := STATUS_UNBEKANNT;
end.
