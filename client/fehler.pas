unit fehler;

interface

procedure fehler(s : String);

implementation

uses allunit,netclient,config;

procedure fehler (s : String);
var
 du : integer;
begin
du:=set_gfx_mode (GFX_TEXT,0,0,0,0);
exitalleg;
netclient_exit;
remove_sound;
writeln (s);
writeln;
writeln ('Schade, Schade, Schade, überprüfen sie bitte ihr System:');
writeln ('  Sie benötigen mindestens:');
writeln (' -Einen Prozessor mit 166Mhz');
writeln (' -Eine Grafikkarte, die unter DOS 640x480 Pixel bei 65536 Farben kann');
writeln (' -Eine Soundkarte, die zu ESS oder Soundblaster kompatibel ist');
writeln (' -Ein funktionierendes TCP/IP Netzwerk');
writeln (' -Einen installierten Dos TCP/IP Treiber installiert ist (liegt dem Spiel bei)');
writeln (' -Eine Tastatur und eine Maus! ;-)');
writeln;
writeln ('und wenn sie all das haben: viel Motivation beim Fehlersuchen!');
writeln;
writeln;
writeln (FROM_MESSAGE);
halt(1);
end;

begin
end.
