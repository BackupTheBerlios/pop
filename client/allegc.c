#include <allegro.h>
#include <stdio.h>

extern volatile int *Mouse_x;
extern volatile int *Mouse_y;
extern volatile int *Mouse_b;
extern BITMAP *Screen;
extern FONT *Font;
extern int *Key;

void alvarsinit()
{
 // und alles wegen fuck Windows!!
 Mouse_x=&mouse_x;
 Mouse_y=&mouse_y;
 Mouse_b=&mouse_b;
 Screen=screen;
 Font=font;
 Key=(int*)key;
}

int _mangled_main();

END_OF_MAIN()
