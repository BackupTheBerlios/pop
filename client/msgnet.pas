unit msgnet;

interface

procedure msgnet_init;
procedure msgnet_exit;
procedure msgnet_run;

implementation

uses message, gpc, gpcutil, config;

function init_netdebug : integer; C;
procedure exit_netdebug; C;
procedure run_netdebug; C;
procedure sstring (s: CString); C;

procedure msgnet_rcv(group,num,arg1,arg2 : integer);
begin
 if (group and DEBUG) <> 0 then
  sstring ('DN: Debug: '+int2str(arg2))
 else
  sstring ('DN: Message - G:'+int2str(group)+' N:'+int2str(num)+' A1:'+int2str(arg1)+' A2:'+int2str(arg2));
end; 

procedure msgnet_rcvs(group,num,arg1 : integer; arg2 : CString);
var
 ts : String(255);
begin
 if (group and DEBUG) <> 0 then
  sstring ( arg2 )
 else begin
  CopyCString (arg2,ts);
  ts :='MessageS - G:'+int2str(group)+' N:'+int2str(num)+' A1:'+int2str(arg1)+' A2:'+ts;
  sstring (ts);
 end;
end;

procedure msgnet_rcvd (group,num : integer; p : void_ptr; l : integer);
begin
 sstring ('DN: MessageD - G:'+int2str(group)+' N:'+int2str(num)+' Datelen:'+int2str(l));
end;

procedure msgnet_init;
begin
 if MSGNET_ACTIVATED then begin
  if init_netdebug >= 0 then begin
   addrcv (GROUP_ALL+DEBUG,@msgnet_rcv);
   addrcvs (GROUP_ALL+DEBUG,@msgnet_rcvs);
   addrcvd (GROUP_ALL+DEBUG,@msgnet_rcvd);
  end else begin
   sendmsgs (DEBUG,0,0,'Fehler beim aktivieren von MessageNetDebug!');
   MSGNET_ACTIVATED:=false;
  end;
 end; 
end;

procedure msgnet_run;
begin
 if MSGNET_ACTIVATED then
  run_netdebug;
end;

procedure msgnet_exit;
begin
 if MSGNET_ACTIVATED then begin
  exit_netdebug;
  delrcv (@msgnet_rcv);
  delrcvs (@msgnet_rcvs);
  delrcvd (@msgnet_rcvd);
 end;
end;

begin
end.
