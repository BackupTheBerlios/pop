/***************************************************************************
                          config.h  -  description
                             -------------------
    begin                : Sun Apr 2 2000
    copyright            : (C) 2000 by Matze Braun
    email                : MatzeBraun@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define VERSION	"0.3"

extern int FPS;
#define TIME_P_F	(1/FPS)

#define STDPORT	1234

#define MAXTYPEN	10

#define STRMAXLINE	5
#define LEVELMAXX	100
#define LEVELMAXY	100

#define LEVELTRANS	100	/* Anzahl Levelteile mit 1 Message versenden */

#define STRTEILE	30

#ifdef DJGPP
 #define DOS_STYLE
#else
 #ifdef CYGWIN
  #define DOS_STYLE
 #else
  #define UNIX_STYLE
 #endif
#endif

#ifndef PFAD_POP
#define PFAD_POP        "../data/"
#endif

#define PFAD_AUTOGFX    PFAD_POP"grafik/autos/"
#define PFAD_LEVEL      PFAD_POP"level/"
#define PFAD_LEVELTEILE PFAD_POP"grafik/strecke/"

#endif
