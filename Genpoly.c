/*********************************************
 * GENPOLY: GENERAZIONE DI POLIGONI ALEATORI *
 *********************************************/

#include <time.h> /* Consente di leggere l'ora di sistema   */
#include <stdio.h> /* Libreria standard i/o                  */
#include <stdlib.h> /* Libreria di conversione caratteri      */
enum { X,Y,DIM }; /* def. spazio bidimensionale             */
typedef enum   { FALSE,TRUE } bool;
typedef int   tPointi [DIM];
typedef tPointi *tPolygoni;


#if !defined (_SYS_TYPES_H)
 typedef unsigned int uint;
 typedef unsigned long ulong;
#endif


#define NPOINTS 100 /*Numero max vertici poligono di default*/
#define XMAX 256 /* Dimensione X della finestra di default */
#define YMAX 256 /* Dimensione Y                           */
#define max(A,B) ((A) > (B) ? (A) : (B))
#define min(A,B) ((A) < (B) ? (A) : (B))
#define abs(A)  ((A) > 0 ? (A) : (-(A)))
#define absdiff(A,B) ((A) > (B) ? (A) - (B) : (B) - (A))
#define C_POINT 255 /* Colore dei punti sul video             */
#define C_LINE 127 /* Colore delle linee sul video           */
#define C_NULL 0 /* Colore dello sfondo                    */

/**
 * Area doppia del triangolo
 */
int Area2 (tPointi a, tPointi b, tPointi c) {
return
 a[X]*b[Y]-a[Y]*b[X]+
 b[X]*c[Y]-b[Y]*c[X]+
 c[X]*a[Y]-c[Y]*a[X];
}

/**
 * Left=TRUE se il punto c e' a sx rispatto al vettore  a -> b
 * Collinear=TRUE se a,b,c sono punti collineari
 */

#define Left(a,b,c)   (Area2((a),(b),(c))>0)
#define LeftOn(a,b,c)  (Area2((a),(b),(c))>=0)
#define Collinear(a,b,c) (Area2((a),(b),(c))==0)


/**
 * Intersezione propria
 */
bool IntersectProp (tPointi a, tPointi b, tPointi c, tPointi d) {
return
 ((Left(a,b,c)&&!LeftOn(a,b,d)) || (!LeftOn(a,b,c)&&Left(a,b,d)))
 &&
 ((Left(c,d,a)&&!LeftOn(c,d,b)) || (!LeftOn(c,d,a)&&Left(c,d,b)));
}

/**
 * Between=TRUE se il punto c si trova sul segmento (a,b)
 */
bool Between (tPointi a, tPointi b, tPointi c) {
if (!Collinear(a,b,c))
 return FALSE;
if (a[X]!=b[X])
 return
 (((a[X]<=c[X])&&(c[X]<=b[X]))||
 ((b[X]<=c[X])&&(c[X]<=a[X])));
else
 return
 (((a[Y]<=c[Y])&&(c[Y]<=b[Y]))||
 ((b[Y]<=c[Y])&&(c[Y]<=a[Y])));
}

/**
 * Intersect=TRUE se la linea a-b ha un punto di intersezione con c-d
 */
bool Intersect (tPointi a, tPointi b, tPointi c, tPointi d) {
return IntersectProp(a,b,c,d)||
  Between(a,b,c)||
  Between(a,b,d)||
  Between(c,d,a)||
  Between(c,d,b);
}

/**
 * Crea una copia del punto a nel punto b
 */
CopyPoint (tPointi a, tPointi b) {
register uint dim;
for (dim=0; dim<DIM; dim++)
 b[dim]=a[dim];
}

/**
 * Crea una copia del poligono P nel poligono Pcopy
 */
CopyPoly (int n, tPolygoni P, tPolygoni Pcopy) {
register uint k;
for (k=0;k<n;k++)
 CopyPoint(P[k],Pcopy[k]);
}

/**
 * Aggiunge al poligono il punto v nella posizione i
 */
void CreateEar(uint i, uint v, uint *pnpoints, tPolygoni Points, uint *pnlati, tPolygoni Poly) {
register uint k;
for (k=++*pnlati; k>i+1; k--)
 CopyPoint(Poly[k-1],Poly[k]);
CopyPoint(Points[v],Poly[i+1]);
for (--*pnpoints,k=v; k<=*pnpoints; k++)
 CopyPoint(Points[k+1],Points[k]);
}

/**
 * Crea un vettore di n indici
 */

void IndPos(uint *vett, uint n) {
register uint k;
for(k=0;k<n;k++)
 vett[k]=k;
}

/**
 * Restituisce una posizione casuale all'interno del vettore e lo comprime
 */
int RandPos (uint *vett,uint *n) {
register uint i=rand()*rand()%*n;
register uint v=vett[i];
for(--*n;i<=*n;i++)
 vett[i]=vett[i+1];
return v;
}

/**
 * Genera punti pseudocasuali nel range (0,xmax),(0,ymax)
 */
