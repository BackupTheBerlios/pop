unit grafik;

interface

procedure grafik_load;
procedure grafik_init;
procedure grafik_run;
procedure grafik_exit;
procedure grafik_setmsg(num : integer; msgstr : String);

var
	strtanz : integer;

implementation

uses message, allunit, gvars, auto, netclient, config, fehler, leveldat;

type
  tpos = record
    x, y :integer;
  end;

  msggfx = record
    xp,yp : integer;
    f : integer;		{Blinkfrequenz}
    bild : BITMAP_ptr;
    starttime : integer;
    endtime : integer;
  end;

const
	MSG_ANZ = 7;
	TIMER_FREQ = 10;

var teile : strteile;
    page1 : BITMAP_ptr;
    page2 : BITMAP_ptr;
    apage : BITMAP_ptr;
    timepos : integer;
    m : array [1..MSG_ANZ] of msggfx;
    mldg : String(255);

procedure grafik_load;
var
 i : integer;
begin
 if level_strload (@teile,PFAD_LEVELGFX) < 0 then
  fehler ('Fehler beim Laden der Levelgrafiken!');

 strtanz:=teile.anz;

 {Messagesladen}
 {StartAmpel}
 m[1].bild:=load_bitmap(PFAD_LEVELGFX+'msgstart1.pcx',nil);
 m[1].xp:=640-m[1].bild^.w;
 m[1].yp:=0;
 m[1].f:=0;
 m[1].starttime:=-1;
 m[1].endtime:=-1;
 m[2].bild:=load_bitmap(PFAD_LEVELGFX+'msgstart2.pcx',nil);
 m[2].xp:=640-m[2].bild^.w;
 m[2].yp:=0;
 m[2].f:=0;
 m[2].starttime:=-1;
 m[2].endtime:=-1;
 m[3].bild:=load_bitmap(PFAD_LEVELGFX+'msgstart3.pcx',nil);
 m[3].xp:=640-m[3].bild^.w;
 m[3].yp:=0;
 m[3].f:=0;
 m[3].starttime:=-1;
 m[3].endtime:=-1;

 {Checkpoint}
 m[4].bild:=load_bitmap(PFAD_LEVELGFX+'msgcp.pcx',nil);
 m[4].xp:=0;
 m[4].yp:=0;
 m[4].f:=TIMER_FREQ div 2;     {2 mal pro Sekunde blinken}
 m[4].starttime:=-1;
 m[4].endtime:=-1;

 {Falscher Weg}
 m[5].bild:=load_bitmap(PFAD_LEVELGFX+'msgwway.pcx',nil);
 m[5].xp:=0;
 m[5].yp:=0;
 m[5].f:=TIMER_FREQ div 4;
 m[5].starttime:=-1;
 m[5].endtime:=-1;

 {Runde beendet}
 m[6].bild:=load_bitmap(PFAD_LEVELGFX+'msground.pcx',nil);
 m[6].xp:=0;
 m[6].yp:=0;
 m[6].f:=TIMER_FREQ;
 m[6].starttime:=-1;
 m[6].endtime:=-1;
 
 {letzte Runde}
 m[7].bild:=load_bitmap(PFAD_LEVELGFX+'msgllap.pcx',nil);
 m[7].xp:=0;
 m[7].yp:=0;
 m[7].f:=TIMER_FREQ;
 m[7].starttime:=-1;
 m[7].endtime:=-1;

 {Alles korrekt geladen?}
 for i:=1 to MSG_ANZ do begin
  if m[i].bild = nil then
   fehler ('Fehler beim laden einer MsgGrafik');
 end;
end;

procedure timeint;
begin
 inc(timepos);
end;

procedure grafik_init;
begin
 {Versuchen Page Flipping zu nutzen}
 if not nopageflip then begin
  if page1 = nil then
   page1 := create_video_bitmap(BILDSCHIRMX,BILDSCHIRMY);
  if page2 = nil then
   page2 := create_video_bitmap(BILDSCHIRMX,BILDSCHIRMY);
 end else begin
  {ansonsten Bilder im Speicher aufbauen}
  if page1 = nil then begin
   page1 := create_bitmap(BILDSCHIRMX, BILDSCHIRMY);
   apage := page1;
  end;
 end;
 if apage = nil then
  apage:=page2;
 
 {Timer Einstellen}
 timepos:=0;
 if install_int_ex (@timeint,BPS_TO_TIMER(TIMER_FREQ)) < 0 then
  fehler ('Fehler beim setzen des Interrupt Handlers');
