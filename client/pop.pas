{ 			- Hauptprogramm zu Pop -

  Funktion	 : Initialisiert Allegro und ruft die einzelen Init und Exit
  		   Prozeduren der anderen Gruppen, und führt die Hauptschleife
                   durch
  Team		 : Physik + Message
  letzte Änderung: 12.6.2000}

program pop;

uses message, gvars, menu, netclient, grafik, sound, allunit,
     fehler, tasten, msgnet, gpc, gpcutil, config, auto, music;

function check_level : boolean;
var
 x, y : integer;
 t : boolean;
begin
	t:=true;
	for y:=0 to alevel.h-1 do
		for x:=0 to alevel.b-1 do begin
			if alevel.l[x,y]>=strtanz then 
				t:=false;
		end;
	check_level:=t;
end;

procedure pop_main;
var 
 i : integer;
begin
 {Allegro Initialisieren}
 initalleg;

 load_config;

 {Sound Initialisieren}
 if install_sound (DIGI_AUTODETECT, MIDI_NONE, 'gesamt') < 0 then begin
  writeln ('Fehler beim initialisieren des Soundtreibers!');
  sound_enabled:=false;
 end else begin
  sound_enabled:=true;
 end;

 {In den Grafikmodus wechseln}
 set_color_depth(16);
 if set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 480*2) < 0 then begin
  if set_gfx_mode(GFX_AUTODETECT,640,480,0,0) < 0 then
   fehler ('Fehler beim setzen des Grafikmodus (640x480x16Bit)');
  i:=alert ('Achtung!','Ihre Grafikkarte unterstuetzt kein Page Flipping',
	'Die Grafik koennte Flimmern und Flackern!','OK',nil,KEY_ENTER,0);
  nopageflip:=true;
 end else begin
  nopageflip:=false;
 end;

 {Oh Scheiss Windows!!}
 alvarsinit;

 {Status initialisieren}
 gvars_init;
 sendmsg (SETUP,1,STATUS_NO_CONNECT,0);
 music_init;

 {Hauptschleife bis STATUS auf beenden steht}
 while status<>STATUS_ENDE do begin

  case status of
   {STATUS_NO_CONNECT -> Hauptmenü}
   STATUS_NO_CONNECT: begin
    if (lstatus<>STATUS_WAIT) then begin
     menu_init;
     sound_init_menu;
    end;

    music_menu;
    menu_draw;

    while (status=STATUS_NO_CONNECT) do begin
     menu_run;
     sound_run;
     msgnet_run;
     SleepMicroSeconds(50000);
    end;

    if status<>STATUS_WAIT then begin
     menu_exit;
     sound_exit_menu;
    end;
   end;

   {STATUS_WAIT -> Einstellungen}
   STATUS_WAIT: begin
    if (lstatus<>STATUS_NO_CONNECT) then begin
     menu_init;
     sound_init_menu;
    end;

    menu_draw;

    while status=STATUS_WAIT do begin
     menu_run;
     sound_run;
     netclient_run;
     msgnet_run;
     {Wir sind lieb zum OS :-) }
     SleepMicroSeconds(50000);
    end;

    if status<>STATUS_NO_CONNECT then begin
     menu_exit;
     sound_exit_menu;
    end;
    if (status=STATUS_NO_CONNECT) or (status=STATUS_ENDE) then begin
     netclient_exit;
     music_menu;
    end;
   end;

   {Laden}
   STATUS_LOAD: begin
    draw_load;

    load_message ('Lade Auto Daten...');
    if load_autos < 0 then begin
     sendmsg (SETUP,1,STATUS_NO_CONNECT,0);
     load_messagea ('Fehler!');
    end else
    load_messagea ('OK!');

    load_message ('Ueberpruefe Daten...');
     netclient_check;
    load_messagea('OK!');

    load_message ('Lade Grafik+Daten der Strecke...');
     grafik_load;
    load_messagea ('OK!');

    load_message ('Lade Sound Daten...');
     sound_init_spiel;
    load_messagea ('OK!');

    load_message ('Initialisiere Tastatur...');
    tasten_init;
    load_messagea ('OK!');

    sendmsg (SETUP+NET,70,0,0); {OK-Signal alle lokalen Daten geladen...}

    load_message ('Warte auf Level...');
    while status=STATUS_LOAD do begin
     netclient_run;
     msgnet_run;
     if levelok = true then begin
	{Level testen}
	if not check_level then begin
		load_messagea ('Ups, inkompatibler Level');
		sendmsg (SETUP+NET,1,STATUS_NO_CONNECT,0);
	end else begin
	      load_messagea ('OK!');
	      levelok:=false;
	      sendmsg (SETUP+NET,75,0,0);
	end;
     end;
    end;

    if (status<>STATUS_RUN) and (STATUS<>STATUS_PAUSE) then begin
     grafik_exit;
     autos_free(typen);
     sound_exit_spiel;
     tasten_exit;
    end;

    if (status=STATUS_NO_CONNECT) or (status=STATUS_ENDE) then begin
     netclient_exit;
    end;
   end;

   {Spiel läuft}
   STATUS_RUN: begin
    grafik_init;	{Bildschirmmodus wechseln}

    while status=STATUS_RUN do begin
     tasten_run;
     netclient_run;
     sound_run;
     grafik_run;
     msgnet_run;
     if (key^[KEY_ESC]<>0) then
      sendmsg (SETUP,1,STATUS_NO_CONNECT,0);
    end;

    if status<>STATUS_PAUSE then begin
     autos_free(typen);
     sound_exit_spiel;
     tasten_exit;
     grafik_exit;
    end;
    if (status=STATUS_NO_CONNECT) or (status=STATUS_ENDE) then begin
     netclient_exit;
    end;
   end;

   {Pause}
   STATUS_PAUSE: begin
    while status=STATUS_PAUSE do begin
     netclient_run;
     sound_run;
     grafik_run;
    end;

    if (status<>STATUS_RUN) and (STATUS<>STATUS_PAUSE) then begin
     autos_free(typen);
     sound_exit_spiel;
     tasten_exit;
     grafik_exit;
    end;
    if (status=STATUS_NO_CONNECT) or (status=STATUS_ENDE) then begin
     netclient_exit;
    end;
   end;

   {Ende}
   STATUS_ENDE: begin

   end;

  end; {Case of}
 end; {while}

 music_exit;

 msgnet_exit;
 gvars_exit;

 {Allegro Beenden}
 remove_sound;
 if (set_gfx_mode (GFX_TEXT,0,0,0,0) < 0) then
  fehler ('Fehler beim zurückschalten in den Textmodus ?! :-/');

 exitalleg;

 writeln (FROM_MESSAGE);
end;

begin
 pop_main;
end.
