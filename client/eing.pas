Unit eing;

interface

 procedure init_larray;
 procedure exit_larray;
 procedure eing(var s : string; xpos, ypos : integer);
 procedure ausg(s : string; xpos, ypos : integer);

implementation

uses fehler,config,allunit;

 var letter : array['a' .. 'z'] of BITMAP_ptr;
     bletter : array['A' .. 'Z'] of BITMAP_ptr;
     punkt : BITMAP_ptr;
     decimals : array['0' .. '9'] of BITMAP_ptr;
     textback : BITMAP_ptr;

 procedure init_larray;
 var c : char;
 begin
  for c:='a' to 'z' do begin
     letter[c] := load_bitmap(PFAD_MENU+'font'+c+'.pcx',nil);
     if letter[c]=nil then
        fehler ('Fehler beim Laden einer Buchstabendatei:'+c);
  end;
  for c:='A' to 'Z' do begin
	bletter[c] := load_bitmap(PFAD_MENU+'fontB'+c+'.pcx',nil);
	if bletter[c]=nil then
		fehler ('Fehler beim Laden einer Buchstabendatei:'+c);
  end;
  for c:='0' to '9' do begin
	decimals[c]:=load_bitmap(PFAD_MENU+'font'+c+'.pcx',nil);
	if decimals[c]=nil then
		fehler ('Fehler beim laden einer Buchstabendatei:'+c);
  end;
  punkt:=load_bitmap(PFAD_MENU+'fontpunkt.pcx',nil);
  if punkt = nil then
	fehler ('Fehler beim Laden der Buchstabendatei: .');
  textback:=load_bitmap(PFAD_MENU+'textfeld.pcx',nil);
 end;

 procedure exit_larray;
 var c : char;
 begin
  if punkt <> nil then
   destroy_bitmap(punkt);

  for c:='a' to 'z' do
     if letter[c]<>nil then
	     destroy_bitmap(letter[c]);
  {FixME den REst noch freigeben}
 end; 

function getgfx(a : char) : BITMAP_ptr;
begin
	if (a>='a') and (a<='z') then
		getgfx:=letter[a]
	else if (a>='A') and (a<='Z') then
		getgfx:=bletter[a]
	else if (a>='0') and (a<='9') then
		getgfx:=decimals[a]
	else if a='.' then
		getgfx:=punkt
	else
		getgfx:=nil;
end;

 procedure eing(var s : string; xpos, ypos : integer);
 var i, x, pos : integer;
     a         : char;
     sc	       : word;
     tmp : string[128];
     alet : BITMAP_ptr;
 begin
  clear_keybuf;
  x := 0;
  pos := xpos;
  tmp[1]:='$';
  repeat
   inc(x);
   sc := readkey;
   a := char(sc and $ff);
   sc := sc shr 8;
   if (sc=KEY_BACKSPACE) and (pos > xpos) then
    begin
     alet:=getgfx(tmp[x-1]);
     if alet<>nil then begin
	     pos := pos-alet^.w;
	     {mit Hintergrund übermalen}
	     blit (textback,screen,pos-xpos,0,pos,ypos,alet^.w,alet^.h);
	     {Text ändern}
	end;
     tmp[x-1] := '$';
     x := x-2;
    end
   else
     begin
	alet:=getgfx(a);
	if alet<>nil then begin
            draw_sprite(screen, alet, pos, ypos);
            pos := pos+alet^.w;
            tmp[x] := a;
            tmp[x+1] := '$';
      end;
     end;
  until (sc=KEY_ENTER) or (x=15);

  i:=1;
  s:='';
  while tmp[i]<>'$' do
    begin
     s:=s+tmp[i];
     inc(i);
    end;
 end;

 procedure ausg(s : string; xpos, ypos : integer);
 var 
   i, pos : integer;
 begin
   pos := xpos;
   i := 1;
   while s[i] <> '0' do
   begin
     draw_sprite(screen, letter[s[i]], pos, ypos);
     pos := pos+letter['a']^.w;
     inc(i);
   end;
 end;


Begin
End.
