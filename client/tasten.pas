unit tasten;

Interface

Const gas    =  1;
      gasweg =  2;
      brems  =  3;
      brunter=  4;
      links  =  5;
      liende =  6;
      rechts =  7;
      reende =  8;
      pause  =  9;
      pende  = 10;
      quit   = 11;
      tut    = 12;
      tutende= 13;

procedure tasten_init;
procedure tasten_run;
procedure tasten_exit;

Implementation

uses message, allunit,gvars;

Type memory = Array[1..12] of byte;

Var mem : memory;
    y : byte;

  Function proof(y : byte) : boolean;
  Begin
   If mem[y]=0 Then proof:=True
      	       Else proof:=False;
  End;

  Procedure tasten_init;
  Begin
   {Keyboard initialisieren...}
   For y:=1 To 12 Do mem[y]:=1;
  End;

  procedure tasten_exit;
  begin
  end;

  Procedure tasten_run;
  Begin
   If key^[key_up]    <> 0 Then If Not proof(1)  Then Begin Sendmsg(Control+NET,1,0,gas)    ; mem[1]:=0; mem[2]:=1; End;
   If key^[key_up]    =  0 Then If Not proof(2)  Then Begin Sendmsg(Control+NET,1,0,gasweg) ; mem[2]:=0; mem[1]:=1; End;
   If key^[key_down]  <> 0 Then If Not proof(3)  Then Begin Sendmsg(Control+NET,1,0,brems)  ; mem[3]:=0; mem[4]:=1; End;
   If key^[key_down]  =  0 Then If Not proof(4)  Then Begin Sendmsg(Control+NET,1,0,brunter); mem[4]:=0; mem[3]:=1; End;
   If key^[key_left]  <> 0 Then If Not proof(5)  Then Begin Sendmsg(Control+NET,1,0,links)  ; mem[5]:=0; mem[6]:=1; End;
   If key^[key_left]  =  0 Then If Not proof(6)  Then Begin Sendmsg(Control+NET,1,0,liende) ; mem[6]:=0; mem[5]:=1; End;
   If key^[key_right] <> 0 Then If Not proof(7)  Then Begin Sendmsg(Control+NET,1,0,rechts) ; mem[7]:=0; mem[8]:=1; End;
   If key^[key_right] =  0 Then If Not proof(8)  Then Begin Sendmsg(Control+NET,1,0,reende) ; mem[8]:=0; mem[7]:=1; End;
   If key^[key_t]     <> 0 Then If Not proof(11) Then Begin Sendmsg(Control+NET,1,0,tut)    ; mem[11]:=0; mem[12]:=1; End;
   If key^[key_t]     =  0 Then If Not proof(12) Then Begin Sendmsg(Control+NET,1,0,tutende); mem[12]:=0; mem[11]:=1; End;
   If key^[key_p]     <> 0 Then If Not proof(9)  Then Begin 			       	mem[9]:=0 ; mem[10]:=1; End;
   If key^[key_p]     =  0 Then If Not proof(10) Then Begin mem[10]:=0; mem[9]:= 1; If y=10 Then Begin Sendmsg(Control+NET,1,0,pende); y:=1; End Else Begin Sendmsg(Control+NET,1,0,pause); y:=10; End; End;
   If key^[key_Esc]   <> 0 Then Sendmsg(Control+NET,1,0,quit);
  End;

Begin
End.