end;

procedure flip_page;
begin
 if not nopageflip then begin
  {Page Flipping}
  if show_video_bitmap(apage) < 0 then
   if page1<>screen then
    fehler ('Fehler beim Seitenwechseln!');

  if apage=page1 then
   apage:=page2
  else
   apage:=page1;
 end else begin
  {kein Page Flipping}
  blit(apage,screen,0,0,0,0,BILDSCHIRMX,BILDSCHIRMY);
 end;
end;

procedure grafik_setmsg(num : integer; msgstr : String);
begin
 case num of 
  11: begin {Ampel1} 
	m[1].starttime:=timepos; m[1].endtime:=timepos+TIMER_FREQ*4;
	m[2].starttime:=-1; m[2].endtime:=-1;
	m[3].starttime:=-1; m[3].endtime:=-1;
	end;
  12: begin {Ampel2}
	m[1].starttime:=-1; m[1].endtime:=-1;
	m[2].starttime:=timepos; m[2].endtime:=timepos+TIMER_FREQ*4;
	m[3].starttime:=-1; m[3].endtime:=-1;
	sendmsg(SOUND,2,0,1);
	end;
  13: begin {Ampel3}
	m[1].starttime:=-1; m[1].endtime:=-1;
	m[2].starttime:=-1; m[2].endtime:=-1;
	m[3].starttime:=timepos; m[3].endtime:=timepos+TIMER_FREQ*4;
	end;
  20: begin {Checkpoint}
	m[4].starttime:=timepos; m[4].endtime:=timepos+TIMER_FREQ*3;
	end;
  21: begin {Runde gepackt}
	if msgstr='letzte' then begin
		{letzte Runde}
		m[7].starttime:=timepos; m[7].endtime:=timepos+TIMER_FREQ*3;
		sendmsg (SOUND,2,0,6);
	end else if msgstr='fertig' then begin
		writeln ('Over');
		sendmsg (SOUND,2,0,5);
	end else begin
		{irgendeine Runde}
		m[6].starttime:=timepos; m[6].endtime:=timepos+TIMER_FREQ*3;
	end;
	end;
  22: begin {Falsche Richtung}
	m[5].starttime:=timepos; m[5].endtime:=timepos+TIMER_FREQ*3;
	end;
 end;
end;

procedure grafik_run;
var bildschirm :array [1..4] of bitmap_ptr; { Pointer auf Grafik}
    teil : array [1..4] of tpos;	{Position in der Teilgrafik}
    teilb : array [1..4] of tpos;	{Höhe + Breite des Ausschnitts}
    location : array [1..4] of tpos;    {teil in dem das auto gerade ist}
    screenpos : array [1..4] of tpos;	{Position auf dem Bildschirm}
    position : tpos;
    i : integer;
    xp,yp:integer;
