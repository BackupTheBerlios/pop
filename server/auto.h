/***************************************************************************
                          auto.h  -  description
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
#ifndef __AUTO_H__
#define __AUTO_H__

#include <fstream.h>

#include "fixed.h"
#include "config.h"
#include "autos.h"

/* **************
   * Auto Daten *
   **************/

class autotypen
{
public:
 autotypen();
 ~autotypen();

 static int load(const char *fname);
 static autodaten *gettyp(int num);
 static int getanz();

 static autos d;
};

#endif
