{ 			- Allgemeine Einstellungen zum Spiel -

  Funktion	 : wichtige Konstanten, die das ganze Spiel betreffen sind hier gekapselt
  Team		 : Physik + Message
  letzte Änderung: 30.3.2000}

unit config;

interface

const
 {Maximale Anzahl Linien(oder Kreisen) in geometrischer Beschreibung
  eines Streckenteils}
 STRMAXLINES = 10;

 {Maximale Anzahl von Streckenteilen}
 MAXSTRTEILE = 20;

 {Maximale Breite (x) und H”he (y) des Levels}
 LEVELMAXX = 100;
 LEVELMAXY = 100;

 LEVELTRANS = 100;

 MAX_AUTOS = 20; {Maximale Anzahl Autotypen (kann unterschritten werden)}

 AUTOGRX = 100; {Größe des Autos horizontal in Pixeln}
 AUTOGRY = 100; {Größe des Autos vertikal in Pixeln}

 MAXPLAYERS = 10;

 FROM_MESSAGE = '			POP ist ein Projekt des Grundkurs Info 2000 am Johanneum';

{*********
 * Pfade *
 *********}

var
 {$ifndef PFAD_POP}
 PFAD_POP : String(255);
 {$endif}
 {$ifndef PFAD_USERDATA}
 PFAD_USERDATA : String(255);
 {$endif}
 PFAD_ENDE : String(255);
 PFAD_LEVEL : String(255);
 PFAD_LEVELGFX : String(255);
 PFAD_AUTOGFX : String(255);
 PFAD_MENU : String(255);
 PFAD_MENU_LEVEL : String(255);
 PFAD_SOUND : String(255);
 PFAD_MUSIC : String(255);

 {Bildschirmaufl”sung im Spiel}
 BILDSCHIRMX : integer;
 BILDSCHIRMY : integer;

 {Größe (in Pixel) eines einzelnen Streckenteils}
 STRTEILGRX : integer;
 STRTEILGRY : integer;

 {Standard Netzwerkport}
 STDPORT : integer;

 MSGNET_ACTIVATED : boolean;



procedure save_config;
procedure std_config;
procedure load_config;

implementation

procedure std_config;
begin
 {$ifndef PFAD_POP}
 PFAD_POP := '../data/';
 {$endif}
 {$ifndef PFAD_USERDATA}
 PFAD_USERDATA := '../userdata/';
 {$endif}
 PFAD_ENDE := PFAD_POP + 'grafik/ende/';
 PFAD_LEVEL := PFAD_POP + 'level/';
 PFAD_LEVELGFX := PFAD_POP + 'grafik/strecke/';
 PFAD_AUTOGFX := PFAD_POP + 'grafik/autos/';
 PFAD_MENU := PFAD_POP + 'grafik/menu/';
 PFAD_MENU_LEVEL := PFAD_POP + 'grafik/level/';
 PFAD_SOUND := PFAD_POP + 'sound/sfx/';
 PFAD_MUSIC := PFAD_USERDATA + 'sound/music/';

 BILDSCHIRMX := 640;
 BILDSCHIRMY := 480;

 STRTEILGRX := BILDSCHIRMX;
 STRTEILGRY := BILDSCHIRMY;

 STDPORT := 1234;
 MSGNET_ACTIVATED := true;
end;

procedure load_config;
begin
 std_config;
end;

procedure save_config;
begin


end;

begin
end.
