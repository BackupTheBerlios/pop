// status.h
#ifndef __STATUS_H__
#define __STATUS_H__

#define STATUS_WAIT	0		// Auf Clients warten
#define STATUS_LOAD	1		// Spiel vorbereiten
#define STATUS_RUN	2		// Spiel läuft
#define STATUS_PAUSE	3		// Spielpause
#define STATUS_ERG	4		// Ergebnisse betrachten

int pops_getstatus();
void pops_statuswechsel(int num);

#endif

