program testit;

uses config,leveldat,allunit,gpc;

const
 RANDX = 10;
 RANDY = 10;

var
 s : strteile;
 l : level;
 ax,ay : integer;
 grx, gry : integer;
 anzx,anzy : integer;
 aktp : BITMAP_ptr;
 ateil : integer;
 bary : integer;

procedure show_level;
var
 x,y : integer;
begin
 {Bereich füllen}
 rectfill (aktp,0,0,grx - RANDX - STRTEILGRX - RANDX,gry,makecol(190,190,190));

 {Zeichnen...}
 y:=ay;
 for y:=ay to ay+anzy do begin
  for x:=ax to ax+anzx do begin
   if ((y-ay) < anzy) and ((x-ax) < anzx) and (x < l.b) and (y < l.h ) then
    blit (s.t[ l.l[x,y] ].gfx,aktp, 0,0 , RANDX + (x-ax)* STRTEILGRX,
          	RANDY + (y-ay) * STRTEILGRY, STRTEILGRX, STRTEILGRY);
  end;
 end;
end;

procedure show_bar;
var
 y : integer;
begin
 {Untergrund zeichnen}
 rectfill (aktp, grx - RANDX - STRTEILGRX - RANDX , 0 ,
                 grx, gry, makecol (125,125,125));

 {Teile zeichnen}
 for y:=bary to s.anz-1 do begin
  if (y-bary < anzy) and (y < s.anz) then
   blit (s.t[y].gfx, aktp, 0,0 , grx-RANDX-STRTEILGRX,
         	(y-bary) * (STRTEILGRY+1) +1, STRTEILGRX, STRTEILGRY);
 end;

 {aktuelles Teil markieren}
 if (ateil-bary < anzy) then
  rect (aktp, grx-RANDX-STRTEILGRX-1, (ateil-bary) * (STRTEILGRY+1),
              grx-RANDX+1, (ateil-bary+1) * (STRTEILGRY+1)+1, makecol(255,0,0));
end;

procedure flip;
begin
 scare_mouse;
 draw_sprite (screen,aktp,0,0);
 show_mouse(screen);
end;

procedure mouse_calc;
var
 x,y : integer;
begin
 {Im Zeichenbereich?}
 if (mouse_x^ > RANDX) and (mouse_x^ < grx - RANDX - STRTEILGRX - RANDX) then begin
  x := (mouse_x^ - RANDX) div STRTEILGRX + ax;
  y := (mouse_y^ - RANDY) div STRTEILGRY + ay;

  if (x>=0) and (x<l.b) and (y>=0) and (y<l.h) then begin
   l.l[x,y]:=ateil;
   show_level;
   flip;
  end;
 end;

 {Im Auswahlbereich?}
 if (mouse_x^ > grx-RANDX-STRTEILGRX) and (mouse_x^ < (grx-RANDX)) then begin
  y := (mouse_y^ - RANDY) div (STRTEILGRY-1) + bary;
  if (y < s.anz) then begin
   ateil:=y;
   show_bar;
   flip;
  end;
 end;

end;

procedure change_b;
var
 b,h,ob,oh : integer;
 i : integer;
begin
 remove_mouse;
 remove_keyboard;
 if set_gfx_mode (GFX_TEXT,0,0,0,0) < 0 then begin
	writeln ('Fehler beim setzen des Grafik Modus');
	halt(1);
 end;
 writeln ('Alte Breite:',l.b);
 writeln ('Alte Höhe:',l.h);
 write ('Neue Breite:'); readln (b);
 write ('Neue Höhe:'); readln (h);
 if (b <0) or (h<0) or (b>=LEVELMAXX) or (h>=LEVELMAXY) then begin
  writeln ('Zu groß!');
  readln(b);
 end else begin
  ob:=l.b;
  oh:=l.h;
  l.b:=b;
  l.h:=h;
  {Eventuell Karte auf 0 setzen}
  for b:=0 to l.b-1 do
   for h:=0 to l.h-1 do begin
    if (b >= ob) or (h >= oh) then
     l.l[b,h]:=0;
   end;
 end;
 if set_gfx_mode (GFX_AUTODETECT,grx,gry,0,0) < 0 then begin
	writeln ('Fehler beim setzen des Grafikmodus');
	halt(1);
 end;
 alvarsinit;
 i:=install_keyboard;
 i:=install_mouse;
 show_level;
 show_bar;
 flip;
 show_mouse(screen);
