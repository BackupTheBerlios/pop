/***************************************************************************
                          auto.cc  -  description
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
#include <malloc.h>
#include <iostream.h>
#include <fstream.h>
#include "auto.h"
#include "autos.h"

autotypen::autotypen()
{
}

autotypen::~autotypen()
{
}

autos autotypen::d;

int autotypen::load(const char *fname)
{
 return autos_load (&d,fname);
}

autodaten *autotypen::gettyp(int num)
{
 if (num>=0 && num<d.anz)
  return &d.d[num];
 else
  return NULL;
}

int autotypen::getanz()
{
 return d.anz;
}

