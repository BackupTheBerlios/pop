unit ende;

interface

procedure ende_run;

implementation

uses allunit,fehler,gvars,config,music;

const anzlines = 19;
      anzpictures = 6;

      hold = 0;
      rotatehin = 1;
      rotatehout = 2;
      rotatevin = 3;
      rotatevout = 4;
      movehin = 5;
      movehout = 6;
      movevin = 7;
      movevout = 8;
      explodein = 9;
      explodeout = 10;

      timelen = 250;

      { Hier könnte man nach Prozessor power testen....
 		160,50 sieht COOL aus! }
	explodetx = 40;
      explodety = 25;
	PICB = 300;
	PICH = 225;

type
   explodetile = record
   	tgfx : BITMAP_ptr;
        fx,fy : integer;
        tx,ty : integer;
        xmul,ymul : real;
        end;

   explodetile_ptr = ^explodetile;

   effekt = record
   	gfx : BITMAP_ptr;
        px, py : integer;
        b,h : integer;
        timepos : integer;
        efftyp : integer;
        fxdata : ^void;
        end;

var
   text : array [0..anzlines-1] of string(255);
   aktp : BITMAP_ptr;
   hinterg : BITMAP_ptr;
   pos : integer;
   zeile : integer;
   xges,yges : integer;
   aktpic : integer;
   picts : array [0..anzpictures-1] of BITMAP_ptr;
   gese : effekt;
   sint : array[0..timelen] of integer;

procedure settext;
begin
 text[0]:='Das war POP';
 text[1]:='-------------';
 text[2]:='Ein Spiel vom Grundkurs Info 2000';
 text[3]:='';
 text[4]:='Geschaffen von:';
 text[5]:='';
 text[6]:='Bernd Andres';
 text[7]:='Peter Becker';
 text[8]:='Roland Bischof';
 text[9]:='Matthias Braun';
 text[10]:='Michael Denne';
 text[11]:='Martin Fuchs';
 text[12]:='Jan Glueck';
 text[13]:='Thorsten Gross';
 text[14]:='Steffen Metzger';
 text[15]:='Christian Mueller';
 text[16]:='David Rings';
 text[17]:='Thomas Schwander';
 text[18]:='Eric Trumm';
end;

procedure errorende;
begin
 fehler ('Fehler beim laden der Bilder für Abschlusseffekt');
end;

procedure setpics;
var
 i : integer;
begin
 picts[0]:=loadbild(PFAD_ENDE+'bild1.bmp',PICB,PICH);
 picts[1]:=loadbild(PFAD_ENDE+'bild2.bmp',PICB,PICH);
 picts[2]:=loadbild(PFAD_ENDE+'bild3.bmp',PICB,PICH);
 picts[3]:=loadbild(PFAD_ENDE+'bild4.bmp',PICB,PICH);
 picts[4]:=loadbild(PFAD_ENDE+'bild5.bmp',PICB,PICH);
 picts[5]:=loadbild(PFAD_ENDE+'bild6.bmp',PICB,PICH);

 for i:=0 to anzpictures-1 do begin
  if picts[i]=nil then errorende;
 end;
end;

procedure delpics;
var
 i : integer;
begin
 for i:=0 to anzpictures-1 do begin
  destroy_bitmap (picts[i]);
 end;
end;

procedure initeffekt(var e : effekt);
var
 i,x,y : integer;