begin
 position.x := myself^.xp;
 position.y := myself^.yp;

 position.x := position.x - (strteilgrx div 2);
 position.y := position.y - (strteilgry div 2);

 {Postion in der Ecke?}
 if position.x < 0 then position.x := 0;
 if position.y < 0 then position.y := 0;
 if position.x > (alevel.b-1)*strteilgrx then
  position.x:=(alevel.b-1)*strteilgrx;
 if position.y > (alevel.h-1)*strteilgry then
  position.y:=(alevel.h-1)*strteilgry;

 {obere linke Ecke berechnen}
 location[1].x := position.x div strteilgrx; {Koordinate im Array}
 location[1].y := position.y div strteilgry;
 teil[1].x :=  position.x mod strteilgrx; {Koordinate im Array}
 teil[1].y :=  position.y mod strteilgry;
 teilb[1].x := strteilgrx - teil[1].x;
 teilb[1].y := strteilgry - teil[1].y;
 screenpos[1].x := 0;
 screenpos[1].y := 0;

 {Obenrechts }
 location[2].x := location[1].x + 1;
 location[2].y := location[1].y;
 teil[2].x := 0;
 teil[2].y := teil[1].y;
 teilb[2].x := strteilgrx - teilb[1].x;
 teilb[2].y := teilb[1].y;
 screenpos[2].x := teilb[1].x;
 screenpos[2].y := 0;

 {untenlinks}
 location[3].x := location[1].x;
 location[3].y := location[1].y + 1;
 teil[3].x := teil[1].x;
 teil[3].y := 0;
 teilb[3].x := teilb[1].x;
 teilb[3].y := strteilgry - teilb[1].y;
 screenpos[3].x := 0;
 screenpos[3].y := teilb[1].y;

 {untenrechts}
 location[4].x := location[1].x + 1;
 location[4].y := location[1].y + 1;
 teil[4].x := 0;
 teil[4].y := 0;
 teilb[4].x := strteilgrx - teilb[1].x;
 teilb[4].y := strteilgry - teilb[1].y;
 screenpos[4].x := teilb[1].x;
 screenpos[4].y := teilb[1].y;

 bildschirm[1] := teile.t [alevel.l[location[1].x,location[1].y] ].gfx;
 bildschirm[2] := teile.t [alevel.l[location[2].x,location[2].y] ].gfx;
 bildschirm[3] := teile.t [alevel.l[location[3].x,location[3].y] ].gfx;
 bildschirm[4] := teile.t [alevel.l[location[4].x,location[4].y] ].gfx;

 blit(bildschirm[1],apage,teil[1].x,teil[1].y,screenpos[1].x,screenpos[1].y,teilb[1].x,teilb[1].y);
 blit(bildschirm[2],apage,teil[2].x,teil[2].y,screenpos[2].x,screenpos[2].y,teilb[2].x,teilb[2].y);
 blit(bildschirm[3],apage,teil[3].x,teil[3].y,screenpos[3].x,screenpos[3].y,teilb[3].x,teilb[3].y);
 blit(bildschirm[4],apage,teil[4].x,teil[4].y,screenpos[4].x,screenpos[4].y,teilb[4].x,teilb[4].y);

 {Spieler zeichnen}
 for i:=0 to MAXPLAYERS-1 do begin
  if players[i].typ=0 then begin	{Zuschauer}
   xp:=dyndata[i].xp-position.x-(zuschauer.gfx^.w div 2);
   yp:=dyndata[i].yp-position.y-(zuschauer.gfx^.h div 2);
   rotate_sprite (apage,zuschauer.gfx,xp,yp,dyndata[i].richtung);
  end else if players[i].typ=1 then begin	{Spieler}
   xp:=dyndata[i].xp-position.x-(players[i].gfx^.w div 2);
   yp:=dyndata[i].yp-position.y-(players[i].gfx^.h div 2);
   rotate_sprite (apage,players[i].gfx,xp,yp,dyndata[i].richtung);
  end;
 end;

 {Auf Messages prüfen}
 for i:=1 to MSG_ANZ do begin
  if (timepos >= m[i].starttime) and (timepos<m[i].endtime) then begin
   if (m[i].f=0) or
     (((timepos-m[i].starttime) mod (m[i].f*2)) >= m[i].f) then
     begin
      draw_sprite(apage,m[i].bild,m[i].xp,m[i].yp);
     end;
  end;
 end;

 if mldg<>"" then begin
	textout (apage,font,mldg,0,0,makecol(255,0,0));
 end;

 {Anzeigen}
 flip_page;
end;

procedure grafik_exit;
begin
 remove_int(@timeint);
 level_strdestroy(teile);
 if not nopageflip then begin
	if apage <> page2 then
		flip_page;
 end;
end;

var
 i : integer;
begin
 {Messages auf 0 setzen}
 for i:=0 to MSG_ANZ do begin
	m[i].xp:=0;
	m[i].yp:=0;
	m[i].f:=0;
	m[i].starttime:=0;
	m[i].bild:=nil;
 end;
 timepos:=0;
 apage:=nil;
 page1:=nil;
 page2:=nil;
end.