end;

var
 s : strteile;
 datei : String(255);
 wahl : String(255);
 i : integer;
 a : char;
 sc : word;
begin
 write ('Name des Levels:'); readln (datei);

 if FileExists(datei) then begin
  if level_load(l,datei) < 0 then begin
   writeln ('LevelDatei vorhanden, aber Lesefehler!');
   halt(1);
  end;
 end else
  level_test (l);

 initalleg;

 grx:=1024;
 gry:=768;

 set_color_depth(16);
 if set_gfx_mode (GFX_AUTODETECT,grx,gry,0,0) < 0 then begin
	writeln ('Fehler beim Grafikmodus');
	halt(1);
 end;
 alvarsinit;

 aktp:=create_bitmap (grx,gry);
 if aktp = nil then begin
	writeln ('Kein Speicher mehr');
	halt(1);
 end;

 {Werte berechnen...}
 anzx := RANDX + (grx - RANDX - STRTEILGRX - RANDX);
 anzy := RANDY + (gry - RANDY);

 anzx := anzx div STRTEILGRX;
 anzy := anzy div STRTEILGRY;

 writeln ('Levelteile');

 if level_strload (@s,PFAD_LEVELGFX) < 0 then begin
  allegro_exit;
  writeln ('Achtung! Fehler beim lesen der Levelteile!');
  halt(1);
 end;

 writeln ('Teile OK');
 clear_to_color (aktp,makecol(125,125,125));

 bary := 0;
 ateil := 0;
 ax:=0;
 ay:=0;
writeln ('Show Level');

 show_level;
 writeln ('Show1');
 show_bar;
 writeln ('Flip');
 flip;
 writeln ('Main Loop');

 show_mouse(screen);
 while key^[KEY_ESC] = 0 do begin
  {Maus?}
  if mouse_b^ <> 0 then
   mouse_calc;

  {Tastatur?}
  if keypressed <> 0 then begin
   sc:=readkey;
   a:=char(sc);
   sc:=sc shr 8;
   case a of
    "b" : change_b;
   end;

   case sc of
   KEY_RIGHT:
    if (ax+1) <= (l.b-anzx) then begin
     ax := ax + 1;
     show_level;
     flip;
    end;
   KEY_LEFT:
    if (ax-1) >= 0 then begin
     ax := ax - 1;
     show_level;
     flip;
    end;
   KEY_DOWN:
    if (ay+1) <= (l.h-anzy) then begin
     ay := ay + 1;
     show_level;
     flip;
    end;
   KEY_UP:
    if (ay-1) >= 0 then begin
     ay := ay - 1;
     show_level;
     flip;
    end;
   KEY_PGDN:
    if (bary+1) <= (s.anz-anzy) then begin
     bary := bary + 1;
     show_bar;
     flip;
    end;
   KEY_PGUP:
    if (bary-1) >= 0 then begin
     bary := bary - 1;
     show_bar;
     flip;
    end;
   end; {case}
  end; {if keypressed}

 end;

 scare_mouse;

 level_strdestroy (s);

 exitalleg;
 write ('Änderungen speichern (j/n)?'); readln (wahl);
 if wahl<>'n' then begin
  if level_save (l,datei) < 0 then begin
   writeln ('Leider trat ein Fehler beim Speichern auf!');
   writeln ('Schade, Schade, Schade ...');
  end;
 end;

 halt(0);
end.
