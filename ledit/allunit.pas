{ 		- Deklaration der Allegro Funktionen in Pascal -

  Funktion	 : Nötig um Allegro unter Pascal zu nutzen
  Team		 : Physik + Message
  Änderungen	 :
  	25.5.2000: Änderungsliste eingefügt
                   AL_ID wieder entfernt (man muÿ also AUTODETECT nutzen
                   etwas aufgeräumt
	27.5.2000: Anpassungen an Allegro WIP version!
		   zum 2. Mal Keyboard defines!!!}
unit allunit;

interface

{ **************
  * Konstanten *
  **************}

const GFX_TEXT=-1;
const GFX_AUTODETECT=0;

const PAL_SIZE=256;

{ ********************
  * Type deklaration *
  ********************}

type intp = ^Integer;
     chp = ^ByteInt;
     uchar = ByteCard;
     uchp = ^uchar;
     ulong = MedCard;
     ulongp = ^ulong;
     voidp = ^void;
     fixed = ulong;
     long = MedInt;

     word = ShortCard;
     TString = string(256);	{ Pascal string schema }
     {CString = __cstring__;}	{ C style string }

     BITMAP_ptr = ^BITMAP;
     getpixel_ptr = ^function (BITMAP_ptr, integer, integer) : integer;
     putpixel_ptr = ^procedure (BITMAP_ptr, integer, integer, integer);
     line_ptr = ^procedure (BITMAP_ptr, integer, integer, integer, integer, integer);
     FPtr = ^procedure;

     { Bem.: Nicht alle Funktionen besitzen den richtigen Zeiger,
             wenn man sie nutzen will, muÿ man erst den richtigen Zeigertyp
             deklarieren (ich war zu faul ;-)}
    GFX_VTABLE = RECORD
        bitmap_type : integer;
        color_depth : integer;
        mask_color : integer;
        hw_blit : integer;

        set_clip : FPtr;
        getpixel : getpixel_ptr;
        putpixel : putpixel_ptr;
        (* 26 Functions *)
        f1 : FPtr;
        f2 : FPtr;
        line : line_ptr;
        f4 : FPtr;
        f5 : FPtr;
        f6 : FPtr;
        f7 : FPtr;
        f8 : FPtr;
        f9 : FPtr;
        f10 : FPtr;
        f11 : FPtr;
        f12 : FPtr;
        f13 : FPtr;
        f14 : FPtr;
        f15 : FPtr;
        f16 : FPtr;
        f17 : FPtr;
        f18 : FPtr;
        f19 : FPtr;
        f20 : FPtr;
        f21 : FPtr;
        f22 : FPtr;
        f23 : FPtr;
        f24 : FPtr;
        f25 : FPtr;
        f26 : FPtr;
    end;

    GFX_VTABLE_ptr = ^GFX_VTABLE;

    BITMAP = RECORD
          w, h : integer;
          clip : integer;
          cl, cr, ct, cb : integer;
          vtable : GFX_VTABLE_ptr;
          write_bank : FPtr;
          read_bank : FPtr;
          dat : chp;
          bitmap_id : integer;
          extra : chp;
          x_ofs,y_ofs : integer;
          seg : integer;
          line : uchp;
     END;


     RGB = RECORD
         r, g, b : uchar;
         filler : uchar;
     END;

     RGB_ptr = ^RGB;

     PALETTE = array [ 1 .. 256 ] of RGB;

     PALETTE_ptr = ^PALETTE;

const FONT_SIZE=    224;            { number of characters in a font }

type
	FONT_VTABLE_ptr = voidp;	{VTABLE not declared in allegro.h}

	FONTR=RECORD                 { can be either }
	  data : voidp;
	  height : integer;
          vtable : FONT_VTABLE_ptr;
	end;

       FONT_ptr = ^FONTR;

{ Konstanten }
const    MOUSE_FLAG_MOVE=1;
         MOUSE_FLAG_LEFT_DOWN=2;
         MOUSE_FLAG_LEFT_UP=4;
         MOUSE_FLAG_RIGHT_DOWN=8;
         MOUSE_FLAG_RIGHT_UP=16;
         MOUSE_FLAG_MIDDLE_DOWN=32;
         MOUSE_FLAG_MIDDLE_UP=64;

type
	mcallback = ^procedure (flags : integer);

const   KB_SHIFT_FLAG=        $0001;
	KB_CTRL_FLAG=         $0002;
	KB_ALT_FLAG=          $0004;
	KB_LWIN_FLAG=         $0008;
	KB_RWIN_FLAG=         $0010;
	KB_MENU_FLAG=         $0020;
	KB_SCROLOCK_FLAG=     $0100;
	KB_NUMLOCK_FLAG=      $0200;
	KB_CAPSLOCK_FLAG=     $0400;
	KB_INALTSEQ_FLAG=     $0800;
	KB_ACCENT1_FLAG=      $1000;
	KB_ACCENT2_FLAG=      $2000;
	KB_ACCENT3_FLAG=      $4000;
	KB_ACCENT4_FLAG=      $8000;

	KEY_A=                 1;
	KEY_B=                 2;
	KEY_C=                 3;
	KEY_D=                 4;
	KEY_E=                 5;
	KEY_F=                 6;
	KEY_G=                 7;
	KEY_H=                 8;
	KEY_I=                 9;
	KEY_J=                 10;
	KEY_K=                 11;
	KEY_L=                 12;
	KEY_M=                 13;
	KEY_N=                 14;
	KEY_O=                 15;
	KEY_P=                 16;
	KEY_Q=                 17;
	KEY_R=                 18;
	KEY_S=                 19;
	KEY_T=                 20;
	KEY_U=                 21;
	KEY_V=                 22;
	KEY_W=                 23;
	KEY_X=                 24;
	KEY_Y=                 25;
	KEY_Z=                 26;
	KEY_0=                 27;
	KEY_1=                 28;
	KEY_2=                 29;
	KEY_3=                 30;
	KEY_4=                 31;
	KEY_5=                 32;
	KEY_6=                 33;
	KEY_7=                 34;
	KEY_8=                 35;
	KEY_9=                 36;
	KEY_0_PAD=             37;
	KEY_1_PAD=             38;
	KEY_2_PAD=             39;
	KEY_3_PAD=             40;
	KEY_4_PAD=             41;
	KEY_5_PAD=             42;
	KEY_6_PAD=             43;
	KEY_7_PAD=             44;
	KEY_8_PAD=             45;
	KEY_9_PAD=             46;
	KEY_F1=                47;
	KEY_F2=                48;
	KEY_F3=                49;
	KEY_F4=                50;
	KEY_F5=                51;
	KEY_F6=                52;
	KEY_F7=                53;
	KEY_F8=                54;
	KEY_F9=                55;
	KEY_F10=               56;
	KEY_F11=               57;
	KEY_F12=               58;
	KEY_ESC=               59;
	KEY_TILDE=             60;
	KEY_MINUS=             61;
	KEY_EQUALS=            62;
	KEY_BACKSPACE=         63;
	KEY_TAB=               64;
	KEY_OPENBRACE=         65;
	KEY_CLOSEBRACE=        66;
	KEY_ENTER=             67;
	KEY_COLON=             68;
	KEY_QUOTE=             69;
	KEY_BACKSLASH=         70;
	KEY_BACKSLASH2=        71;
	KEY_COMMA=             72;
	KEY_STOP=              73;
	KEY_SLASH=             74;
	KEY_SPACE=             75;
	KEY_INSERT=            76;
	KEY_DEL=               77;
	KEY_HOME=              78;
	KEY_END=               79;
	KEY_PGUP=              80;
	KEY_PGDN=              81;
	KEY_LEFT=              82;
	KEY_RIGHT=             83;
	KEY_UP=	               84;
	KEY_DOWN=              85;
	KEY_SLASH_PAD=         86;
	KEY_ASTERISK=          87;
	KEY_MINUS_PAD=         88;
	KEY_PLUS_PAD=          89;
	KEY_DEL_PAD=           90;
	KEY_ENTER_PAD=         91;
	KEY_PRTSCR=            92;
	KEY_PAUSE=             93;
	KEY_YEN=               94;
	KEY_YEN2=              95;

	KEY_MODIFIERS=         96;

	KEY_LSHIFT=            96;
	KEY_RSHIFT=            97;
	KEY_LCONTROL=          98;
	KEY_RCONTROL=          99;
	KEY_ALT=               100;
	KEY_ALTGR=             101;
	KEY_LWIN=              102;
	KEY_RWIN=              103;
	KEY_MENU=              104;
	KEY_SCRLOCK=           105;
	KEY_NUMLOCK=           106;
	KEY_CAPSLOCK=          107;

	KEY_MAX=               108;

 { Juhuuuuu!!!!!!!!!!  123 Defines umgeschrieben!!!! Was e scheiß Arbeit
  					by M.B.}

const DIGI_VOICES=           64;       { Theoretical maximums: }
      MIDI_VOICES=           64;       { actual drivers may not be }
      MIDI_TRACKS=           32;       { able to handle this many }

      DIGI_AUTODETECT=       -1;       { for passing to install_sound() }
      DIGI_NONE=              0;

      MIDI_AUTODETECT=      -1;
      MIDI_NONE=            0;

      PLAYMODE_PLAY=           0;
      PLAYMODE_LOOP=           1;
      PLAYMODE_FORWARD=        0;
      PLAYMODE_BACKWARD=       2;
      PLAYMODE_BIDIR=          4;

type SAMPLE = RECORD                   { a sample }
   bits : integer;                           { 8 or 16 }
   stereo : integer;                         { sample type flag }
   freq : integer;                           { sample frequency }
   priority : integer;                       { 0-255 }
   len : ulong;                  { length (in samples) }
   loop_start : ulong;           { loop start position }
   loop_end : ulong;             { loop finish position }
   param : ulong;                { for internal use by the driver }
   data : voidp;                 { sample data }
   end;

   SAMPLE_ptr = ^SAMPLE;

   MIDIMSGSTREAM = RECORD
    date : uchp;
    len : integer;
    end;

   MIDI = RECORD                    { a midi file }
    divisions : integer;            { number of ticks per quarter note }
    track : array [0..MIDI_TRACKS] of MIDIMSGSTREAM;
    end;

   MIDI_ptr = ^MIDI;

   AUDIOSTREAM = RECORD
    voice : integer;                     { the voice we are playing on }
    samp : SAMPLE_ptr;                   { the sample we are using }
    b1, b2 : voidp;                      { two audio buffers }
    bufnum : integer;                    { which buffer is currently playing }
    len : integer;                       { buffer length }
    end;

   AUDIOSTREAM_ptr = ^AUDIOSTREAM;

{ ****************
  * globale Vars *
  ****************}

var 
    screen : BITMAP_ptr; {Achtung Windows Hack}
    black_rgb : RGB; asmname 'black_rgb';
    black_palette : PALETTE; asmname 'black_palette';
    desktop_palette : PALETTE; asmname 'desktop_palette';

type
	keytype = array [0..127] of byte;

var key : ^keytype;
 key_shifts : integer; asmname 'key_shifts';

 three_finger_flag : integer; attribute (dllimport); asmname 'three_finger_flag';
 key_led_flag : integer; attribute (dllimport); asmname 'key_led_flag';

 switch_standard_kb_key : integer; asmname 'switch_standard_kb_key';
 switch_custom_kb_key : integer; asmname 'switch_custom_kb_key';
 switch_standard_kb_flags : integer; asmname 'switch_standard_kb_flags';
 switch_custom_kb_flags : integer; asmname 'switch_custom_kb_flags';

{ *********************
  * Allegro allgemein *
  *********************}

type
	ATEXIT_PTR = ^function (f : FPtr) : integer;

const 	SYSTEM_AUTODETECT =	0;

{function allegro_init : integer; C;}
{In der neuen Version nicht mehr vorhanden...}
procedure install_allegro (system_id : integer;errno_ptr : intp; at_exit : ATEXIT_PTR); asmname 'install_allegro';
procedure allegro_exit; C;

function install_mouse : integer; C;
procedure remove_mouse; C;

function install_timer : integer; C;
procedure remove_timer; C;

function install_keyboard : integer; C;
procedure remove_keyboard; C;

{ ******************
  * GFX Procedures *
  ******************}

function set_gfx_mode (card, wi,ho,v_w,v_h : integer): integer; C;

function create_bitmap(w,h : integer) : BITMAP_ptr; C;
function create_bitmap_ex(c,w,h : integer) : BITMAP_ptr; C;
function create_video_bitmap(w,h : integer) : BITMAP_ptr; C;
function create_sub_bitmap(scr : BITMAP_ptr; x,y,w,h : integer) : BITMAP_ptr; C;
function show_video_bitmap(b : BITMAP_ptr) : integer; C;
function request_video_bitmap (b : BITMAP_ptr) : integer; C;

procedure destroy_bitmap (bitmap : BITMAP_ptr); C;

function poll_modex_scroll : integer; C;
procedure request_modex_scroll(x,y : integer); C;

{Farben und Pallete}
procedure set_color_depth (d : integer); C;
procedure set_color(index : integer; p : RGB_ptr); C;

procedure set_palette(p : PALETTE_ptr); C;
procedure get_palette(p : PALETTE_ptr); C;

function makecol(r,g,b : integer) : integer; C;

{Bitmaps laden}
function load_pcx( filename : CString; pal : PALETTE_ptr) : BITMAP_ptr; C;
function load_bitmap(filename : CString; pal : PALETTE_ptr) : BITMAP_ptr; C;
function load_bmp(filename : CString; pal : PALETTE_ptr) : BITMAP_ptr; C;
function load_lbm(filename : CString; pal : PALETTE_ptr) : BITMAP_ptr; C;
function load_pcx(filename : CString; pal : PALETTE_ptr) : BITMAP_ptr; C;
function load_tga(filename : CString; pal : PALETTE_ptr) : BITMAP_ptr; C;

{Bitmaps anzeigen}
procedure blit(source,dest : BITMAP_ptr;source_x,source_y,dest_x,dest_y,width,height : integer); C;
procedure masked_blit(source,dest : BITMAP_ptr;source_x,source_y,dest_x,dest_y,width,height : integer); C;
procedure stretch_blit(s,d : BITMAP_ptr;s_x,s_y,s_w,s_h,d_x,d_y,d_w,d_h : integer); C;
procedure draw_sprite (bmp, sprite : BITMAP_ptr; x,y : integer); C;
procedure draw_sprite_v_flip (bmp, sprite : BITMAP_ptr; x,y : integer); C;
procedure draw_sprite_h_flip (bmp, sprite : BITMAP_ptr; x,y : integer); C;
procedure draw_sprite_vh_flip (bmp, sprite : BITMAP_ptr; x,y : integer); C;
procedure draw_trans_sprite (bmp, sprite : BITMAP_ptr; x,y : integer); C;
procedure draw_lit_sprite (bmp, sprite : BITMAP_ptr; x,y,color : integer); C;
procedure stretch_sprite(bmp,sprite : BITMAP_ptr; x, y, w, h : integer); C;
procedure rotate_sprite(bmp,sprite : BITMAP_ptr; x, y : integer;angle : fixed); C;

{Mal und Zeichen Routinen}
procedure putpixel (bmp : BITMAP_ptr; x, y, color : integer); C;
function getpixel (bmp : BITMAP_ptr; x,y : integer) : integer; C;
procedure line (bmp : BITMAP_ptr;x1,y1,x2,y2,color : integer); C;
procedure clear (bmp : BITMAP_ptr); C;
procedure clear_to_color (bmp : BITMAP_ptr; color : integer); C;
procedure circle(bmp : BITMAP_ptr; x, y, radius, color : integer); C;
procedure triangle(bmp : BITMAP_ptr;x1,y1,x2,y2,x3,y3,color : integer); C;
procedure polygon(bmp : BITMAP_ptr;vertices : integer;points : intp;color : integer); C;
procedure rect(bmp : BITMAP_ptr;x1,y1,x2,y2,color : integer); C;
procedure rectfill (bmp : BITMAP_ptr;x1,y1,x2,y2,color : integer); C;
procedure circle(bmp : BITMAP_ptr;x,y,radius,color : integer); C;
procedure circlefill(bmp : BITMAP_ptr;x,y,radius,color : integer); C;
procedure do_ellipse(bmp : BITMAP;x,y,rx,ry,d : integer;eldo : Fptr); C;
procedure ellipse(bmp : BITMAP_ptr;x,y,rx,ry,color : integer); C;
procedure ellipsefill(bmp : BITMAP_ptr;x,y,rx,ry,color : integer); C;
procedure do_arc(bmp : BITMAP_ptr;x,y : integer;ang1,ang2 : fixed;r,d : integer; doa : FPtr); C;
procedure arc(bmp : BITMAP_ptr;x,y : integer;ang1,ang2 : fixed;r,color : integer); C;
procedure calc_spline(points : intp;npts : integer;x,y : intp); C;
procedure spline(bmp : BITMAP_ptr;points : intp;color : integer); C;
procedure floodfill(bmp : BITMAP_ptr;x,y,color : integer); C;

{Font Prozeduren}
var
font : FONT_ptr;

procedure text_mode(mode : integer); C;
procedure textout(bmp : BITMAP_ptr; f : FONT_ptr; str : CString;x,y,color : integer); C;
procedure textout_centre(bmp : BITMAP_ptr; f : FONT_ptr; str : CString;x, y,color : integer); C;
procedure textout_justify(bmp : BITMAP_ptr; f : FONT_ptr; str : CString;x1,x2,y,diff,color : integer); C;

function text_length(f : FONT_ptr; str : CString) : integer; C;
function text_height(f : FONT_ptr) : integer; C;
procedure destroy_font(f : FONT_ptr); C;


{ *******************
  * Maus Prozeduren *
  *******************}

var
        mouse_x : ^integer;
	mouse_y : ^integer;
	mouse_b : ^integer;
	mouse_pos : ^integer; asmname 'mouse_pos';

	freeze_mouse_flag : integer; asmname 'freeze_mouse_flag';

        mouse_callback : mcallback; asmname 'mouse_callback';

{ Prozeduren }
procedure show_mouse(bmp : BITMAP_ptr); C;
procedure scare_mouse; C;
procedure unscare_mouse; C;
procedure position_mouse(x,y : integer); C;
procedure set_mouse_range(x1, y1, x2, y2 : integer); C;
procedure set_mouse_speed(xspeed, yspeed : integer); C;
procedure set_mouse_sprite(sprite : BITMAP_ptr); C;
procedure set_mouse_sprite_focus(x, y : integer); C;
procedure get_mouse_mickeys(mickeyx, mickeyy : intp); C;

function mouse_needs_poll : integer; C;
procedure poll_mouse; C;

{ ***********************
  * Tastatur Prozeduren *
  ***********************}

function readkey : integer; C;
function keypressed : integer; C;

procedure simulate_keypress(key : integer); C;
procedure clear_keybuf; C;

procedure set_leds(leds : integer); C;
procedure set_standard_keyboard; C;
procedure set_custom_keyboard; C;

function keyboard_needs_poll : integer; C;
procedure poll_keyboard; C;

{ *******************
  * Soundfunktionen *
  *******************}

function install_sound(digi_card,midi_card : integer;cfg_path : CString) : integer; C;
procedure remove_sound; C;

procedure set_volume(digi_volume,midi_volume : integer); C;

function load_sample(filename : CString) : SAMPLE_ptr; C;
function load_wav(filename : CString) : SAMPLE_ptr; C;
function load_voc(filename : CString) : SAMPLE_ptr; C;
function create_sample(bits,stereo,freq,len : integer) : SAMPLE_ptr; C;
procedure destroy_sample(spl : SAMPLE_ptr); C;

function play_sample(spl : SAMPLE_ptr;vol,pan,freq,loop : integer) : integer; C;
procedure stop_sample(spl : SAMPLE_ptr); C;
procedure adjust_sample(spl : SAMPLE_ptr;vol,pan,freq,loop : integer); C;

function allocate_voice(spl : SAMPLE_ptr) : integer; C;
procedure deallocate_voice(voice : integer); C;
procedure reallocate_voice(voice : integer; spl : SAMPLE_ptr); C;
procedure release_voice(voice : integer); C;
procedure voice_start(voice : integer); C;
procedure voice_stop(voice : integer); C;
procedure voice_set_priority(voice,priority : integer); C;
function voice_check(voice : integer) : SAMPLE_ptr; C;

procedure voice_set_playmode(voice,playmode : integer); C;

function voice_get_position(voice : integer) : integer; C;
procedure voice_set_position(voice,position : integer); C;

function voice_get_volume(voice : integer) : integer; C;
procedure voice_set_volume(voice,volume : integer); C;
procedure voice_ramp_volume(voice,time,endvol : integer); C;
procedure voice_stop_volumeramp(voice : integer); C;

function voice_get_frequency(voice : integer) : integer; C;
procedure voice_set_frequency(voice,frequency : integer); C;
procedure voice_sweep_frequency(voice,time,endfreq : integer); C;
procedure voice_stop_frequency_sweep(voice : integer); C;

function voice_get_pan(voice : integer) : integer; C;
procedure voice_set_pan(voice,pan : integer); C;
procedure voice_sweep_pan(voice,time,endpan : integer); C;
procedure voice_stop_pan_sweep(voice : integer); C;

procedure voice_set_echo(voice,strength,delay : integer); C;
procedure voice_set_tremolo(voice,rate,depth : integer); C;
procedure voice_set_vibrato(voice,rate,depth : integer); C;

function load_midi(filename : CString) : MIDI_ptr; C;
procedure destroy_midi(midi : MIDI_ptr); C;
function play_midi(midi : MIDI_ptr;loop : integer) : integer; C;
function play_looped_midi(midi : MIDI_ptr;loop_start,loop_end : integer) : integer; C;
procedure stop_midi; C;
procedure midi_pause; C;
procedure midi_resume; C;
function midi_seek(target : integer) : integer; C;
procedure midi_out(data : chp;length : integer); C;
function load_midi_patches : integer; C;

function play_audio_stream(len,bits,stereo,freq,vol,pan : integer) : AUDIOSTREAM_ptr; C;
procedure stop_audio_stream(stream : AUDIOSTREAM_ptr); C;
function get_audio_stream_buffer(stream : AUDIOSTREAM_ptr) : voidp; C;
procedure free_audio_stream_buffer(stream : AUDIOSTREAM_ptr); C;

{ ******************
  * GUI Funktionen *
  ******************}

function alert(s1,s2,s3,b1,b2 : CString;c1,c2 : integer) : integer; C;
function alert3(s1,s2,s3,b1,b2,b3 : CString;c1,c2,c3 : integer) : integer; C;
function file_select(message,path,ext : CString) : integer; C;
function gfx_mode_select (card, wi, ho : intp) : integer; C;
function gfx_mode_select_ex (card,wi,ho,color_depth : intp) : integer ; C;

{ ********************
  * Timer Funktionen *
  ********************}

const TIMERS_PER_SECOND=     1193181;

function install_int_ex(proc : fptr; speed : longint) : integer; C;
function  install_int(proc : fptr; speed : longint) : integer; C;
procedure remove_int(proc : fptr); C;

procedure rest(time : integer); C;


{ *****************************************
  * Hilfsfunktionen (fixpoint arithmetik) *
  *****************************************}

function itofix (num : integer) : fixed; C;
function ftofix (num : real) : fixed; C;
function fixtoi (f : fixed) : integer; C;
function fixtof (f : fixed) : real; C;

function fsqrt(x : fixed) : fixed; C;
function fatan(x : fixed) : fixed; C;
function fatan2(y,x : fixed) : fixed; C;

function fcos(x : fixed) : fixed; C;
function fsin(x : fixed) : fixed; C;
function ftan(x : fixed) : fixed; C;
function facos(x : fixed) : fixed; C;
function fasin(x : fixed) : fixed; C;
function fadd(x,y : fixed) : fixed; C;
function fsub(x,y : fixed) : fixed; C;
function fdiv(x,y : fixed) : fixed; C;
function fmul(x,y : fixed) : fixed; C;

{***********
 * Eigenes *
 ***********}

procedure srand ( num : integer); C;
function rand : integer; C;

procedure initalleg;
procedure exitalleg;
procedure selectgfxmode;
procedure eselectgfxmode;
procedure alvarsinit; C;
function loadbild (fname : String;x,y : integer) : BITMAP_ptr;

function BPS_TO_TIMER(x : longint) : longint;

{---------------------------------------------------------------------------}

implementation

{ C-Stuff, which is needed for Allegro }
var
 errno : integer; asmname 'errno';
function atexit (f : FPtr) : integer; C;
{ --- }

function getscreen : BITMAP_ptr; C;
procedure showptr (p : BITMAP_ptr); C;

procedure initalleg;
var
 i : integer;
begin
 install_allegro (SYSTEM_AUTODETECT, @errno, @atexit);
 i:=install_timer;
 i:=install_keyboard;
 i:=install_mouse;
 if mouse_needs_poll>0 then begin
  writeln ('Maus Polling ben”tigt!');
 end;
 if keyboard_needs_poll>0 then begin
  writeln ('Keyboard Polling n”tig!');
 end;
end;

procedure exitalleg;
begin
 remove_mouse;
 remove_keyboard;
 remove_timer;
 allegro_exit;
end;

procedure selectgfxmode;
var
 card, w, h : integer;
 i : integer;
begin
 i:=set_gfx_mode (GFX_AUTODETECT,320,200,0,0);
 set_palette (@desktop_palette);
 i:=gfx_mode_select ( @card, @w, @h);
 i:=set_gfx_mode (card,w,h,0,0);
end;

procedure eselectgfxmode;
var
 card, w, h, colors : integer;
 i : integer;
begin
 i:=set_gfx_mode (GFX_AUTODETECT,320,200,0,0);
 set_palette (@desktop_palette);
 i:=gfx_mode_select_ex ( @card, @w, @h, @colors);
 i:=set_gfx_mode (card,w,h,0,0);
end;

procedure t;
begin
	three_finger_flag:=key_led_flag;
end;

function loadbild(fname : String; x,y : integer) : BITMAP_ptr;
var
 t,t2 : BITMAP_ptr;
begin
 t := load_bitmap (fname,nil);
 if t=nil then
  loadbild:=nil
 else
  begin
   t2:=create_bitmap(x,y);
   clear_to_color(t2,makecol(255,0,255));
   stretch_sprite(t2,t,0,0,x,y);
   destroy_bitmap(t);
   loadbild:=t2;
  end;
end;

function BPS_TO_TIMER(x : longint) : longint;
begin
	BPS_TO_TIMER:=((TIMERS_PER_SECOND) DIV x);
end;

begin
end.
