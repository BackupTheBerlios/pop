// include this file with #include in your source

#include <iostream.h>

#ifndef RELATIV
        #define RELATIV 1
#endif

inline void swap(int &n1,int &n2)
{
	int t=n1;
	n1=n2;
	n2=t;
}

// Seite einer Linie bestimmen (rechts/ links)
inline int getsitel( int x1,int y1,int x2,int y2,float xp,float yp)
{
 int dx,dy;

 // Deltas berechnen
 if (x1>x2)
  dx=x1-x2; else dx=x2-x1;
 if (y1>y2)
  dy=y1-y2; else dy=y2-y1;

 if (dx>dy) {// Taktik 1 -> horizontal
  float m; // Geradengleichung: y=m*x+b
  float b;

  // ‹berhaupt testen?
  if (x1<x2) {
 	if (xp+RELATIV<x1 || xp-RELATIV>x2) return 0;
  } else {
	if (xp+RELATIV<x2 || xp-RELATIV>x1) return 0;
  }

  if (x2>x1) {
   b=y1;
   m=dy?dx/(y2-y1):0;
  } else {
   b=y2;
   m=dy?dx/(y1-y2):0;
  }
  if ((m*xp+b)>yp)
   return 1;
  else
   return -1;
 } else { // Taktik 2 -> vertikal
  float m; // Geradengleichung: x=m*y+b
  float b;

  // ‹berhaupt testen?
  if (y1<y2) {
	if (yp+RELATIV<y1 || yp-RELATIV>y2) return 0;
  } else {
	if (yp+RELATIV<y2 || yp-RELATIV>y1) return 0;
  }

  if (y2>y1) {
   b=x1;
   m=dx?dy/(x2-x1):0;
  } else {
   b=x2;
   m=dx?dy/(x1-x2):0;
  }
  if ((m*yp+b)>xp)
   return 1;
  else
   return -1;
 }
}

// Seite eines Kreises bestimmen (innerhalb/auﬂerhalb)
inline int getsitek(int xp, int yp, int rad, int t, float x, float y)
{
	float dx,dy;
	float alpha;

	dx=x- xp;
	dy=yp- y;	// Achtung, ykoordinate jetzt mathematisch von unten nach oben!
			// Macht aber das Denken in Winkeln leichter :)
	// Spezialf‰lle
	if (dx==0) {
		if (dx>rad) return 1; else return -1;
	}

	// Testen obs in den Bereich des Kreis f‰llt....
	alpha=atan(dy/dx);
	// Falls sich das Auto links vom Mittelpunkt befindet
	// muﬂ der Winkel angepaﬂt werden, da tan nur im Bereich -90 - 90 grad
	// liefert
	if (dx<0) alpha-=M_PI;

	if (alpha<0)
		alpha+=M_PI*2;
	if (alpha<0 || alpha>M_PI*2) {
		cout << "AError: %f" << alpha;
	}
	// Etwas mehr Spielraum lassen, so daﬂ grenzen zeischen Objekten funktionieren
	#define ADDSPACE	((float)RELATIV/50)
	switch (t) {
		case geom_kreisro: if (alpha>(M_PI/2)+ADDSPACE) return 0; break;
		case geom_kreislo: if (alpha>M_PI+ADDSPACE) return 0; if (alpha+ADDSPACE<(M_PI/2)) return 0; break;
		case geom_kreislu: if (alpha>(3*M_PI/2)+ADDSPACE) return 0; if (alpha+ADDSPACE<M_PI) return 0; break;
		case geom_kreisru: if (alpha+ADDSPACE<(3*M_PI/2)) return 0; break;
		default: break;
	}

	//printf ("X:%f Y:%f TP%d W:%3.0f S:%3.3f",dx,dy,t,alpha*180/M_PI,(float)dy/dx);

	// Innen/Auﬂen?
	float len=sqrt(dx*dx + dy*dy);
	if (len<0 || len>1000) {
		cout << "Len?!? " << len << "\n";
	}
	if ((int)len>rad) {
		return 1;
	} else
		return -1;
}