begin
 e.timepos:=timelen;
 case e.efftyp of
  hold:
   begin
   end;
  rotatehin:
   begin
    {Sinus Tabelle füllen...}
    for i:=0 to timelen do
    begin
     sint[timelen-i] := fixtoi ( fsin(ftofix (64/timelen * i)) * e.b);
    end;
   end;
  rotatehout:
   begin
    {Sinus Tabelle füllen...}
    for i:=0 to timelen do
    begin
     sint[i] := fixtoi ( fsin(ftofix (64/timelen * i)) * e.b);
    end;
   end;
  rotatevin:
   begin
    {Sinus Tabelle füllen...}
    for i:=0 to timelen do
    begin
     sint[timelen-i] := fixtoi ( fsin(ftofix (64/timelen * i)) * e.h);
    end;
   end;
  rotatevout:
   begin
    {Sinus Tabelle füllen...}
    for i:=0 to timelen do
    begin
     sint[i] := fixtoi ( fsin(ftofix (64/timelen * i)) * e.h);
    end;
   end;
  explodein:
   begin
    with (e) do begin
    fxdata := getmem (sizeof(explodetile)*explodetx*explodety);

    for x:=0 to explodetx-1 do
     for y:=0 to explodety-1 do begin
      {$x+}
      with((explodetile_ptr(fxdata)+(x*(explodety-1)+y))^) do begin
       tgfx:=create_bitmap (b div explodetx,h div explodety);
       blit (gfx,tgfx,x*(b div explodetx),y*(h div explodety),0,0,b div explodetx,h div explodety);
       tx:=px+x*(b div explodetx)-(b div 2);
       ty:=py+y*(h div explodety);
       fx:=rand mod xges*4 - xges*2 + (xges div 2);
       fy:=rand mod yges*4 - yges*2 + (yges div 2);
       while (fx>=0) and (fx<xges) and (fy>=0) and (fy<yges) do begin
        fx:=rand mod xges*4 - xges*2 + (xges div 2);
        fy:=rand mod yges*4 - yges*2 + (yges div 2);
       end;
       xmul:= (fx-tx) / timelen;
       ymul:= (fy-ty) / timelen;
      end;
      {$x-}
     end;
    end;
   end;

  explodeout:
   begin
    with (e) do begin
    fxdata := getmem (sizeof(explodetile)*explodetx*explodety);

    for x:=0 to explodetx-1 do
     for y:=0 to explodety-1 do begin
      {$x+}
      with((explodetile_ptr(fxdata)+(x*(explodety-1)+y))^) do begin
       tgfx:=create_bitmap (b div explodetx,h div explodety);
       blit (gfx,tgfx,x*(b div explodetx),y*(h div explodety),0,0,b div explodetx,h div explodety);
       tx:=px+x*(b div explodetx)-(b div 2);
       ty:=py+y*(h div explodety);
       fx:=rand mod xges*4 - xges*2 +(xges div 2);
       fy:=rand mod yges*4 - yges*2 +(yges div 2);
       while (fx>=0) and (fx<xges) and (fy>0) and (fy<yges) do begin
        fx:=rand mod xges*4 - xges*2+(xges div 2);
        fy:=rand mod yges*4 - yges*2+(yges div 2);
       end;
       xmul:= (fx-tx) / timelen;
       ymul:= (fy-ty) / timelen;
      end;
      {$x-}
     end;
    end;
   end;

  end;
end;

procedure exiteffekt(var e : effekt);
var
 x,y : integer;
begin
 case e.efftyp of
  explodein:
   begin
    with (e) do begin
    for x:=0 to explodetx-2 do
     for y:=0 to explodety-2 do begin
      {$x+}
      with((explodetile_ptr(fxdata)+(x*(explodety-1)+y))^) do begin
      {$x-}
       destroy_bitmap (tgfx);
      end;
     end;
    freemem (fxdata);
    end;
   end;
  explodeout:
   begin
    with (e) do begin
    for x:=0 to explodetx-2 do
     for y:=0 to explodety-2 do begin
      {$x+}
      with((explodetile_ptr(fxdata)+(x*(explodety-1)+y))^) do begin
      {$x-}
       destroy_bitmap (tgfx);
      end;
     end;
    freemem (fxdata);
    end;
   end;
 end;
end;

procedure effektint;
begin
 dec(gese.timepos);
 dec(pos);
 if pos=0 then begin
	pos:=20;
	zeile:=zeile+1;
	if zeile=anzlines+20 then
		zeile:=(anzlines-1)-(xges DIV 16)-1;
 end; 
end;

procedure doeffekt(var e : effekt);
var
 x,y : integer;
 apx,apy : integer;
