unit sound;

interface

procedure sound_init_menu;
procedure sound_init_spiel;
procedure sound_run;
procedure sound_exit_menu;
procedure sound_exit_spiel;

implementation

uses message,gvars,fehler,allunit,config;

var
 schon : boolean;
 motor : SAMPLE_ptr;
 start,brems,kcrash,gcrash,over,final, tute : SAMPLE_ptr;
 klick, ok, abbruch, intro, ende, tests : SAMPLE_ptr;

procedure sound_menu_rcv(group, num, arg1, arg2 : integer);
var
 soundp : SAMPLE_ptr;
 i : integer;
begin
 if num = 1 then begin
  soundp:=nil;
  case arg2 of
   1 : soundp:=klick;
   2 : soundp:=ok;
   3 : soundp:=abbruch;
   4 : soundp:=ende;
   5 : soundp:=intro;
  end;
  if soundp=nil then
   sendmsgs (DEBUG,0,0,'Achtung! Falsche Soundnummer!')
  else
   i:=play_sample (soundp,255,128,1000,0);
 end;
end;

procedure sound_spiel_rcv(group, num, arg1, arg2 : integer);
var
 v : integer;
 soundp,soundh : SAMPLE_ptr;
 i : integer;
begin
 soundh:=motor;
 v:=1000;

 if num=2 then
  begin
   soundp:=start;
   case arg2 of
    1 : soundp:=start;
    2 : soundp:=brems;
    3 : soundp:=kcrash;
    4 : soundp:=gcrash;
    5 : soundp:=over;
    6 : soundp:=final;
   12 : soundp:=tute;
   end;
   i:=play_sample (soundp,255,128,1000,0);
  end;

if num=3 then
 begin
     case arg2 of
     1 : begin
         soundh:=motor;
         {case arg1 of}
             v:= 800 + arg1*3;
            { 36.. 65 : v:= 900;
             66..100  : v:=1000;
            101..135  : v:=1100;
            136..200  : v:=1200;}
         { end;}
         end;


    end;
  if arg1 <> 0 then
                begin
                 if schon = false then
                  begin
                   schon := true;
                   i:=play_sample (soundh,255,128,1,1);
                  end
                 else
                 adjust_sample (soundh,255,128,v,1);
                end
               else adjust_sample (soundh,255,128,v,0);
 end;
end;

{Gibt Samples aus dem Menü frei!}
procedure sound_free_menu;
begin
 if klick<>nil then
  destroy_sample (klick);
 if ok<>nil then
  destroy_sample (ok);
 if abbruch<>nil then
  destroy_sample (abbruch);
 if ende<>nil then
  destroy_sample (ende);
 if intro<>nil then
  destroy_sample (intro);
end;

{Samples aus dem Spiel freigeben}
procedure sound_free_spiel;
begin
 if start<>nil then
  destroy_sample (start);
 if brems<>nil then
  destroy_sample (brems);
 if kcrash<>nil then
  destroy_sample (kcrash);
 if gcrash<>nil then
  destroy_sample (gcrash);
 if over<>nil then
  destroy_sample (over);
 if final<>nil then
  destroy_sample (final);
 if tute<>nil then
  destroy_sample (tute);
 if motor<>nil then
  destroy_sample (motor);
end;

procedure sound_init_menu;
var
 sound_fehler : String(255);
begin
 if sound_enabled then begin
  sound_fehler:='';
  {menü}
  klick   := load_sample(PFAD_SOUND+'klick.wav');
  if klick=nil then
   sound_fehler := 'Sound Klick nicht gefunden!';
  ok      := load_sample(PFAD_SOUND+'ok.wav');
  if ok=nil then
   sound_fehler := 'Sound Ok nicht gefunden!';
  abbruch := load_sample(PFAD_SOUND+'abbruch.wav');
  if abbruch=nil then
   sound_fehler := 'Sound Abbruch nicht gefunden!';
  ende    := load_sample(PFAD_SOUND+'ende.wav');
  if ende=nil then
   sound_fehler := 'Sound Ende nicht gefunden!';
  intro   := load_sample(PFAD_SOUND+'intro.wav');
  if intro=nil then
   sound_fehler := 'Sound Intro nicht gefunden!';

  if sound_fehler <> '' then begin
   sendmsgs (DEBUG,0,0,'Sound is deactivated! Cause:');
   sendmsgs (DEBUG,0,0,sound_fehler);
   sound_enabled:=false;
   sound_free_menu;
  end else
   addrcv (SOUND, @sound_menu_rcv);
 end;
end;

procedure sound_init_spiel;
var
 sound_fehler : String(255);
begin
 if sound_enabled then begin
  sound_fehler:='';
  {im spiel}
  start := load_sample(PFAD_SOUND+'start.wav');
   if start=nil then
    sound_fehler :='Sound Start1 nicht gefunden!';

  brems  := load_sample(PFAD_SOUND+'brems.wav');
   if brems=nil then
    sound_fehler :='Sound Brems nicht gefunden!';

  kcrash := load_sample(PFAD_SOUND+'kcrash.wav');
   if kcrash=nil then
    sound_fehler :='Sound Kcrash nicht gefunden!';

  gcrash := load_sample(PFAD_SOUND+'gcrash.wav');
   if gcrash=nil then
    sound_fehler :='Sound Gcrash nicht gefunden!';

  over   := load_sample(PFAD_SOUND+'over.wav');
   if over=nil then
    sound_fehler :='Sound Over nicht gefunden!';

  final  := load_sample(PFAD_SOUND+'final.wav');
   if final=nil then
    sound_fehler :='Sound final nicht gefunden!';

  tute   := load_sample(PFAD_SOUND+'tute.wav');
   if tute=nil then
    sound_fehler :='Sound Tute nicht gefunden!';

  {Hintergrund+Motor}
  motor  := load_sample(PFAD_SOUND+'motor3.wav');
   if motor=nil then
    sound_fehler :='Sound Motor nicht gefunden!';

  if sound_fehler <> '' then begin
   writeln (sound_fehler);
   sendmsgs (DEBUG,0,0,'Sound is deactivated! Cause:');
   sendmsgs (DEBUG,0,0,sound_fehler);
   sound_enabled:=false;
   sound_free_spiel;
  end else
   addrcv (SOUND, @sound_spiel_rcv);
 end;
end;

procedure sound_run;
begin
end;

procedure sound_exit_menu;
begin
 if sound_enabled then begin
  delrcv (@sound_menu_rcv);

  sound_free_menu;
 end;
end;

procedure sound_exit_spiel;
begin
 if sound_enabled then begin
  delrcv (@sound_spiel_rcv);
  sound_free_spiel;
 end;
end;

begin
 {Alle Zeiger auf NULL setzen}
 motor := nil;
 start := nil;
 brems := nil;
 kcrash := nil;
 gcrash := nil;
 over := nil;
 final := nil;
 tute := nil;
 klick := nil;
 ok := nil;
 abbruch := nil;
 intro := nil;
 ende := nil;
 tests := nil;
 schon := false;
end.
