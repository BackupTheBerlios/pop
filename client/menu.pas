{ 			- Unit Hauptmenü -

  Funktion	 : Darstellen des Hauptmenüs und prüfen auf User Aktionen
  Team		 : Grafik
  letzte Änderung: 30.3.2000}

unit menu;

{*********************
 * ”ffentlicher Teil *
 *********************}

interface

procedure menu_init;
procedure menu_run;
procedure menu_exit;

procedure menu_draw;

{*****************
 * privater Teil *
 *****************}

implementation

uses gvars,allunit,config,fehler,ende,message,msgnet,netclient,eing;

const BUTTON_ANZ = 12; {Anzahl der Buttons in Haupt- und Untermenü}

	{Nummern der Untermenüs}
	MENU_WAIT = 1;
	MENU_STRECKE = 2;

type tbutton = record
	bild1, bild2 : BITMAP_ptr;
        xpos, ypos : integer;
        state : boolean; {true=Mausfocus, false=nich berührt}
        nodel : boolean; { Bild beim Beenden nicht löschen }
      end;
     tmenu = array[0..BUTTON_ANZ] of tbutton;

var m : tmenu;      {das Menüarray mit den Daten}
    txtf : tbutton; {Eingabefeld  für Text}
    subm : integer; {Welches Untermenü}
    lastl : String(255);

procedure check_menu(var m : tmenu; anfang, ende : integer); forward;
procedure draw_menu(m : tmenu; anfang, ende : integer); forward;
function  check_mouse(m : tmenu; i : integer) : boolean; forward;
procedure reaction(i : integer); forward;
procedure check_menu(var m : tmenu; anfang, ende : integer); forward;
procedure init_menu(var b : tmenu); forward;

procedure init_menu(var b:tmenu);
var
 i : integer;
begin
 {--- Grafiken laden ---}
 {Hintergrund}
 b[0].bild1 := load_bitmap(PFAD_MENU+'popback.pcx', nil);
 b[0].bild2 := b[0].bild1;
 b[0].xpos := 0;
 b[0].ypos := 0;

 {Hauptmenü}
 b[1].bild1 := load_bitmap(PFAD_MENU+'spielen1.pcx', nil);
 b[1].bild2 := load_bitmap(PFAD_MENU+'spielen2.pcx', nil);
 b[1].xpos := 430;
 b[1].ypos := 200;

 b[2].bild1 := load_bitmap(PFAD_MENU+'highscore2.pcx', nil);
 b[2].bild2 := load_bitmap(PFAD_MENU+'highscore1.pcx', nil);
 b[2].xpos := 430;
 b[2].ypos := 250;

 b[3].bild1 := load_bitmap(PFAD_MENU+'mitwirkende1.pcx', nil);
 b[3].bild2 := load_bitmap(PFAD_MENU+'mitwirkende2.pcx', nil);
 b[3].xpos := 430;
 b[3].ypos := 300;

 b[4].bild1 := load_bitmap(PFAD_MENU+'ende1.pcx', nil);
 b[4].bild2 := load_bitmap(PFAD_MENU+'ende2.pcx', nil);
 b[4].xpos := 430;
 b[4].ypos := 350;

 {Untermenue "Spielen"}
 b[5].bild1 := load_bitmap(PFAD_MENU+'strecke1.pcx', nil);
 b[5].bild2 := load_bitmap(PFAD_MENU+'strecke2.pcx', nil);
 b[5].xpos := 430;
 b[5].ypos := 200;

 b[6].bild1 := load_bitmap(PFAD_MENU+'auto1.pcx', nil);
 b[6].bild2 := load_bitmap(PFAD_MENU+'auto2.pcx', nil);
 b[6].xpos := 430;
 b[6].ypos := 250;

 b[7].bild1 := load_bitmap(PFAD_MENU+'name1.pcx', nil);
 b[7].bild2 := load_bitmap(PFAD_MENU+'name2.pcx', nil);
 b[7].xpos := 430;
 b[7].ypos := 300;

 b[8].bild1 := load_bitmap(PFAD_MENU+'zurueck1.pcx', nil);
 b[8].bild2 := load_bitmap(PFAD_MENU+'zurueck2.pcx', nil);
 b[8].xpos := 430;
 b[8].ypos := 350;

 {keine Levelgrafik}
 b[9].bild1 := load_bitmap(PFAD_MENU_LEVEL+'nogfx.pcx', nil);
 b[9].bild2 := b[9].bild1;
 b[9].xpos := 0;
 b[9].ypos := 480-100;

 b[10].bild1 := load_bitmap(PFAD_MENU+'weiter1.pcx', nil);
 b[10].bild2 := load_bitmap(PFAD_MENU+'weiter2.pcx', nil);
 b[10].xpos := 430;
 b[10].ypos := 200;

 b[11].bild1 := b[8].bild1; {Bild zurück}
 b[11].bild2 := b[8].bild2;
 b[11].xpos := 430;
 b[11].ypos := 250;
 b[11].nodel := true;

 b[12].bild1 := load_bitmap(PFAD_MENU+'ok1.pcx',nil);
 b[12].bild2 := load_bitmap(PFAD_MENU+'ok2.pcx',nil);
 b[12].xpos := 430;
 b[12].ypos := 300;

 txtf.bild1 := load_bitmap(PFAD_MENU+'textfeld.pcx',nil);
 txtf.xpos := 300;
 txtf.ypos := 200;
 
 for i:=0 to BUTTON_ANZ do
  if (b[i].bild1 = nil) or (b[i].bild2 = nil) then
   fehler ('Fehler beim Laden eines Menü Buttons');

 if (txtf.bild1 = nil) then
  fehler ('Fehler beim Laden des Textfelds!');

 init_larray;

 {Maus initialisieren}
 show_mouse(screen);
