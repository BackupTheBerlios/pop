{ 			- Datenstrukturen zur Beschreibung eines Levels -

  Funktion	 : Datenstrukturen + Lade/Speicher funktionen eines Levels
  Team		 : Physik + Message
  Änderungen	 :
  	25.5.2000   Änderungsliste eingeführt
        	    Levelrecord erweitert (Startpunkte + Reserviert)
                    Aufgeräumt
                    Message aus Uses entfernt}
unit leveldat;

interface

uses allunit,config;

{**************
 * Konstanten *
 **************}

const
 geom_linie = 1;
 geom_kreisro = 2; {Viertelkreis rechts oben}
 geom_kreislo = 3; {Viertelkreis links oben}
 geom_kreisru = 4; {Viertelkreis rechts unten}
 geom_kreislu = 5; {Viertelkreis links oben}

{*******************
 * Datenstrukturen *
 *******************}

type
 {Geometrische Beschreibung von Linie und Kreisen in der Strecke}
 strgeom = record
        typ : integer;	{Typ des geometrischen Objekts}
        x1,y1,x2,y2 : integer; {Daten}
        end;

 {Beschreibung eines Streckenteils}
 strteil = record
 	gfx : BITMAP_ptr; 	{Grafik Bitmap}
        b,h : integer;        	{Breite, H”he in Pixel}
        flags : integer;
        anzg : integer;        	{Anzahl geometrische Objekte}
        {das eigentliche Array der Objekte}
        gobj : array [0..STRMAXLINES-1] of strgeom;
        end;

 {Array aller Streckenteiltypen}
 strteile = record
    anz : integer; {Anzahl Streckenteile}
    t : array [0..MAXSTRTEILE-1] of strteil;
    end;

 strteile_ptr = ^strteile;

 {Datentyp für Streckenteil}
 strtyp = byte;

 pos = record
    x, y : ShortCard;
 end;

 {Array mehrerer Streckenteile -> ein Level! }
 level = record
    b,h : integer; {Breite + Hoehe des Levels}
    cpoints : array [0..5] of pos;
    reserver : array [0..20] of integer; {Für zukünftige Erweiterungen}
    l : array [0..LEVELMAXX-1,0..LEVELMAXY-1] of strtyp;
    end;

 void_p = ^void;

{***************************
 * Funktions Deklarationen *
 ***************************}

{Läd einen Level aus einer Datei}
function level_load(var l : level; datei : String) : integer;
{Speichert einen Level in eine Datei}
function level_save(l : level; datei : String) : integer;
{Erzeugt einen einfache Testlevel}
procedure level_test(var l : level);
{erzeugt aus einem Messagestrom einen Level}
procedure level_trans (var l : level; anz : integer; buf : void_p);
{Liefert die Anzahl der ben”tigten Messages für diese Levelgr”ÿe}
function level_anzdata(var l : level) : integer;

{Lädt alle Grafiken aus einem Verzeichnis in ein Streckenteil Array}
function level_strload (s : strteile_ptr; verzeichnis : CString) : integer; C;
{L”scht alle Grafiken wieder aus den Speicher}
procedure level_strdestroy (var s : strteile);

{Rückgabewerte der Funktionen:
  0 bei Erfolg
 <0 bei Fehler}

{----------------------------------------------------------------------------}

implementation

uses gpc;

{**********************************
 * Implementierung der Funktionen *
 **********************************}

function level_load(var l : level; datei : String) : integer;
var
 dat : file of level;
begin
 if FileExists (datei) = false then
  level_load := -1
 else begin
  assign (dat,datei);
  reset (dat);
  if filesize(dat)=0 then
   level_load := -2
  else begin
   read (dat,l);
   level_load := 0;
  end;
  close (dat);
 end;
end;

function level_save(l : level; datei : String) : integer;
var
 dat : file of level;
begin
 assign (dat,datei);
 rewrite (dat);

 write (dat,l);

 close (dat);

 if FileExists (datei) = false then
  level_save := -1
 else
  level_save := 0;
end;

procedure level_test(var l : level);
begin
 {... Testlevel setzen ...}
 l.b:=3;
 l.h:=3;

 l.l[0,0]:= 2; l.l[1,0]:= 1; l.l[2,0]:= 3;
 l.l[0,1]:= 0; l.l[1,1]:= 6; l.l[2,1]:= 0;
 l.l[0,2]:= 5; l.l[1,2]:= 1; l.l[2,2]:= 4;
end;

type
 tarray = array [0..LEVELTRANS-1] of strtyp;

procedure level_trans(var l:level; anz : integer; buf : void_p);
var
 ptr : ^tarray;
 x,y : integer;
 bufpos : integer;
begin
 ptr:=buf;
 bufpos:=0;

 if anz=0 then begin
  x:=0;
  y:=0;
 end else begin
  y:=(l.b*l.h) div (LEVELTRANS*anz);
  x:=(l.b*l.h) mod (LEVELTRANS*anz);
 end;

 for y:=y to l.h-1 do begin
  for x:=x to l.b-1 do begin
   l.l[x,y]:=ptr^[bufpos];
   bufpos:=bufpos+1;
   if bufpos >= LEVELTRANS then
    break;
  end;
  if bufpos >= LEVELTRANS then
   break;
  x:=0;
 end;
end;

function level_anzdata(var l : level) : integer;
begin
 if ((l.b*l.h) mod LEVELTRANS) <> 0 then
  level_anzdata := ((l.b*l.h) div LEVELTRANS)+1
 else
  level_anzdata := ((l.b*l.h) div LEVELTRANS);
end;

procedure level_strdestroy (var s : strteile);
var
 i : integer;
begin
 for i:=0 to s.anz-1 do
  if s.t[i].gfx<>nil then
   destroy_bitmap (s.t[i].gfx);
end;

begin
end.
