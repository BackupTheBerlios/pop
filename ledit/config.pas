{ 			- Allgemeine Einstellungen zum Spiel -

  Funktion	 : wichtige Konstanten, die das ganze Spiel betreffen sind hier gekapselt
  Team		 : Physik + Message
  letzte Änderung: 30.3.2000}

unit config;

interface

{*************
 * Unit Ende *
 *************}

const
 {$ifndef PFAD_POP} 
 PFAD_POP = '../data/';
 {$endif}
 PFAD_ENDE = PFAD_POP + 'grafik/ende/';
 PFAD_LEVEL = PFAD_POP + 'level/';
 PFAD_LEVELGFX = PFAD_POP + 'grafik/strecke/';
 PFAD_MENU = PFAD_POP + 'grafik/menu/';
 PFAD_SOUND = PFAD_POP + 'sound/';

{*****************
 * Unit leveldat *
 *****************}

const
 {Maximale Anzahl Linien(oder Kreisen) in geometrischer Beschreibung
  eines Streckenteils}
 STRMAXLINES = 10;

 {Maximale Anzahl von Streckenteilen}
 MAXSTRTEILE = 20;

 {Maximale Breite (x) und Höhe (y) des Levels}
 LEVELMAXX = 100;
 LEVELMAXY = 100;

 LEVELTRANS = 100;

 {Größe (in Pixel) eines einzelnen Streckenteils}
 STRTEILGRX = 100;
 STRTEILGRY = 100;

{*************
 * Unit Auto *
 *************}

 MAXTYPEN = 10; {Maximale Anzahl Autotypen (kann unterschritten werden)}

 AUTOGRX = 100; {Größe des Autos horizontal in Pixeln}
 AUTOGRY = 100; {Größe des Autos vertikal in Pixeln}

 GFX_DATEITYP = '.pcx'; {Dateityp der Grafiken}
 GFX_PFAD     = PFAD_POP+'grafik\data';

{************
 * Netzwerk *
 ************}

 STDPORT = 1234;

 MAXPLAYERS = 20;

implementation

begin
end.