end;

{malt das Menü von Menüpunkt "anfang"-"ende"}
procedure draw_menu;
var i: integer;
begin
 scare_mouse;
 draw_sprite (screen, m[0].bild1,m[0].xpos,m[0].ypos);
 for i := anfang to ende do begin
  if m[i].state = true then
   draw_sprite (screen, m[i].bild2, m[i].xpos, m[i].ypos)
  else
   draw_sprite (screen, m[i].bild1, m[i].xpos, m[i].ypos);
 end;
 unscare_mouse;
end;

procedure draw_netstatus;
begin
 if (master=1) or (mytyp=0) then begin
  scare_mouse;
  if master=1 then
   textout (screen,font,'Master',580,460,makecol(0,255,0));
  if mytyp=0 then
   textout (screen,font,'Z',560,460,makecol(50,255,0));
  textout (screen,font,'L:'+lastl,500,460,makecol(50,255,0));
  unscare_mouse;
 end;
 if lastl <> alevelname then begin
  lastl:=alevelname;
  if m[9].bild1<>nil then
   destroy_bitmap(m[9].bild1);
  m[9].bild1:=load_bitmap(PFAD_MENU_LEVEL+lastl+'.pcx',nil);
  if m[9].bild1 = nil then
   m[9].bild1:=load_bitmap(PFAD_MENU_LEVEL+'nogfx.pcx',nil);
  if m[9].bild1 = nil then
   fehler ('nogfx.pcx ist verschwunden (Eben wars doch noch da?!?)');
  m[9].bild2:=m[9].bild1;
  menu_draw;
 end;   
end;

procedure check_menu; {wechselt Buttons aus und reagiert auf Mouseklick}
var i : integer;
begin
   for i:=anfang to ende do
   begin
    if (check_mouse(m, i)) = true then
    begin
      if m[i].state <> true then
        begin
          m[i].state := true;
          scare_mouse;
          draw_sprite(screen, m[i].bild2, m[i].xpos, m[i].ypos);
          unscare_mouse;
          sendmsg (SOUND,1,0,1); {Tutgeräusch abspielen}
        end;
      if ((mouse_b^ and 1)>0) then reaction(i);
     end
    else if (check_mouse(m, i) = false) and (m[i].state <> false) then
         begin
           scare_mouse;
           draw_sprite(screen, m[i].bild1, m[i].xpos, m[i].ypos);
           unscare_mouse;
           m[i].state := false;
         end;
   end;
end;

function check_mouse; {checkt die Mouse} {<-- cool Englisch :) by MB}
begin
  if ((mouse_x^ > m[i].xpos) and (mouse_x^ < m[i].bild1^.w+m[i].xpos) and
     (mouse_y^ > m[i].ypos) and (mouse_y^ < m[i].bild1^.h+m[i].ypos)) then
       check_mouse := true
  else check_mouse := false;
end;