void Randomize (tPolygoni P, int n, int xmax, int ymax) {
uint  k,posmax=xmax*ymax;
uint  *pos=calloc(posmax,sizeof(int));
IndPos(pos,posmax);
while (n--) {
 k=RandPos(pos,&posmax);
 P[n][X]=k%xmax;
 P[n][Y]=k/xmax;
 }
free(pos);
}

/**
 * Invia l'immagine allo standard output in formato Hips
 */
void OutHips (char *Screen, int xmax, int ymax) {
register uint k;
printf("HIPS\n\n\n1\n\n%d\n%d\n%d\n%d\n0\n0\n0\n1\n0\n1\n\n0\n  0\n",ymax,xmax,ymax,xmax);
for (k=0;k<xmax*ymax;k++)
  putchar(Screen[k]);
}

/**
 * Disegna una linea tra due punti
 */
void DrawLine (tPointi a, tPointi b, char *Screen, int xmax, int ymax) {
int i,
 x1=min(a[X],b[X]),
 x2=max(a[X],b[X]),
 y1=min(a[Y],b[Y]),
 y2=max(a[Y],b[Y]),
 dx=b[X]-a[X],
 dy=b[Y]-a[Y];
if ((x2-x1)>(y2-y1))
 for(i=x1;i<=x2;i++)
  Screen[i+((i-a[X])*dy/dx+a[Y])*xmax]=C_LINE;
else
 for(i=y1;i<=y2;i++)
  Screen[(i-a[Y])*dx/dy+a[X]+i*xmax]=C_LINE;
}
/**
 * Pulisce lo schermo prima di disegnare
 */
void ClearScreen (char *Screen, int xmax, int ymax) {
register ulong k;
for(k=0; k<xmax*ymax; k++)
 Screen[k]=C_NULL;
}
/**
 * Disegna i punti sullo schermo
 */
void DrawPoints (int n, tPolygoni P, char *Screen, int xmax, int ymax) {
while (n--)
 Screen[P[n][X]+P[n][Y]*xmax]=C_POINT;
}
/**
 * Disegna un poligono
 */
void DrawPoly (int n, tPolygoni P, char *Screen, int xmax, int ymax) {
register uint i;
for(i=1;i<n;i++)
 DrawLine(P[i-1],P[i],Screen,xmax,ymax);
DrawLine(P[n-1],P[0],Screen,xmax,ymax); /* chiusura poligono */
}

/**
 * Compatta un vettore di punti
 */
void Compatta (int i, int *pn, tPolygoni P) {
register uint k;
for (--*pn,k=i; k<*pn; k++)
 CopyPoint(P[k+1],P[k]);
}

/**
 * SwapPoint: scambia 2 punti
 */
void SwapPoint (tPointi a, tPointi b) {
tPointi t;
CopyPoint(a,t);
CopyPoint(b,a);
CopyPoint(t,b);
}

/**
 * DownHeap: procedura chiamata da HeapSort
 */
void DownHeap(int k, tPolygoni P, int n) {
register int i;
tPointi v;
CopyPoint(P[k],v);
while(k<=n>>1) {
 i=k<<1;
 if ((i<n)&&!Left(P[0],P[i],P[i+1]))
   i++;
 if (Left(P[0],v,P[i])) goto rtn;
 CopyPoint(P[i],P[k]);
 k=i;
 }
rtn:
CopyPoint(v,P[k]);
}

/**
 * HeapSort: procedura di ordinamento vettore in base all'angolo polare
 */
void HeapSort (tPolygoni P, int npoints) {
register int n,k,min;
for(min=0,n=1;n<npoints;n++)
 if(P[n][Y]<P[min][Y])
  min=n;
SwapPoint(P[0],P[min]);
for(n=npoints-1,k=npoints>>1; k>=1; k--)
 DownHeap(k,P,n);
do {
 SwapPoint(P[1],P[n]);
 DownHeap(1,P,--n);
 }
while (n>1);
}
/**
 * Costruisce il poligono convesso sui *pnpoints punti di P (Graham)
 */
int ConvexHull (tPolygoni P, uint *pnpoints, tPolygoni Poly) {
register uint n,i;
HeapSort(P,*pnpoints);
for(n=2,i=3;i<*pnpoints;i++,n++) {
 while (!LeftOn(P[n],P[n-1],P[i]))
  n--;
 SwapPoint(P[i],P[n+1]);
 }
CopyPoly(++n,P,Poly);
for(i=n; i<*pnpoints; i++)
 CopyPoint(P[i],P[i-n]);
*pnpoints-=n;
return n;
}
/**
 * GoodEar=TRUE se il punto p puo' essere aggiunto al poligono tra v e v+1
 */
