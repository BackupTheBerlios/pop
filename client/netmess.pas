{ Pascal Code für messnet.c }

unit netmess;

interface

type
 serverchan_ptr = ^void;
 serverchan_ptr_ptr = ^serverchan_ptr;
 clientchan_ptr = ^void;
 clientchan_ptr_ptr = ^clientchan_ptr;
 s_client_ptr	= ^void;
 void_ptr	= ^void;

 s_rcvmsg_ptr	= ^procedure (c : s_client_ptr; group,num,arg1,arg2 : integer);
 s_rcvmsgs_ptr	= ^procedure (c : s_client_ptr; group,num,arg1 : integer; arg2 : CString);
 s_rcvmsgd_ptr	= ^procedure (c : s_client_ptr; group,num : integer;arg1 : void_ptr;arg2 : integer);

 c_rcvmsg_ptr	= ^procedure (group,num,arg1,arg2 : integer);
 c_rcvmsgs_ptr	= ^procedure (group,num,arg1 : integer; arg2 : CString);
 c_rcvmsgd_ptr	= ^procedure (group,num : integer;arg1 : void_ptr;arg2 : integer);

{ Server Teil }
function start_server (sc : serverchan_ptr_ptr; port : integer) : integer; C;
procedure stop_server (sc : serverchan_ptr); C;

procedure serv_setacpt(sc : serverchan_ptr;a : integer); C;
procedure serv_kill_client (sc : serverchan_ptr; a : s_client_ptr); C;

procedure calcserv(sc : serverchan_ptr); C;

procedure serv_sendmsg (sc : serverchan_ptr;c:s_client_ptr;group,num,arg1,arg2 : integer); C;
procedure serv_sendmsgs (sc : serverchan_ptr;c:s_client_ptr;group,num,arg1 : integer;arg2 : CString); C;
procedure serv_sendmsgd (sc : serverchan_ptr;c:s_client_ptr;group,num : integer; arg1 : void_ptr;arg2 : integer); C;

procedure serv_sendmsg_all (sc : serverchan_ptr;group,num,arg1,arg2 : integer); C;
procedure serv_sendmsgs_all (sc : serverchan_ptr;group,num,arg1 : integer;arg2 : CString); C;
procedure serv_sendmsgd_all (sc : serverchan_ptr;group,num : integer; arg1 : void_ptr;arg2 : integer); C;

procedure serv_set_rcvmsg (sc : serverchan_ptr; f : s_rcvmsg_ptr); C;
procedure serv_set_rcvmsgs (sc : serverchan_ptr; f : s_rcvmsgs_ptr); C;
procedure serv_set_rcvmsgd (sc : serverchan_ptr; f : s_rcvmsgd_ptr); C;

{ Client Teil }
function connect_server (cc : clientchan_ptr_ptr; addr : CString; port : integer) : integer; C;
procedure disconnect_server (cc : clientchan_ptr); C;

procedure calcclient (cc : clientchan_ptr); C;

procedure cli_sendmsg (cc : clientchan_ptr;group,num,arg1,arg2 : integer); C;
procedure cli_sendmsgs (cc : clientchan_ptr;group,num,arg1 : integer;arg2 : CString); C;
procedure cli_sendmsgd (cc : clientchan_ptr;group,num : integer; arg1 : void_ptr;arg2 : integer); C;

procedure cli_set_rcvmsg (cc : clientchan_ptr; f : c_rcvmsg_ptr); C;
procedure cli_set_rcvmsgs (cc : clientchan_ptr; f : c_rcvmsgs_ptr); C;
procedure cli_set_rcvmsgd (cc : clientchan_ptr; f : c_rcvmsgd_ptr); C;

implementation

begin
end.