procedure reaction;  {reagiert auf Mouseklick}
var s : string(255);
begin
 sendmsg (SOUND,1,0,2); {Klickgeräusch abspielen}
 while mouse_b^<>0 do begin
  {Irgendwas sinnloses tun... weil mouse_b eigentlich volatile sein müsste}
  {ich aber nicht weis wie das in Pascal geht}
  write ('');
 end;

 case i of
   1 : begin {Button Spielen}
        scare_mouse;
        draw_menu(m,0,0); {malt Hintergrund bund }
        draw_sprite(screen, txtf.bild1, txtf.xpos, txtf.ypos);
        eing(s, txtf.xpos, txtf.ypos);
        unscare_mouse;
        if netclient_connect(s) < 0 then
          begin
             i:=alert ('Fehler beim Verbinden mit dem Server:',netclient_fehler,nil,'OK',nil,KEY_ENTER,0);
             menu_draw;
          end
        else begin
         sendmsg (SETUP,1,STATUS_WAIT,0);
	   subm:=MENU_WAIT;
        end;
       end;
   2 : begin {Button Highscore}
        msgnet_init;
	if mytyp=0 then
         mytyp:=1
        else
         mytyp:=0;
       end;
   3 : begin {Button Mitwirkende}
        scare_mouse;
        ende_run;
	  menu_draw;
        unscare_mouse;
       end;
   4 : begin {Button Ende}
        sendmsg (SETUP,1,STATUS_ENDE,0);
       end;
   5 : begin {Button Strecke}
	  subm:=MENU_STRECKE;
	  menu_draw;
       end;
   6 : begin {Button Auto}
        sendmsg (SETUP+NET,63,0,0);
       end;
   7 : begin {Button Name}
        scare_mouse;
        draw_menu(m,0,0); {malt Hintergrund bund }
        draw_sprite(screen, txtf.bild1, txtf.xpos, txtf.ypos);
        eing(s, txtf.xpos, txtf.ypos);
        sendmsgs (SETUP+NET,60,0,s);
        unscare_mouse;
	  menu_draw;
       end;
   8 : begin {Button zurück}
        sendmsg (SETUP,1,STATUS_NO_CONNECT,0);
       end;
   9 : begin
       end;
   10: begin {Button nächste Strecke}
	  if alevelnum+1<anz_level then begin
		inc(alevelnum);
		sendmsg(SETUP+NET,62,alevelnum,0);
	  end;
      end;
   11: begin {Button letzter Level}
	  if alevelnum>0 then begin
		dec(alevelnum);
		sendmsg(SETUP+NET,62,alevelnum,0);
        end;
       end;
   12 : begin {Button Strecke/Zurück}
		subm:=MENU_WAIT;
		menu_draw;
	  end;
   end;
end;

procedure exit_menu(var b:tmenu);
var
 i : integer;
begin
 scare_mouse;

 for i:=0 to BUTTON_ANZ do
  begin
   if b[i].nodel=false then begin
    if b[i].bild2 <> b[i].bild1 then
     if b[i].bild2 <> nil then
      destroy_bitmap (b[i].bild2);
    if b[i].bild1 <> nil then
     destroy_bitmap (b[i].bild1);
   end;
  end;
 if txtf.bild1<>nil then
  destroy_bitmap(txtf.bild1);
 exit_larray;
 clear(screen);
end;

{ ***************************************
  * Kommunikation mit dem Hauptprogramm *
  ***************************************}

procedure menu_init;
begin
 init_menu (m);

end;

procedure menu_run;
begin
 if status = STATUS_NO_CONNECT then begin
  check_menu (m,1,4);
 end;
 if status = STATUS_WAIT then begin
  if netchanged = true then begin
   menu_draw;
  end else begin
   case subm of
    MENU_WAIT:
 	  check_menu (m,5,8);	{Trick: Bild9 ist der Level, der sich
 					nicht ändert }
    MENU_STRECKE:
 	  check_menu (m,10,12);
   end;
  end;
 end;
end;

procedure menu_exit;
begin
 exit_menu (m);
end;

procedure menu_draw;
begin
 {Je nach Status anderes Menü zeichnen}
 if status = STATUS_NO_CONNECT then begin
  {normales Hauptmenü}
  draw_menu (m,1,4);
 end;
 if status = STATUS_WAIT then begin
  {Level, Name + Autoauswahl}
  case subm of
   MENU_WAIT:
	  draw_menu (m,5,9);
   MENU_STRECKE:
        draw_menu (m,9,12);
  end;
  draw_netstatus;
 end;
end;

{Unit Hauptprogramm - Vorsichtshalber alle Zeiger auf NULL setzen}
var
 i : integer;
begin
 for i:=0 to BUTTON_ANZ do begin
  m[i].bild1:=nil;
  m[i].bild2:=nil;
  m[i].xpos:=0;
  m[i].ypos:=0;
  m[i].state:=false;
  m[i].nodel:=false;
 end;
 lastl:='';
end.