bool GoodEar (int v, int p, int npoints, tPolygoni Points, int nlati, tPolygoni Poly) {
register int k,k1;
register int v1=(v+1)%nlati;
if (Collinear(Points[p],Poly[v],Poly[v1]))
 return FALSE;
for(k=0;k<nlati;k++) {
 k1=(k+1)%nlati;
 if((Intersect(Poly[v],Points[p],Poly[k],Poly[k1])&&(k!=v)&&(k1!=v))
 ||(Intersect(Points[p],Poly[v1],Poly[k],Poly[k1])&&(k!=v1)&&(k1!=v1)))
  return FALSE;
 }
for(k=0;k<p;k++)
 if(LeftOn(Points[p],Poly[v1],Points[k])
 &&LeftOn(Poly[v],Points[p],Points[k])
 &&LeftOn(Poly[v1],Poly[v],Points[k]))
  return FALSE;
for(k=p+1;k<npoints;k++)
 if(LeftOn(Points[p],Poly[v1],Points[k])
 &&LeftOn(Poly[v],Points[p],Points[k])
 &&LeftOn(Poly[v1],Poly[v],Points[k]))
  return FALSE;
return TRUE;
}
/**
 * Aggiunge un lato al poligono
 */
int CreatePoly(uint *lato, uint *pn, uint *pnpoints, tPolygoni Points, uint *pnlati, tPolygoni Poly) {
uint i,rp,rl,npoints=0;
uint *point=(uint *)calloc(*pnpoints,sizeof(int));
do {
 if (npoints==0) {
  if ((*pn)==0) return (-1); /* non converge */
  npoints=*pnpoints;
  IndPos(point,npoints); /* inizializza il vett. di indici */
  rl=RandPos(lato,pn);
  }
 rp=RandPos(point,&npoints);
 }
while (!GoodEar(rl,rp,*pnpoints,Points,*pnlati,Poly));
CreateEar(rl,rp,pnpoints,Points,pnlati,Poly);
for(i=0;i<(*pn);i++) /* aggiorna gli indici > rl */
 if(lato[i]>rl)
  lato[i]++;
lato[(*pn)++]=rl;  /* aggiunge gli indici dei 2 nuovi lati */
lato[(*pn)++]=rl+1;
free (point);
}

/**
 * ParseArgs: tratta i parametri della riga di comando
 */
int ParseArgs (int argc, char *argv[], bool *paflag, uint *npoints, uint *xmax, uint *ymax) {
char c,*prgname=argv[0];
newoption:
while(--argc>0 && (*++argv)[0]=='-')
 while (c=*++argv[0])
  switch (c) {
  case 'n':
   *npoints=atoi(++*argv);
   if (*npoints==FALSE) {
    *npoints=atoi(*++argv);
    --argc;
    }
   goto newoption;
  case 'a':
   *paflag=TRUE;
   break;
  case 's':
   *xmax=*ymax=atoi(++*argv);
   if (*xmax==FALSE) {
    *xmax=*ymax=atoi(*++argv);
    --argc;
    }
   if (argc>1)
    *ymax=atoi(*++argv);
   if (*ymax==FALSE) {
    *--argv;
    *ymax=*xmax;
    }
   else
    --argc;
   goto newoption;
  default:
   fprintf(stderr,"%s: illegal option -- %c\n",prgname,c);
   fprintf(stderr,"Usage: %s [-n nedges] [-a | -s xmax [ymax]]\n",prgname);
   return -1;
  }
}
/**
 * Output in formato testo (coordinata X, coordinata Y) per ogni punto
 */
void Outascii (register int k, tPolygoni Poly) {
while (k--)
 printf("%d,%d\n",Poly[k][X],Poly[k][Y]);
}


/*******
 * M A I N
 */
main (int argc, char *argv[]) {
bool aflag=FALSE; /* output ascii */
uint n,npoints,nlati,*lato,
 xmax=XMAX,ymax=YMAX;
tPolygoni Poly,Points;
srand(time(NULL));
npoints=rand()%(NPOINTS-3)+3; /* numero casuale 3-NPOINTS lati */
if (ParseArgs(argc,argv,&aflag,&npoints,&xmax,&ymax)==-1)
 return FALSE;
lato=(uint *)calloc(npoints,sizeof(int));
Points=(tPolygoni)calloc(npoints,sizeof(tPointi));
Poly=(tPolygoni)calloc(npoints,sizeof(tPointi));
Randomize(Points,npoints,xmax,ymax);
nlati=ConvexHull(Points,&npoints,Poly);
IndPos(lato,n=nlati);
while (npoints)
 if (CreatePoly(lato,&n,&npoints,Points,&nlati,Poly)==-1) {
  fprintf(stderr,"Nonconvergence\n");
  break;
  }
free (lato);
if (aflag)
 Outascii(nlati,Poly);    /* Output in formato coordinate ascii */
else {
 char *Screen=malloc(xmax*ymax);
 ClearScreen(Screen,xmax,ymax);
 DrawPoly(nlati,Poly,Screen,xmax,ymax);     /* Disegna il poligono */
 DrawPoints(nlati,Poly,Screen,xmax,ymax);   /* Disegna i vertici   */
 if (npoints) /* Se l'algoritmo non converge, mancano da disegnare */
  DrawPoints(npoints,Points,Screen,xmax,ymax);/*questi punti*/
 OutHips(Screen,xmax,ymax);     /* Output in formato standard Hips */
 free(Screen);
 }
free (Points);
free (Poly);
return TRUE;
}