begin
 if e.timepos<=0 then begin
  {...}
  exiteffekt (e);
  if (e.efftyp mod 2)=0 then begin
   e.efftyp:=((rand mod 5)+1)*2-1;
   inc(aktpic);
   if aktpic>=anzpictures then
    aktpic:=0;
   e.gfx:=picts[aktpic];
   {Neues Bild laden...}
   end
  else
   e.efftyp:=((rand mod 5)+1)*2;
  initeffekt(e);
  e.timepos:=timelen;
  end;

 with (e) do begin
 case efftyp of
  hold:
   begin
    draw_sprite (aktp,gfx,px-(b DIV 2),py);
   end;
  rotatehin:
   begin
    stretch_sprite (aktp,gfx,px- (sint[timepos] DIV 2),py,sint[timepos],h);
   end;
  rotatehout:
   begin
    stretch_sprite (aktp,gfx,px- (sint[timepos] DIV 2),py,sint[timepos],h);
   end;
  rotatevin:
   begin
    stretch_sprite (aktp,gfx,px- (b DIV 2),py+(h DIV 2)-(sint[timepos] DIV 2),b,sint[timepos]);
   end;
  rotatevout:
   begin
    stretch_sprite (aktp,gfx,px- (b DIV 2),py+(h DIV 2)-(sint[timepos] DIV 2),b,sint[timepos]);
   end;
  movehin:
   begin
    x:=-b*2;
    y:=px-(b div 2);
    draw_sprite (aktp,gfx, (((x-y) div (timelen-1)) * timepos) + y, py)
   end;
  movehout:
   begin
    x:=-b*2;
    y:=px-(b div 2);
    draw_sprite (aktp,gfx, (((x-y) div (timelen-1)) * (timelen-timepos-1)) + y, py)
   end;
  movevin:
   begin
    x:=-2*h;
    y:=py;
    draw_sprite (aktp,gfx, px-(b div 2),(((x-y) div timelen) * timepos) + y)
   end;
  movevout:
   begin
    x:=yges+h;
    y:=py;
    draw_sprite (aktp,gfx, px-(b div 2),(((x-y) div timelen) * (timelen-timepos)) + y)
   end;
  explodein:
   begin
    for x:=0 to explodetx-1 do
     for y:=0 to explodety-1 do begin
{$x+}
      with((explodetile_ptr(fxdata)+(x*(explodety-1)+y))^) do begin
       apx:=round((xmul * (timepos)) + tx);
       apy:=round((ymul * (timepos)) + ty);
       blit (tgfx,aktp,0,0,apx, apy,tgfx^.w,tgfx^.h);
       end;
{$x-}
     end;
   end;
  explodeout:
   begin
    for x:=0 to explodetx-1 do
     for y:=0 to explodety-1 do begin
      {$x+}
      with((explodetile_ptr(fxdata)+(x*(explodety-1)+y))^) do begin
       apx:=round((xmul * (timelen-timepos)) + tx);
       apy:=round((ymul * (timelen-timepos)) + ty);
       blit (tgfx,aktp,0,0,apx, apy,tgfx^.w,tgfx^.h);
       end;
      {$x-}
     end;
   end;
  else
   draw_sprite (aktp,gfx,px-(b DIV 2),py);


 end; {case}
 end; {with}

end;

procedure drawtext;
var
 i : integer;
 x : integer;
begin
 x:=pos;

 for i:=zeile to anzlines-1 do
  begin
   if i>=0 then
    textout_centre (aktp, font, text[i], xges DIV 2, x,makecol(255,0,0));
   x:=x+20;
   if x>=xges then
    i:=anzlines-1;
  end;
end;

procedure nextframe;
begin
 blit (aktp,screen,0,0,0,0,xges,yges);
 blit (hinterg,aktp,0,0,0,0,xges,yges);
end;

procedure init;
var
 i : integer;
begin
 {Sinus Tabelle füllen...}
 for i:=0 to 100 do
  begin
   sint[i] := fixtoi ( fsin(ftofix (128/100 * i)) * 200);
  end;
end;

procedure ende_run;
begin
 music_abspann;
 pos:=20;
 zeile:=0;

 settext;
 setpics;

 xges:=640;
 yges:=480;
 aktp:=create_bitmap (xges,yges);
 clear (aktp);
 hinterg:=loadbild (PFAD_ENDE+'ende.bmp',xges,yges);
 if hinterg=nil then
  fehler ('Fehler beim Laden des Hintergrundbildes für Abschlusseffekt');

 { Transparent zeichnen }
 text_mode (-1);

 aktpic:=0;
 gese.gfx:=picts[0];
 gese.px:=xges div 2;
 gese.py:=yges div 2-(PICH div 2);
 gese.b:=PICB;
 gese.h:=PICH;
 gese.efftyp:=hold;
 gese.timepos:=0;

 zeile:=(anzlines-1)-(xges DIV 16)-1;

 if install_int_ex (@effektint,BPS_TO_TIMER(50)) < 0 then
  fehler ('Fehler beim setzen des Interrupt handlers für Abschlusseffekt');

 while key^[KEY_ESC]=0 do
  begin
   drawtext;
   doeffekt(gese);
   nextframe;
  end;

 exiteffekt (gese);

 {Speicher freigeben...}
 remove_int (@effektint);
 destroy_bitmap(aktp);
 destroy_bitmap(hinterg);
 delpics;
 music_menu;
end;

begin
end.
