/***************************************************************************
                          client.cc  -  description
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

#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <math.h>

#include "server.h"
#include "client.h"

// Class Clients

client *clients::findclient(s_client *adr)
{
	vector<client>::iterator i;
	for (i=clist.begin();i!=clist.end();i++) {
		if (i->a==adr) break;
	}
	if (i==clist.end())
		return NULL;

	return i;
}

client *clients::findclient(int i)
{
	return &(clist[i]);
}

client *clients::makenew(s_client *adr)
{
	client *tc=new client;

	tc->a=adr;
	clist.push_back(*tc);
	return tc;
}

void clients::delclient(s_client *adr)
{
	vector<client>::iterator i;

	for (i=clist.begin();i!=clist.end();i++) {
		if (i->a==adr) {
			clist.erase(i);
			break;	// Hmm, should search for others...
		}
	}
}

int clients::size()
{
	return clist.size();
}

void clients::calc()
{
	iterator i;

	// Schritt 1
	for (i=clist.begin();i!=clist.end();i++) {
		i->obj->calcpos();
	}

	// Schritt 2:
	vector <game_objekt> t;
	for (i=clist.begin();i!=clist.end();i++) {
		i->obj->calccoll(t);
	}
}
