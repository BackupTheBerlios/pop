{				-Music Hilfe (funktioniert im Moment nur in Linux)-		}

unit music;

interface

procedure music_init;
procedure music_exit;
procedure music_abspann;
procedure music_menu;

implementation

uses config, gpc, fehler, gvars, message;

procedure play_music(fname : String);
begin
	{$ifdef LINUX}
	Execute ('playmusic '+PFAD_MUSIC+fname);
	{$endif}
end;

procedure music_abspann;
begin
	play_music ('mitwirk.mp3');
end;

procedure music_menu;
begin
	play_music ('menu1.mp3');
end;

procedure music_rcv(group, num, arg1, arg2 : integer);
begin
 if num=1 then begin
  case arg1 of
   0: play_music ('menu2.mp3');
   1: play_music ('loading.mp3');
   2: play_music ('game1.mp3');
   3: play_music ('game2.mp3');
   4: play_music ('menu1.mp3');
  end;
 end;
end; 

procedure music_init;
begin
 {$ifdef LINUX} {geht nur unter Linux :( }
  addrcv (SETUP, @music_rcv);
 {$endif}
end;

procedure music_exit;
begin
 {$ifdef LINUX}
  delrcv(@music_rcv);
  Execute ('nomusic');
 {$endif}
end;

begin
end.
