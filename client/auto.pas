{      - Unit mit Datenstrukturen und Hilfsfunktionen für Autos -

  Funktion	 : Bereitstellen der Datenstrukturen und zugeh”rigen
  		    Funktionen um einen Spieler und den Typ seines Wagens
                    darzustellen.
  Team		 : Physik + Message
  letzte Änderung: 9.6.00}

unit auto;

interface

uses allunit,config;

{*******************
 * Datenstrukturen *
 *******************}

type
 {Ein Autotyp}
 autodaten = record
 	name : CString; {Name des Autotyps}
	b,h : integer;
	flags : integer;
	reifen : real;	  {Profilstärke der Reifen}
        ps : real;	  {Stärke des Motors}
	gfx : BITMAP_ptr;
        end;

 autodaten_ptr = ^autodaten;

 autos = record
 	anz : integer; {Anzahl der Typen im Array}
        d : array [0..MAX_AUTOS-1] of autodaten; {Das eigentliche Array mit den
        					Typbeschreibungen}
        end;

 autos_ptr = ^autos;

{**************************
 * Funktionsdeklarationen *
 **************************}

{Läd die Beschreibung der einzelnen Autotypen aus einer Datei}
function autos_load (a: autos_ptr; datei : CString) : integer; C;
{Löscht Autobilder aus dem Speicher}
procedure autos_free (var a: autos);

{Rückgabewert der Funktionen:
  0  bei Erfolg
  <0 bei Fehler}

{----------------------------------------------------------------------------}

{*********************************
 * Implementation der Funktionen *
 *********************************}

implementation

procedure autos_free (var a: autos);
var
 i : integer;
begin
 for i:=0 to a.anz-1 do begin
  if a.d[i].gfx<>nil then
   destroy_bitmap (a.d[i].gfx);
 end;
end;

begin
end.
